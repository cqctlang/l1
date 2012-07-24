;; mkdoc -- expand references to cinquecento code and evaluations
;;
;; usage: (mkdoc file)
;;   FILE is a file name of form X.src.Y
;;   creates a new file named X.Y whose contents are an expansion
;;   the contents of FILE.
;;   expansions occur to directives in either of the following forms
;;      @@include(FILE:FUNC)
;;      @@eval(FILE:FUNC)
;;
;;   these directives must begin at the beginning of a line and be
;;   the only contents on the line.
;;
;;   @@include(FILE:FUNC) includes the definition of the cinquecento
;;   function FUNC in file FILE.
;;   example:
;;
;;   	@@include(../demo/examples.cqct:example1)
;;
;;   @@eval(FILE:FUNC) includes the result of calling FUNC() after
;;   loading FILE in a cinquecento session.
;;   example:
;;
;;      @@eval(../demo/examples.cqct:example1)
;;
;; a batch invocation might look like:
;; 	% emacs --batch -l ./mkdoc.el --eval "(mkdoc \"foo.src.html\")"

(defun output-file (file)
  (let ((sp (split-string file "\\.src")))
    (if (not (= (length sp) 2))
	(error "invalid file name: %s" file))
    (apply 'concat sp)))

(defun find-text (file func)
  (let ((buf (find-file-noselect file))
	b e text)
    (save-excursion
      (set-buffer buf)
      (goto-char (point-min))
      (if (not (re-search-forward (concat "^@define " func) nil t))
	  (error "cannot find definition of %s in %s" func file))
      (beginning-of-line)
      (setq b (point))
      (if (not (re-search-forward "^}" nil t))
	  (error "cannot find definition of %s in %s" func file))
      (forward-line 1)
      (setq e (point))
      (setq text (buffer-substring b e)))
    (kill-buffer buf)
    text))

(defun strip-warnings ()
  (goto-char (point-min))
  (while (re-search-forward "^.*warning: .*\n" nil t)
    (replace-match "" nil nil)))

(defun eval-l1 (file func)
  (let ((cwd (file-name-as-directory
	      (file-name-directory buffer-file-name)))
	(wd (file-name-directory file))
	(lf (file-name-nondirectory file))
	(buf (generate-new-buffer "*l1*"))
	out)
    (save-excursion
      (set-buffer buf)
      (setq default-directory (concat cwd wd))
      ;; how do we give relative program names to call-process? (eg "../l1")
      ;; seems like '..' gets appended to each component of PATH
      (or (eq 0 (call-process (concat cwd "../l1") nil buf nil "-e" "runexample" lf func))
	  (error "l1 raised error calling %s from %s:\n%s" func file
		 (buffer-substring (point-min) (point-max))))
      (strip-warnings)
      (setq out (buffer-substring (point-min) (point-max))))
    (kill-buffer buf)
    (concat
     "; " func "();\n"
     "<span class=\"arg\">"
     out
     "</span>; \n")))

(defun splice (how spec)
  (let* ((sp (or (split-string spec ":")
		 (error "bad include specifier: %s" spec)))
	 (file (elt sp 0))
	 (func (elt sp 1))
	 (text (cond
		((eq how 'include)
		 (find-text file func))
		((eq how 'eval)
		 (eval-l1 file func))
		(t
		 (error "bug: %s" how)))))
    (insert "<div class=\"code\">\n")
    (insert "<pre>\n")
    (insert text)
    (insert "</pre>\n")
    (insert "</div>\n")))

(defun mkdoc (file)
  (interactive "fFile: ")
  (let* ((nfile (output-file file))
	 (buf (find-file-noselect nfile)))
    (save-excursion
      (set-buffer buf)
      (erase-buffer)
      (insert-file-contents file)
      (goto-char (point-min))
      (while (re-search-forward "^@@\\(.*\\)(\\(.*\\))\n" nil t)
	(let ((cmd (match-string 1))
	      (s (match-string 2)))
	  (replace-match "" nil nil)
	  (splice (intern cmd) s)))
      (save-buffer))
    (kill-buffer buf)))
