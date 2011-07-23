 HEADERS       = CInyokaEdit.h \
                 CHighlighter.h \
                 CParser.h \
                 CDownloadArticle.h \
                 CTextEditor.h \
                 CInsertSyntaxElement.h \
    qtfindreplacedialog/findreplaceform.h \
    qtfindreplacedialog/findreplacedialog.h \
    qtfindreplacedialog/findreplace_global.h \
    qtfindreplacedialog/findform.h \
    qtfindreplacedialog/finddialog.h
 SOURCES       = main.cpp \
                 CInyokaEdit.cpp \
                 CHighlighter.cpp \
                 CParser.cpp \
                 CDownloadArticle.cpp \
                 CTextEditor.cpp \
                 CInsertSyntaxElement.cpp \
    qtfindreplacedialog/findreplaceform.cpp \
    qtfindreplacedialog/findreplacedialog.cpp \
    qtfindreplacedialog/findform.cpp \
    qtfindreplacedialog/finddialog.cpp
 RESOURCES     = InyokaEditResources.qrc
 QT           += webkit

FORMS += \
    qtfindreplacedialog/findreplaceform.ui \
    qtfindreplacedialog/findreplacedialog.ui
