## Macro=
<@ for $block in $arguments join_with ' ' split_by '

' @><@ if $loop.first @>||<-3 tablestyle="border:none; margin-top: 0; margin-right: 0"; cellstyle="border-width:0 10px 0 0; border-color: #FFFFFF; background-color:#E2C889; font-size:1.1em;" :> <@ $block @> ||<@ else @>
||<@ if $block matches_regex ".*\n.*" @><@ for $row in $block split_by '
' @><cellstyle="border-width:0 10px 10px 0; border-color: #FFFFFF; width:32%; background-color:#F9EAAF" :> <@ $row @>||<@ endfor @><@ else @><-3 cellstyle="border-width:0 10px 10px 0; border-color: #FFFFFF; background-color:#F9EAAF" :><@ $block @>||<@ endif @><@ endif @><@ endfor @>
