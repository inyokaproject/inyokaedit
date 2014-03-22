## Macro=
<@ for $block in $arguments join_with ' ' split_by '

' @><@ if $loop.first @>||<-6 tablestyle="width: 98%; border:none"; cellstyle="border-width:0 10px 10px 0; border-color: #FFFFFF; background-color:#E2C889; font-size:1.1em" :> <@ $block @>||<@ else @>
||<@ for $row in $block split_by '
' @><cellstyle="border-color: #FFFFFF;<@ if $loop.index is even @>width:26%; border-width:0 10px 10px 0; <@ else @>border-width:0 0 10px 0; <@ endif @>background-color:#F9EAAF;" > <@ $row @> ||<@ if $loop.last @><@ if $loop.index == 2 @><-4 cellstyle="border-color: #FFFFFF; border-width:0 10px 10px 0;"> ||<@ endif @><@ if $loop.index == 4 @><-2 cellstyle="border-color: #FFFFFF; border-width:0 10px 10px 0;"> ||<@ endif @><@ endif @><@ endfor @><@ endif @><@ endfor @>
