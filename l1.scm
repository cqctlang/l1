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

(define make-ctx
  (lambda (val range type addr)
    `((val . ,val)
      (range . ,range)
      (type . ,type)
      (addr . ,addr))))

(define lookvar
  (lambda (var ctx)
    (cdr (assq var ctx))))

(define compile0
  (lambda (e)
    (compile0* e (make-ctx #f #f #f #f))))

(define newvar
  (let ((n 0))
    (lambda (sym)
      (set! n (+ n 1))
      (string->symbol (string-append (symbol->string sym)
				     (number->string n))))))

;; *(p+1) = x;

;; cases:
;;  need cvalue 
;;  need location
;;  

(define compile0*
  (let ((n 0))
    (lambda (e ctx)
      (let* ((thisblock '())
	     (newvar (lambda (sym)
		       (set! n (+ n 1))
		       (let ((var (string->symbol
				   (string-append (symbol->string sym)
						  (number->string n)))))
			 (set! thisblock (cons var thisblock))
			 var))))
	(case (car e)
	  ((Etick)
	   (let ((val (lookvar 'val ctx))
		 (range (cond ((lookvar 'range ctx)) (else (newvar 'range))))
		 (type (cond ((lookvar 'type ctx)) (else (newvar 'type))))
		 (addr (cond ((lookvar 'addr ctx)) (else (newvar 'addr)))))
	     (let ((body
		    (list `(Eg p (Ecall "looksym" ,(list-ref e 1)))
			  `(Eg ,type (Ecar p))
			  `(Eg ,addr (Ecdr p))
			  `(Eg ,range (Erange ,addr (Esizeof ,type))))))
	       `(Eblock ,thisblock
		 ,(if val
		      (append body (list `(Eg ,val (Ecall "get" ,range))))
		      body)))))
	  ((Eg)
	   (let* ((val (cond ((lookvar 'val ctx)) (else (newvar 'val))))
		  (range (newvar 'range))
		  (type (newvar 'type))
		  (addr (newvar 'addr)))
	     `(Eblock ,thisblock
	       ,(compile0* (list-ref e 2) (make-ctx val #f #f #f))
	       ,(compile0* (list-ref e 1) (make-ctx #f range addr type))
	       (Ecall "put" ,range ,val))))
	  (else
	   (error 'long-way-to-go)))))))


