 HEADERS     = CInyokaEdit.h \
               CHighlighter.h \
               CParser.h \
               CTextEditor.h \
               CInsertSyntaxElement.h \
               qtfindreplacedialog/findreplaceform.h \
               qtfindreplacedialog/findreplacedialog.h \
               qtfindreplacedialog/findreplace_global.h \
               qtfindreplacedialog/findform.h \
               qtfindreplacedialog/finddialog.h \
               CProgressDialog.h
 SOURCES     = main.cpp \
               CInyokaEdit.cpp \
               CHighlighter.cpp \
               CParser.cpp \
               CTextEditor.cpp \
               CInsertSyntaxElement.cpp \
               qtfindreplacedialog/findreplaceform.cpp \
               qtfindreplacedialog/findreplacedialog.cpp \
               qtfindreplacedialog/findform.cpp \
               qtfindreplacedialog/finddialog.cpp \
               CProgressDialog.cpp
 RESOURCES   = inyokaeditresources.qrc
 QT         += webkit

 FORMS      += qtfindreplacedialog/findreplaceform.ui \
               qtfindreplacedialog/findreplacedialog.ui \
               CProgressDialog.ui
