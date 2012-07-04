;; functions for editing cqct documentation 

(defun mktag (beg end tag)
  (let ((pos nil))
    (save-excursion
      (set-mark end)
      (goto-char beg)
      (insert "<span class=\"" tag "\">")
      (goto-char (mark))
      (insert "</span>")
      (setq pos (point)))
    (goto-char pos)))

(defun mkval (beg end)
  (interactive "r")
  (mktag beg end "val"))

(defun mkarg (beg end)
  (interactive "r")
  (mktag beg end "arg"))

(defun mkid (beg end)
  (interactive "r")
  (mktag beg end "id"))

(defun mkref (beg end)
  (interactive "r")
  (let ((r (buffer-substring beg end)))
    (delete-region beg end)
    (insert (format "<span class=\"%s\"><a href=\"#fn:%s\">%s</a></span>"
		    r r r))))
