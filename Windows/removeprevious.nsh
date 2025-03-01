;Remove previous files
RMDir /r "$INSTDIR\bearer"
RMDir /r "$INSTDIR\generic"
RMDir /r "$INSTDIR\iconengines"
RMDir /r "$INSTDIR\imageformats"
RMDir /r "$INSTDIR\networkinformation"
RMDir /r "$INSTDIR\platforminputcontexts"
RMDir /r "$INSTDIR\platforms"
RMDir /r "$INSTDIR\position"
RMDir /r "$INSTDIR\printsupport"
RMDir /r "$INSTDIR\qml"
RMDir /r "$INSTDIR\qmltooling"
RMDir /r "$INSTDIR\resources"
RMDir /r "$INSTDIR\tls"
RMDir /r "$INSTDIR\translations"
RMDir /r "$INSTDIR\styles"

;Very old stuff --
RMDir /r "$INSTDIR\iWikilinks"
RMDir /r "$INSTDIR\win-bash"
RMDir /r "$INSTDIR\lang"
RMDir /r "$INSTDIR\templates"
Delete "$INSTDIR\InyokaStyles.zip"
;-----------------

RMDir /r "$INSTDIR\community"

Delete "$INSTDIR\dicts\de_DE.aff"
Delete "$INSTDIR\dicts\de_DE.dic"
RMDir "$INSTDIR\dicts"

Delete "$INSTDIR\Plugins\highlighter*.dll"
Delete "$INSTDIR\Plugins\hotkey*.dll"
Delete "$INSTDIR\Plugins\spellchecker-nuspell*.dll"
Delete "$INSTDIR\Plugins\knowledgebox*.dll"
Delete "$INSTDIR\Plugins\tabletemplate*.dll"
Delete "$INSTDIR\Plugins\uu_knowledgebox*.dll"
Delete "$INSTDIR\Plugins\uu_tabletemplate*.dll"
RMDir "$INSTDIR\Plugins"

Delete "$INSTDIR\COPYING"
Delete "$INSTDIR\*.txt"
Delete "$INSTDIR\*.md"
Delete "$INSTDIR\*.dll"
Delete "$INSTDIR\vcredist_x64.exe"
Delete "$INSTDIR\vc_redist.x64.exe"
Delete "$INSTDIR\QtWebEngineProcess.exe"
Delete "$INSTDIR\InyokaEdit.exe"
Delete "$INSTDIR\Uninstall.exe"
