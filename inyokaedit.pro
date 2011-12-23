 HEADERS      = CInyokaEdit.h \
                CHighlighter.h \
                CTextEditor.h \
                CInsertSyntaxElement.h \
                qtfindreplacedialog/findreplaceform.h \
                qtfindreplacedialog/findreplacedialog.h \
                qtfindreplacedialog/findreplace_global.h \
                qtfindreplacedialog/findform.h \
                qtfindreplacedialog/finddialog.h \
                CProgressDialog.h \
                CSettings.h \
                CInterwiki.h \
                CParser.h

 SOURCES      = main.cpp \
                CInyokaEdit.cpp \
                CHighlighter.cpp \
                CTextEditor.cpp \
                CInsertSyntaxElement.cpp \
                qtfindreplacedialog/findreplaceform.cpp \
                qtfindreplacedialog/findreplacedialog.cpp \
                qtfindreplacedialog/findform.cpp \
                qtfindreplacedialog/finddialog.cpp \
                CProgressDialog.cpp \
                CSettings.cpp \
                CInterwiki.cpp \
                CParser.cpp

 RESOURCES    = inyokaeditresources.qrc

 QT          += core \
                gui \
                webkit \
                xml

 FORMS       += CInyokaEdit.ui \
                qtfindreplacedialog/findreplaceform.ui \
                qtfindreplacedialog/findreplacedialog.ui \
                CProgressDialog.ui

 CODECFORSRC  = UTF-8
 CODECFORTR   = UTF-8

 TRANSLATIONS = lang/inyokaedit_de.ts

 # After changing defines clean all and run qmake again
 #DEFINES += NO_INSTALL
