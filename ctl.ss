(define t1
  '(seq (primop v1 v2) (primop w1 w2)))

(define t2
  '(seq (if p (return v1) (primop w1 w2))
	(primop x1 x2)))

(define value?
  (lambda (M)
    (not (list? M))))

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
	  [(primop)
	   (normalize-name* (cdr M) (lambda (t*) (k `(primop . ,t*))))]
	  [else
	   (error "bad form")]))))

(define newvar
  (let ((x 0))
    (lambda ()
      'fixme)))
   
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
