(define t0
  '(+ (+ 2 2) (let (x 1) (+ x))))

(define t1
  `(seq
    1
    (seq
     (if 2 (seq 3 4) 5)
     6)))

(define t2
  `(seq
    1
    (seq
     (if 2 (seq 3 4) (return 5))
     6)))

(define t3
  `(seq
    1
    (seq
     (if 2
	 (seq
	  (if 3 (return 4) (set! v 5))
	  (set! v 6))
	 (set! v 7))
     (set! v 8))))

(define value?
  (lambda (M)
    (not (list? M))))

(define newvar
  (let ((x 0))
    (lambda ()
      (set! x (+ x 1))
      (string->symbol (format #f "t~a" x)))))

(define jumps?
  (lambda (e)
    (if (value? e)
	#f
	(case (car e)
	  ((seq)
	   (or (jumps? (list-ref e 1))
	       (jumps? (list-ref e 2))))
	  ((set!)
	   (jumps? (list-ref e 2)))
	  ((if)
	   (or (jumps? (list-ref e 1))
	       (jumps? (list-ref e 2))
	       (jumps? (list-ref e 3))))
	  ((+)
	   (let loop ((t* (cdr e)))
	     (cond
	      ((null? t*) #f)
	      ((jumps? (car t*)) #t)
	      (else (loop (cdr t*))))))
	  ((return) #t)
	  (else
	   (error 'jumps "bad form" e))))))

(define saw
  (lambda (M ctl)
    (if (value? M)
	(ctl M)
	(case (car M)
	  ((seq)
	   (let ((M1 (list-ref M 1))
		 (M2 (list-ref M 2)))
	     (if (jumps? M1)
		 (let ((l (newvar)))
		   `(letrec ((,l (lambda () ,(saw M2 ctl))))
		      ,(saw M1 (lambda (v) `(,l)))))
		 `(seq ,(saw M1 'effect) ,(saw M2 ctl)))))
	  ((set!)
	   (let ((v (list-ref M 1))
		 (V (list-ref M 2)))
	     (if (jumps? V)
		 (let ((l newvar))
		   `(letrec ((,l (lambda (val)
				   (set! ,v ,val)
				   ,(ctl nil))))
		      ,(saw V (lambda (v) `(,l v)))))
		 `(set! ,v ,(saw V 'value)))))
	  ((if)
	   (let ((P (list-ref M 1))
		 (C (list-ref M 2))
		 (A (list-ref M 3)))
	     `(if ,P ,(saw C ctl) ,(saw A ctl))))
	  ((return)
	   (let ((rv (list-ref M 1)))
	     (ctl rv)))
	  (else
	   (error 'saw "bad form" M))))))

(define normalize
  (lambda (M k)
    (if (value? M)
	(k M)
	(case (car M)
	  [(lambda)
	   (let ((params (list-ref M 1))
		 (body (list-ref M 2)))
	     (k `(lambda ,params ,(normalize-term body))))]
	  [(let)
	   (let ((bf (list-ref M 1))
		 (M2 (list-ref M 2)))
	     (let ((x (list-ref bf 0))
		   (M1 (list-ref bf 1)))
	       (normalize M1 (lambda (N1) `(let (,x ,N1) ,(normalize M2 k))))))]
	  [(if0)
	   (let ((M1 (list-ref M 1))
		 (M2 (list-ref M 2))
		 (M3 (list-ref M 3)))
	     (normalize-name M1 (lambda (t)
				  (k `(if0 ,t
					   ,(normalize-term M2)
					   ,(normalize-term M3))))))]
	  [(+)
	   (normalize-name* (cdr M) (lambda (t*) (k `(+ . ,t*))))]
	  [else
	   (error 'normalize "bad form" M)]))))

(define normalize-term
  (lambda (M)
    (normalize M (lambda (x) x))))

(define normalize-name
  (lambda (M k)
    (normalize M (lambda (N)
		   (if (value? N)
		       (k N)
		       (let ((t (newvar)))
			 `(let (,t ,N) ,(k t))))))))

(define normalize-name*
  (lambda (M* k)
    (if (null? M*)
	(k '())
	(normalize-name (car M*)
			(lambda (t)
			  (normalize-name* (cdr M*)
					   (lambda (t*) (k `(,t . ,t*)))))))))
