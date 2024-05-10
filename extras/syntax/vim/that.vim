" Vim syntax file
" Language:	that
" Maintainer:	Aran Roig <aranseraroig@gmail.com>

" quit when a syntax file was already loaded.
if exists("b:current_syntax")
  finish
endif

syn keyword thatGet get
syn keyword thatKeywords if els eif brk fn kin hoc lup ret
syn keyword thatBooleans yep nop nil 
syn keyword thatTypes Int Str Any Nil Num Arr Map Bul Chr
syn match thatString '\"[^\"]*\"'
syn match thatInlineComment '\/\/[^\n]*'
syn region thatBlockComment start="/\*" end="\*/" fold
syn region thatBlock start="{" end="}" fold transparent
" syn match thatTypes '\(:\|=>\)\zs[\n\r\t\f\v ]*[a-zA-Z0-9_]\+\(<.*>\)\?'

syn match thatNumber '\d\+' 
syn match thatNumber '[-+]\d\+'
syn match thatNumber '\d\+\.\d*'
syn match thatNumber '[-+]\d\+\.\d*' 
syn match thatLabel  '\'[^\n\r\t\f\v ]*'

let b:current_syntax = "that"
hi def link thatGet            PreProc
hi def link thatKeywords		    Statement 
hi def link thatString		      String 
hi def link thatNumber		      Number 
hi def link thatBooleans       Boolean
hi def link thatLabel          Label
hi def link thatTypes          Type
hi def link thatInlineComment  Comment
hi def link thatBlockComment   Comment
