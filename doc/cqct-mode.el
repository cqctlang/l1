;; derived from example in http://cc-mode.sourceforge.net/derived-mode-ex.el

(require 'cc-mode)

;; These are only required at compile time to get the sources for the
;; language constants.  (The cc-fonts require and the font-lock
;; related constants could additionally be put inside an
;; (eval-after-load "font-lock" ...) but then some trickery is
;; necessary to get them compiled.)
(eval-when-compile
  (require 'cc-langs)
  (require 'cc-fonts))

(eval-and-compile
  ;; Make our mode known to the language constant system.  Use C
  ;; mode as the fallback for the constants we don't change here.
  ;; This needs to be done also at compile time since the language
  ;; constants are evaluated then.
  (c-add-language 'cqct-mode 'c-mode))

(defcustom cqct-font-lock-extra-types nil
  "*List of extra types (aside from the type keywords) to recognize in C: mode.
Each list item should be a regexp matching a single identifier.")

(defconst cqct-font-lock-keywords-1 (c-lang-const c-matchers-1 cqct)
  "Minimal highlighting for CQCT mode.")

(defconst cqct-font-lock-keywords-2 (c-lang-const c-matchers-2 cqct)
  "Fast normal highlighting for CQCT mode.")

(defconst cqct-font-lock-keywords-3 (c-lang-const c-matchers-3 cqct)
  "Accurate normal highlighting for CQCT mode.")

(defvar cqct-font-lock-keywords cqct-font-lock-keywords-3
  "Default expressions to highlight in CQCT mode.")

(defvar cqct-mode-syntax-table nil
  "Syntax table used in cqct-mode buffers.")
(or cqct-mode-syntax-table
    (let ((tab (funcall (c-lang-const c-make-mode-syntax-table cqct))))
      (modify-syntax-entry ?' "'" tab)
      (setq cqct-mode-syntax-table tab)))

(defvar cqct-mode-abbrev-table nil
  "Abbreviation table used in cqct-mode buffers.")
(c-define-abbrev-table 'cqct-mode-abbrev-table '())

(defvar cqct-mode-map (let ((map (c-make-inherited-keymap)))
		      ;; Add bindings which are only useful for CQCT
		      map)
  "Keymap used in cqct-mode buffers.")

(easy-menu-define cqct-menu cqct-mode-map "CQCT: Mode Commands"
		  (cons "CQCT" (c-lang-const c-mode-menu cqct)))

;;;###autoload
(add-to-list 'auto-mode-alist '("\\.cqct\\'" . cqct-mode))

;;;###autoload
(defun cqct-mode ()
  "Major mode for editing cinquecento code.

The hook `c-mode-common-hook' is run with no args at mode
initialization, then `cqct-mode-hook'.

Key bindings:
\\{cqct-mode-map}"
  (interactive)
  (kill-all-local-variables)
  (c-initialize-cc-mode t)
  (set-syntax-table cqct-mode-syntax-table)
  (setq major-mode 'cqct-mode
	mode-name "cqct"
	local-abbrev-table cqct-mode-abbrev-table
	abbrev-mode t)
  (use-local-map c-mode-map)
  ;; `c-init-language-vars' is a macro that is expanded at compile
  ;; time to a large `setq' with all the language variables and their
  ;; customized values for our language.
  (c-init-language-vars cqct-mode)
  ;; `c-common-init' initializes most of the components of a CC Mode
  ;; buffer, including setup of the mode menu, font-lock, etc.
  ;; There's also a lower level routine `c-basic-common-init' that
  ;; only makes the necessary initialization to get the syntactic
  ;; analysis and similar things working.
  (c-common-init 'cqct-mode)
  (easy-menu-add cqct-menu)
  (run-hooks 'c-mode-common-hook)
  (run-hooks 'cqct-mode-hook)
  (c-update-modeline))

(provide 'cqct-mode)
