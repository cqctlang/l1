(define intersect
  (lambda (s t)
    (let loop ((s s) (rv '()))
      (cond
       ((null? s) rv)
       ((memq (car s) t)
	(loop (cdr s) (cons (car s) rv)))
       (else
	(loop (cdr s) rv))))))

(define union
  (lambda (s t)
    (let loop ((s s) (rv t))
      (cond
       ((null? s) rv)
       ((memq (car s) t)
	(loop (cdr s) rv))
       (else
	(loop (cdr s) (cons (car s) rv)))))))

(define mkx
  (lambda (sym idx)
    (cons sym idx)))

(define xsym
  (lambda (x)
    (car x)))

(define xidx
  (lambda (x)
    (cdr x)))

(define xdiff
  (lambda (xset syms)
    (let loop ((xset xset) (rv '()))
      (cond
       ((null? xset)
	rv)
       ((memq (xsym (car xset)) syms)
	(loop (cdr xset) rv))
       (else
	(loop (cdr xset) (cons (car xset) rv)))))))

(define xunion
  (lambda (xs xt)
    (let loop ((xt xt) (rv xs))
      (cond
       ((null? xt) rv)
       ((assq (xsym (car xt)) xs)
	=> (lambda (x)
	     (if (eq? (xidx x) (xidx (car xt)))
		 (loop (cdr xt) rv)
		 (loop (cdr xt) (cons (car xt) rv)))))
       (else
	(loop (cdr xt) (cons (car xt) rv)))))))

(define mkids
  (lambda (vars)
    (let loop ((i 0) (vars vars) (rv '()))
      (cond ((null? vars) (reverse rv))
	    (else
	     (loop (+ i 1) (cdr vars) (cons (mkx (car vars) i) rv)))))))

(define mklocal
  (lambda (vars oldl nl)
    (xunion (xdiff oldl vars)
	    (let loop ((i nl) (vars vars) (rv '()))
	      (cond
	       ((null? vars) (reverse rv))
	       (else
		(loop (+ i 1) (cdr vars) (cons (mkx (car vars) i) rv))))))))

(define free-vars
  (lambda (e v)
    (cond
     ((not (list? e))
      (error 'bad-expr))
     ((symbol? (car e))
      (case (car e)
	((ref)
	 (let ((var (list-ref e 1)))
	   (if (memq var v)
	       '()
	       (list var))))
	((lambda block)
	 (let ((vars (list-ref e 1))
	       (body (cddr e)))
	   (free-vars body (union vars v))))
	(else
	 (format #t "unknown form ~a~%" (car e))
	 (error 'unknown-form))))
     (else
      (let loop ((e e) (rv '()))
	(if (null? e)
	    rv
	    (loop (cdr e) (union (free-vars (car e) v) rv))))))))

(define local-vars
  (lambda (e l nl)
    (let loop ((e e) (l l))
      (cond
       ((not (list? e))
	(error 'bad-expr))
       ((null? e)
	'())
       ((symbol? (car e))
	(case (car e)
	  ((ref)
	   '())
	  ((lambda)
	   '())
	  ((block)
	   (let* ((vars (list-ref e 1))
		  (body (cddr e))
		  (newl (mklocal vars l nl)))
	     (set! nl (+ nl (length newl)))
	     (xunion newl (loop body newl))))
	  (else
	   (format #t "unknown form ~a~%" (car e))
	   (error 'unknown-form))))
       (else
	(xunion (loop (car e) l)
		(loop (cdr e) l)))))))

(define genref
  (lambda (v p l f)
    (cond
     ((assq v p)
      => (lambda (x)
	   (list 'paramref x)))
     ((assq v l)
      => (lambda (x)
	   (list 'localref x)))
     ((assq v f)
      => (lambda (x)
	   (list 'freeref x)))
     (else
      (list 'topref v)))))

(define mapfn
  (lambda (e p l f nl)
    (cond
     ((not (list? e))
      (error 'bad-expr))
     ((symbol? (car e))
      (case (car e)
	((ref)
	 (let ((var (list-ref e 1)))
	   (format #t "~a~%" (genref var p l f)))
	 nl)
	((lambda)
	 (let* ((vars (list-ref e 1))
		(body (cddr e))
		(newp (mkids vars))
		(newl '())
		(newf (intersect
		       (union (map xsym p)
			      (union (map xsym l)
				     (map xsym f)))
		       (free-vars body vars))))
	   (format #t "lambda ~a " (list-ref e 1))
	   (format #t "locals ~a " (local-vars body '() 0))
	   (format #t "collect ~a~%" 
		   (map
		    (lambda (v)
		      (genref v p l f))
		    newf))
	   (mapfn body newp newl (mkids newf) nl)))
	((block)
	 (let* ((vars (list-ref e 1))
		(body (cddr e))
		(newl (mklocal vars l nl))
		(newnl (+ nl (length newl)))
		(newp (xdiff p vars)))
	   (format #t "block ~a~%" (list-ref e 1))
	   (mapfn body newp newl f newnl)))
	(else
	 (format #t "unknown form ~a~%" (car e))
	 (error 'unknown-form))))
     (else
      (let loop ((e e) (nl nl))
	(if (null? e)
	    nl
	    (loop (cdr e) (mapfn (car e) p l f nl))))))))


(define vparam
  (lambda (nc vars)
    (let loop ((vars vars) (i 0) (rv '()))
      (cond
       ((null? vars)
	(reverse rv))
       (else
	(loop (cdr vars) (+ i 1) (cons (list (car vars) 'param nc i) rv)))))))

(define vlocal
  (lambda (nc nl vars)
    (let loop ((vars vars) (rv '()))
      (cond
       ((null? vars)
	(reverse rv))
       (else
	(loop (cdr vars) (cons (list (car vars) 'local nc (nl)) rv)))))))

(define shadow
  (lambda (old new)
    (append new old)))

(define genref1
  (lambda (var vs)
    (cond
     ((assq var vs)
      => (lambda (x) x))
     (else
      (list var 'toplevel)))))

(define mkcnt
  (lambda ()
    (let ((nl 0))
      (lambda ()
	(let ((rv nl))
	  (set! nl (+ nl 1))
	  rv)))))


(define mkref
  (lambda (var)
    (vector 'ref var)))

(define ref-var
  (lambda (ref)
    (vector-ref ref 1))) 

(define mkset
  (lambda (var val)
    (vector 'set! var val)))

(define set-var
  (lambda (set)
    (vector-ref set 1)))

(define set-val
  (lambda (set)
    (vector-ref set 2)))

(define mklambda
  (lambda (param nc body)
    (vector 'lambda param nc body)))

(define lambda-param
  (lambda (l)
    (vector-ref l 1)))

(define lambda-nc
  (lambda (l)
    (vector-ref l 2)))

(define lambda-body
  (lambda (l)
    (vector-ref l 3)))

(define mkblock
  (lambda (locals body)
    (vector 'block locals body)))

(define block-locals
  (lambda (b)
    (vector-ref b 1)))

(define block-body
  (lambda (b)
    (vector-ref b 2)))

(define mape1
  (lambda (e)
    (let ((cnt (mkcnt)))
      (let loop ((e e) (vs '()) (nc (cnt)) (nl (mkcnt)))
	(cond
	 ((not (list? e))
	  (error 'bad-expr))
	 ((symbol? (car e))
	  (case (car e)
	    ((ref)
	     (let ((var (list-ref e 1)))
	       (cons 'ref (list (genref1 var vs)))))
	    ((set!)
	     (let ((var (list-ref e 1))
		   (val (list-ref e 2)))
	       (cons 'set! (list (genref1 var vs)
				 (loop val vs nc nl)))))
	    ((lambda)
	     (let* ((nc (cnt))
		    (vars (list-ref e 1))
		    (body (cddr e))
		    (ps (vparam nc vars)))
	       (cons 'lambda
		     (cons nc
			   (cons ps
				 (loop body (shadow vs ps) nc (mkcnt)))))))
	    ((block)
	     (let* ((vars (list-ref e 1))
		    (body (cddr e))
		    (ls (vlocal nc nl vars)))
	       (cons 'block (cons ls (loop body (shadow vs ls) nc nl)))))
	    (else
	     (format #t "unknown form ~a~%" (car e))
	     (error 'unknown-form))))
	 (else
	  (map (lambda (e) (loop e vs nc nl)) e)))))))

(define union-equal
  (lambda (s t)
    (let loop ((s s) (rv t))
      (cond
       ((null? s) rv)
       ((member (car s) t)
	(loop (cdr s) rv))
       (else
	(loop (cdr s) (cons (car s) rv)))))))

(define local-locals
  (lambda (e)
    (cond
     ((not (list? e))
      (error 'bad-expr))
     ((symbol? (car e))
      (case (car e)
	((ref) '())
	((set!) (local-locals (list-ref e 2)))
	((lambda) '())
	((block) (union-equal (list-ref e 1)
			      (local-locals (cddr e))))
	(else
	 (format #t "unknown form ~a~%" (car e))
	 (error 'unknown-form))))
     (else
      (let loop ((e e) (rv '()))
	(cond ((null? e) rv)
	      (else
	       (loop (cdr e)
		     (union-equal (local-locals (car e)) rv)))))))))

(define lift-locals
  (lambda (e)
    (cond
     ((not (list? e))
      (error 'bad-expr))
     ((symbol? (car e))
      (case (car e)
	((ref) e)
	((set!)
	 (cons 'set! (list (list-ref e 1) (lift-locals (list-ref e 2)))))
	((lambda)
	 (let ((nc (list-ref e 1))
	       (vars (list-ref e 2))
	       (body (cdddr e)))
	   (cons 'lambda (cons nc (cons (append vars (local-locals body))
					(lift-locals body))))))
	((block)
	 (cons 'block (cons (list-ref e 1) (lift-locals (cddr e)))))
	(else
	 (format #t "unknown form ~a~%" (car e))
	 (error 'unknown-form))))
     (else
      (map lift-locals e)))))

(define mape2
  (lambda (e)
    (lift-locals e)))

(define find-sets
  (lambda (e)
    (cond
     ((not (list? e))
      (error 'bad-expr))
     ((symbol? (car e))
      (case (car e)
	((ref)
	 '())
	((set!)
	 (union-equal
	  (list (list-ref e 1))
	  (find-sets (list-ref e 2))))
	((block)
	 (let ((body (cddr e)))
	   (find-sets body)))
	((lambda)
	 (let ((body (cdddr e)))
	   (find-sets body)))
	(else
	 (format #t "unknown form ~a~%" (car e))
	 (error 'unknown-form))))
     (else
      (let loop ((e e) (rv '()))
	(cond ((null? e) rv)
	      (else
	       (loop (cdr e)
		     (union-equal (find-sets (car e)) rv)))))))))

(define mkbox
  (lambda (var)
    (append var (list 'box))))

(define mkboxes
  (lambda (vars sets)
    (map (lambda (v)
	   (if (member v sets)
	       (mkbox v)
	       v))
	 vars)))

(define label-boxes
  (lambda (e)
    (let ((sets (find-sets e)))
      (let loop ((e e))
	(cond
	 ((not (list? e))
	  (error 'bad-expr))
	 ((symbol? (car e))
	  (case (car e)
	    ((ref)
	     (let ((var (list-ref e 1)))
	       (if (member var sets)
		   (list 'ref (mkbox var))
		   e)))
	    ((set!)
	     (let ((var (list-ref e 1))
		   (val (list-ref e 2)))
	       (cons 'set! (list (mkbox var) (loop val)))))
	    ((lambda)
	     (let ((nc (list-ref e 1))
		   (vars (list-ref e 2))
		   (body (cdddr e)))
	       (cons 'lambda
		     (cons nc (cons (mkboxes vars sets)
				    (loop body))))))
	    ((block)
	     (cons 'block (cons (list-ref e 1) (loop (cddr e)))))
	    (else
	     (format #t "unknown form ~a~%" (car e))
	     (error 'unknown-form))))
	 (else
	  (map loop e)))))))

(define mape3
  (lambda (e)
    (label-boxes e)))

(define free-vars
  (lambda (e bound)
    (letrec ((freevar?
	      (lambda (var bound)
		(let ((kind (list-ref var 1)))
		  (and (not (eq? kind 'toplevel))
		       (not (member var bound)))))))
      (let loop ((e e) (bound bound))
	(cond
	 ((not (list? e))
	  (error 'bad-expr))
	 ((symbol? (car e))
	  (case (car e)
	    ((ref)
	     (let ((var (list-ref e 1)))
	       (if (freevar? var bound)
		   (list var)
		   '())))
	    ((set!)
	     (let ((var (list-ref e 1))
		   (val (list-ref e 2)))
	       (union-equal (if (freevar? var bound) (list var) '())
			    (loop val bound))))
	    ((lambda)
	     (let ((vars (list-ref e 2))
		   (body (cdddr e)))
	       (loop body (shadow bound vars))))
	    ((block)
	     (let ((body (cddr e)))
	       (loop body bound)))
	    (else
	     (format #t "unknown form ~a~%" (car e))
	     (error 'unknown-form))))
	 (else
	  (let loop1 ((e e) (rv '()))
	    (cond ((null? e) rv)
		  (else
		   (loop1 (cdr e)
			  (union-equal (loop (car e) bound) rv)))))))))))

(define find-collected
  (lambda (var collected)
    (let loop ((from (car collected)) (to (cdr collected)))
      (cond
       ((null? from) #f)
       ((equal? var (car from)) (car to))
       (else (loop (cdr from) (cdr to)))))))

(define do-collect
  (lambda (free collected return)
    (let loop ((free free) (collect '()) (from '()) (to '()) (i 0))
      (cond
       ((null? free) (return (reverse collect)
			     (cons (reverse from) (reverse to))))
       ((find-collected (car free) collected)
	=> (lambda (cur)
	     (loop (cdr free)
		   (cons cur collect)
		   (cons (car free) from)
		   (cons (list 'free i) to)
		   (+ i 1))))
       (else
	(loop (cdr free)
	      (cons (car free) collect)
	      (cons (car free) from)
	      (cons (list 'free i) to)
	      (+ i 1)))))))

(define collect-free
  (lambda (e)
    (let loop ((e e) (collected '()))
      (cond
       ((not (list? e))
	(error 'bad-expr))
       ((symbol? (car e))
	(case (car e)
	  ((ref)
	   (let ((var (list-ref e 1)))
	     (cond
	      ((find-collected var collected)
	       => (lambda (cur)
		    (list 'ref cur)))
	      (else
	       e))))
	  ((set!)
	   (let ((var (list-ref e 1))
		 (val (list-ref e 2)))
	     (let ((newvar (cond
			    ((find-collected var collected)
			     => (lambda (cur) cur))
			    (else var))))
	       (cons 'set! (list newvar (loop val collected))))))
	  ((lambda)
	   (let* ((nc (list-ref e 1))
		  (vars (list-ref e 2))
		  (body (cdddr e))
		  (free (free-vars body vars)))
	     (do-collect free collected
	      (lambda (collect collected)
		(cons 'lambda
		      (cons nc 
			    (cons collect
				  (cons vars
					(loop body collected)))))))))
	  ((block)
	   (cons 'block (cons (list-ref e 1) (loop (cddr e) collected))))
	  (else
	   (format #t "unknown form ~a~%" (car e))
	   (error 'unknown-form))))
       (else
	(map (lambda (e) (loop e collected)) e))))))

(define mape4
  (lambda (e)
    (collect-free e)))

(define test
  (lambda (e)
    (format #t "input:~%")
    (pretty-print e)
    (let ((e1 (mape1 e)))
      (format #t "mape1:~%")
      (pretty-print e1)
      (let ((e2 (mape2 e1)))
	(format #t "mape2:~%")
	(pretty-print e2)
	(let ((e3 (mape3 e2)))
	  (format #t "mape3:~%")
	  (pretty-print e3)
	  (let ((e4 (mape4 e3)))
	    (format #t "mape4:~%")
	    (pretty-print e4)))))))

(define test1
  '(lambda (a) (block (b c d) (ref a))))

(define test2
  '(lambda (a) (block (a b c d) (ref a))))

(define test3
  '(lambda (a) (block (b c d) (ref c))))

(define test4
  '(lambda (a) (block (b c d) (ref e))))

(define test5
  '(lambda (a)
     (block (b c d)
	    (ref a)
	    (ref b))
     (block (b c d)
	    (ref a)
	    (ref b))))

(define test6
  '(lambda (a)
     (block (b c d)
	    (lambda (c)
	      (ref a)
	      (ref b)
	      (ref c)))))

(define test8
  '(lambda (a)
     (block (b c d)
	    (ref e)
	    (lambda (c)
	      (ref e)
	      (ref a)
	      (ref b)
	      (ref c)))
     (block (b c d)
	    (ref e)
	    (lambda (c)
	      (ref e)
	      (ref a)
	      (ref b)
	      (ref c)))))

(define test9
  '(lambda (a)
     (block ()
	    (lambda ()
	      (lambda ()
		(ref a))))))

(define test10
  '(lambda (a)
     (block (b)
	    (lambda (c)
	      (set! c (ref a))
	      (set! b (ref c))))))

(define test11
  '(lambda (a)
     (block (b)
	    (lambda (c)
	      (set! c (set! b (ref a)))))))

(define test12
  '(lambda (a)
     (block (b)
	    (block (a)
		   (set! a (ref b)))
	    (set! a (ref b)))))

(define test13
  '(lambda (a)
     (lambda (b)
       (lambda (c)
	 (ref a)
	 (ref b)
	 (ref c)))))

(define test14
  '(lambda (a b c d)
     (lambda (b c)
       (ref d)
       (lambda (c)
	 (ref a)
	 (ref b)
	 (ref c)))))

(define test14
  '(lambda (a b c d)
     (lambda (b c)
       (ref d)
       (lambda (c)
	 (ref a)
	 (ref b)
	 (ref c)))))
