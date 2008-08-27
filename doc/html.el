(defun fnref (beg end)
  (interactive "r")
  (let ((text (buffer-substring beg end)))
    (delete-region beg end)
    (insert "<a href=\"#fn:")
    (insert text)
    (insert "\">")
    (insert text)
    (insert "</a>")))


    

