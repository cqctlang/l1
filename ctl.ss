(define t0
  '(+ (+ 2 2) (let (x 1) (+ x))))

(define t1
  `(seq
    (+ 1 1)
    (seq
     (if 2
	 (seq
	  (+ 2 2)
	  (+ 3 3))
	 (+ 4 4))
     (+ 5 5))))

(define t2
  `(seq
    (+ 1 1)
    (seq
     (if 2
	 (seq
	  (+ 2 2)
	  (+ 3 3))
	 (return (+ 4 4)))
     (+ 5 5))))

(define t3
  `(seq
    (+ 1 1)
    (seq
     (if 2
	 (seq
	  (+ 2 2)
	  (+ 3 3))
	 (return (+ 4 4)))
     (seq
      (if 5
	  (seq
	   (+ 6 6)
	   (return 7))
	  (+ 8 8))
      (+ 9 9)))))

(define value?
  (lambda (M)
    (not (list? M))))

(define X
  (lambda (M k)
    (if (value? M)
	(k M)
	(case (car M)
	  [(seq)
	   (let ((M1 (list-ref M 1))
		 (M2 (list-ref M 2)))
	     (X M1 (lambda (N1)
		     `(seq ,N1 ,(X M2 k)))))]
	  [(return)
	   (let ((M1 (list-ref M 1)))
	     M1)]
	  [(if)
	   (let ((M1 (list-ref M 1))
		 (M2 (list-ref M 2))
		 (M3 (list-ref M 3)))
	     (X M1 (lambda (t)
		     `(if ,t
			   ,(X M2 k)
			   ,(X M3 k)))))]
	  [(+)
	   (X-name* (cdr M) (lambda (t*) (k `(+ . ,t*))))]
	  [else
	   (error 'X "bad form" M)]))))

(define X-name
  (lambda (M k)
    (format #t "X-Name ~a~%" M)
    (X M (lambda (N)
	   (if (value? N)
	       (k N)
	       (let ((t (newvar)))
		 `(let (,t ,N) ,(k t))))))))

(define X-name*
  (lambda (M* k)
    (if (null? M*)
	(k '())
	(X-name (car M*)
			(lambda (t)
			  (X-name* (cdr M*)
				   (lambda (t*) (k `(,t . ,t*)))))))))

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

(define newvar
  (let ((x 0))
    (lambda ()
      (set! x (+ x 1))
      (string->symbol (format #f "t~a" x)))))

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
