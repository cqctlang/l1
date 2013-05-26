" Vim syntax file
" Language:	CQCT
" modified from C syntax highlighter
"
" To use, put this file in $HOME/.vim/ and add this to .vimrc:
"
" au BufRead,BufNewFile *.cqct    set filetype=cqct
" au BufRead,BufNewFile *.cql     set filetype=cqct
" au BufRead,BufNewFile *.l1      set filetype=cqct
" au BufRead,BufNewFile *.names   set filetype=cqct
" au Syntax cqct source $HOME/.vim/cqct.vim

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" A bunch of useful C keywords
syn keyword cqctStatement   goto break return continue asm
syn keyword cqctLabel       case default
syn keyword cqctConditional if else switch
syn keyword cqctRepeat      while for do 
syn keyword cqctOperator	sizeof

syn match   cqctAtSymbol display "@[^\t \(\+\-%@]*" 
syn match   cqctAtDecl "@\(global\|local\)"
syn match   cqctAtDef "@\(define\|defloc\|defstx\|names\|record\|const\)"
syn match   cqctAtInclude "@\(include\|import_object\|import_globals\|with_imports\|with_exports\|import\|export\)"
syn match   cqctAtMatch "@\(match\)"
syn match   cqctAtFn "@\(typeof\|containerof\)"
syn match   cqctAtLambda "@\(lambda\)"

syn keyword cqctTodo    contained TODO FIXME XXX

" It's easy to accidentally add a space after a backslash that was intended
" for line continuation.  
syn match cqctBadContinuation   contained "\\\s\+$"

" cqctCommentGroup allows adding matches for special things in comments
syn cluster cqctCommentGroup    contains=cqctTodo

" String and Character constants
" Highlight special characters (those which have a backslash) differently
syn region cqctString start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=cqctSpecial,cqctFormat,@Spell

syn match cqctSpecial display contained "\\\(x\x\+\|\o\{1,3}\|.\|$\)"

syn match cqctFormat display "%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlL]\|ll\|hh\)\=\([acfgdiouxXepsbBty]\|\[\^\=.[^]]*\]\)" contained
syn match cqctFormat display "%%" contained


syn match   cqctCharacter   "L\='[^\\]'"
syn match   cqctCharacter   "L'[^']*'" contains=cqctSpecial

syn match   cqctSpecialError    "L\='\\[^'\"?\\abfnrtv]'"
syn match   cqctSpecialChar     "L\='\\['\"?\\abfnrtv]'"

syn match   cqctSpecialChar display "L\='\\\o\{1,3}'"
syn match   cqctSpecialChar display "'\\x\x\{1,2}'"
syn match   cqctSpecialChar display "L'\\x\x\+'"

"when wanted, highlight trailing white space
"if !exists("c_no_trail_space_error")
  "syn match    cqctSpaceError display excludenl "\s\+$"
"endif
"if !exists("c_no_tab_space_error")
  "syn match    cqctSpaceError display " \+\t"me=e-1
"endif

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match	cqctNumbers	display transparent "\<\d\|\.\d" contains=cqctNumber,cqctFloat,cqctOctalError,cqctOctal,cqctBinary,cqctBinaryError
" Same, but without octal error (for comments)
syn match	cqctNumbersCom	display contained transparent "\<\d\|\.\d" contains=cqctNumber,cqctFloat,cqctOctal
syn match	cqctNumber		display contained "\d\+\(u\=l\{0,2}\|ll\=u\)\>"
"hex number
syn match	cqctNumber		display contained "0x\x\+\(u\=l\{0,2}\|ll\=u\)\>"
" Flag the first zero of an octal number as something special
syn match	cqctOctal		display contained "0\o\+\(u\=l\{0,2}\|ll\=u\)\>" contains=cqctOctalZero
syn match	cqctOctalZero	display contained "\<0"
"cqct allows bitstrings prefixed by '0b', i.e. 0b010101011
syn match   cqctBinary      display contained "0b[01]\+\>"
syn match	cqctFloat		display contained "\d\+f"
"floating point number, with dot, optional exponent
syn match	cqctFloat		display contained "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="
"floating point number, starting with a dot, optional exponent
syn match	cqctFloat		display contained "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match	cqctFloat		display contained "\d\+e[-+]\=\d\+[fl]\=\>"

" flag an octal number with wrong digits
syn match	cqctOctalError	display contained "0\o*[89]\d*"
syn case match
syn match	cqctBinaryError	display contained "0[bB]\d*[23456789]\d*"

" A comment can contain cqctString, cqctCharacter and cqctNumber.
" But a "*/" inside a cqctString in a cqctComment DOES end the comment!  So we
" need to use a special type of cqctString: cqctCommentString, which also ends on
" "*/", and sees a "*" at the start of the line as comment again.
" Unfortunately this doesn't very well work for // type of comments :-(
syntax match	cqctCommentSkip	contained "^\s*\*\($\|\s\+\)"
syntax region cqctCommentString	contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=cqctSpecial,cqctCommentSkip
syntax region cqctComment2String	contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=cqctSpecial
syntax region  cqctCommentL	start="//" skip="\\$" end="$" keepend contains=@cqctCommentGroup,cqctComment2String,cqctCharacter,cqctNumbersCom,cqctSpaceError,@Spell
" Use "extend" here to have preprocessor lines not terminate halfway a
" comment.
syntax region cqctComment	matchgroup=cqctCommentStart start="/\*" end="\*/" contains=@cqctCommentGroup,cqctCommentStartError,cqctCommentString,cqctCharacter,cqctNumbersCom,cqctSpaceError,@Spell extend
syntax region cqctComment	matchgroup=cqctCommentStart start="#!" end="!#" contains=@cqctCommentGroup,cqctHBCommentStartError,cqctCommentString,cqctCharacter,cqctNumbersCom,cqctSpaceError,@Spell extend

" keep a // comment separately, it terminates a preproc. conditional
syntax match	cqctCommentError	display "\*/"
syntax match	cqctCommentStartError display "/\*"me=e-1 contained
syntax match	cqctCommentError	display "!#"
syntax match	cqctHBCommentStartError display "#!"me=e-1 contained

syn keyword	cqctType		int long short char void
syn keyword	cqctType		int8 int16 int32 int64
syn keyword	cqctType		uint8 uint16 uint32 uint64 uintptr
syn keyword	cqctType		signed unsigned float double
syn keyword	cqctType		_Bool _Complex

syn keyword	cqctStructure	struct union enum typedef

syn keyword cqctConstant nil

" Highlight User Labels
syn cluster	cqctMultiGroup	contains=cqctSpecial,cqctCommentSkip,cqctCommentString,cqctComment2String,@cqctCommentGroup,cqctCommentStartError,cqctUserCont,cqctUserLabel,cqctBitField,cqctOctalZero,cFormat,cqctNumber,cqctFloat,cqctOctal,cqctOctalError,cqctNumbersCom,cCppParen,cCppBracket,cCppString
syn region	cMulti		transparent start='?' skip='::' end=':' contains=ALLBUT,@cqctMultiGroup,@Spell
" Avoid matching foo::bar() in C++ by requiring that the next char is not ':'
syn cluster	cqctLabelGroup	contains=cqctUserLabel
syn match	cqctUserCont	display "^\s*\I\i*\s*:$" contains=@cqctLabelGroup
syn match	cqctUserCont	display ";\s*\I\i*\s*:$" contains=@cqctLabelGroup
syn match	cqctUserCont	display "^\s*\I\i*\s*:[^:]"me=e-1 contains=@cqctLabelGroup
syn match	cqctUserCont	display ";\s*\I\i*\s*:[^:]"me=e-1 contains=@cqctLabelGroup

syn match	cqctUserLabel	display "\I\i*" contained

" Avoid recognizing most bitfields as labels
syn match	cqctBitField	display "^\s*\I\i*\s*:\s*[1-9]"me=e-1 contains=cqctType
syn match	cqctBitField	display ";\s*\I\i*\s*:\s*[1-9]"me=e-1 contains=cqctType

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link cqctBadContinuation	Error
hi def link cqctStatement	    Statement
hi def link cqctLabel		    Label
hi def link cqctConditional	    Conditional
hi def link cqctRepeat		    Repeat
hi def link cqctTodo		    Todo

hi def link cqctAtDecl  	    Type
hi def link cqctAtDef    	    Macro
hi def link cqctAtInclude	    Macro
hi def link cqctAtMatch 	    Macro
hi def link cqctAtFn            Operator
hi def link cqctAtLambda        Special
hi def link cqctAtSymbol	    Constant
hi def link cqctString		    String
hi def link cqctFormat		    cqctSpecial

hi def link cqctSpecialAt       Conditional

hi def link cqctCommentL		cqctComment
hi def link cqctCommentStart	cqctComment
hi def link cqctUserLabel		Label
hi def link cqctCharacter		Character
hi def link cqctSpecialChar	    cqctSpecial
hi def link cqctNumber		    Number
hi def link cqctOctal		    Number
hi def link cqctBinary		    Number
hi def link cqctOctalZero		Number
hi def link cqctFloat		    Float
hi def link cqctOctalError		cqctError
hi def link cqctBinaryError		cqctError
hi def link cqctParenError		cqctError
hi def link cqctCommentError	cqctError
hi def link cqctCommentStartError	cqctError
hi def link cqctSpaceError		cqctError
hi def link cqctSpecialError	cqctError
hi def link cqctOperator		Operator
hi def link cqctStructure		Structure
hi def link cqctError		    Error
hi def link cqctType		    Type
hi def link cqctConstant		Constant
hi def link cqctCommentString	cqctString
hi def link cqctComment2String	cqctString
hi def link cqctCommentSkip	    cqctComment
hi def link cqctComment		    Comment
hi def link cqctSpecial		    SpecialChar

let b:current_syntax = "cqct"

" vim: ts=4
