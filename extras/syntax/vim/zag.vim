" Vim syntax file
" Language:	Zag
" Maintainer:	Aran Roig <aranseraroig@gmail.com>

" quit when a syntax file was already loaded.
if exists("b:current_syntax")
  finish
endif

syn keyword zagGet get
syn keyword zagKeywords if els eif brk fn kin hoc lup 
syn keyword zagBooleans yep nop nil 
syn keyword zagTypes Int Str Any Nil Num Arr Map Bul
syn match zagString '\"[^\"]*\"'
syn match zagInlineComment '\/\/[^\n]*'
syn region zagBlockComment start="/\*" end="\*/" fold
syn region zagBlock start="{" end="}" fold transparent
" syn match zagTypes '\(:\|=>\)\zs[\n\r\t\f\v ]*[a-zA-Z0-9_]\+\(<.*>\)\?'

syn match zagNumber '\d\+' 
syn match zagNumber '[-+]\d\+'
syn match zagNumber '\d\+\.\d*'
syn match zagNumber '[-+]\d\+\.\d*' 
syn match zagLabel  '\'[^\n\r\t\f\v ]*'

let b:current_syntax = "zag"
hi def link zagGet            PreProc
hi def link zagKeywords		    Statement 
hi def link zagString		      String 
hi def link zagNumber		      Number 
hi def link zagBooleans       Boolean
hi def link zagLabel          Label
hi def link zagTypes          Type
hi def link zagInlineComment  Comment
hi def link zagBlockComment   Comment
