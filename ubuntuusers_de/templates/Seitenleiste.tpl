## Macro=
||<tablestyle="float:right; clear:right; border:none; margin-top: 0; margin-left: 5px;" <@ for $block in $arguments join_with ' ' split_by '

' @><@ if $loop.first @><@ else @>
||<<@ endif @><@ for $row in $block split_by '
' @><@ if $loop.first @>cellstyle="background-color:#E2C889; font-size:1.1em; border:none;" :><@ else @>
||<cellstyle="background-color:#F9EAAF; <@ if $loop.last or $row matches_regex ".*\[\[BR\]\]" @>border-width: 0 0 10px 0; border-color: #FFFFFF;<@ else @>border:none;<@ endif @>"><@ endif @> <@ $row @>||<@ endfor @><@ endfor @>
