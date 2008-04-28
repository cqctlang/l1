(define s0 '(seq
	     (= x (+ (+ 1 2) (+ 3 4)))
	     (= y (+ (+ 5 6) (+ 7 8)))))

(define s1 '(seq
	     (= x (+ (+ (+ 0 1) (+ 2 3)) (+ (+ 5 6) (+ 7 8))))
	     (= y (+ (+ 5 6) (+ 7 8)))))

(define simple?
  (lambda (e)
    (or (number? e) (symbol? e))))

(define rand
  (lambda (e p)
    (cond
     ((number? e) e)
     ((symbol? e)
      (cdr (assq e p)))
     (else
      (error 'bad-operand)))))

(define cg
  (lambda (e p dst tmp)
    (case (car e)
      ((seq)
       (cg (list-ref e 1) p 'effect tmp)
       (cg (list-ref e 2) p dst tmp))
      ((=)
       (let* ((l (list-ref e 1))
	      (r (list-ref e 2))
	      (d (cdr (assq l p))))
	 (cg r p d tmp)))
      ((+)
       (let ((e1 (list-ref e 1))
	     (e2 (list-ref e 2)))
	 (cond ((and (simple? e1) (simple? e2))
		(format #t "add ~a ~a ~a~%"
			(rand e1 p)
			(rand e2 p)
			dst))
	       ((simple? e1)
		(cg e2 p 'acc tmp)
		(format #t "add ~a 'acc ~a~%"
			(rand e1 p)
			dst))
	       ((simple? e2)
		(cg e1 p 'acc tmp)
		(format #t "add 'acc ~a ~a~%"
			(rand e2 p)
			dst))
	       (else
		(let ((rand (format #f "t~a" tmp)))
		  (cg e1 p rand tmp)
		  (cg e2 p 'acc (+ tmp 1))
		  (format #t "add ~a acc ~a~%" rand dst))))))
      (else
       (error 'bad-term)))))

(define p0
  (lambda (e)
    (let loop ((e e) (n 0) (p '()))
      (if (list? e)
	  (case (car e)
	    ((seq = +)
	     (let ((l (loop (list-ref e 1) n p)))
	       (loop (list-ref e 2) (length l) l)))
	    (else
	     (error 'bad-term)))
	  (cond
	   ((number? e) p)
	   ((symbol? e)
	    (if (assq e p)
		p
		(cons (cons e (format #f "l~a" n)) p)))
	   (else
	    (error 'bad-term)))))))

(define main
  (lambda (e)
    (let* ((p (p0 e))
	   (t (length p)))
      (cg e p 'acc (length p)))))


