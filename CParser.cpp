/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CParser.cpp
* Purpose:    Parse plain text with inyoka syntax to html code
***************************************************************************/

#include "CParser.h"

// Constructor
CParser::CParser(QTextDocument *rawDocument, const QString &sUrlToWiki, const QDir TmpFileOutputDir, const QList<QStringList> sListIWiki, const QList<QStringList> sListIWikiUrl)
    : rawText(rawDocument), sWikiUrl(sUrlToWiki), TmpFileDir(TmpFileOutputDir)
{
    // Copy interwiki links to lists
    for (int i = 0; i < sListIWiki.size(); i++) {
        for (int j = 0; j < sListIWiki[i].size(); j++) {
            sListInterwikiKey << sListIWiki[i][j];
            sListInterwikiLink << sListIWikiUrl[i][j];
        }
    }

    // Initialize possible text formats
    sListFormatStart << "'''";          // BOLD
    sListFormatEnd << "'''";
    sListFormatHtmlStart << "<strong>";
    sListFormatHtmlEnd << "</strong>";
    sListFormatStart << "''";           // ITALIC
    sListFormatEnd << "''";
    sListFormatHtmlStart << "<em>";
    sListFormatHtmlEnd << "</em>";
    sListFormatStart << "`";            // MONOTYPE
    sListFormatEnd << "`";
    sListFormatHtmlStart << "<code>";
    sListFormatHtmlEnd << "</code>";
    sListFormatStart << "__";           // UNDERLINE
    sListFormatEnd << "__";
    sListFormatHtmlStart << "<span class=\"underline\">";
    sListFormatHtmlEnd << "</span>";
    sListFormatStart << "--(";          // STRIKE OUT
    sListFormatEnd << ")--";
    sListFormatHtmlStart << "<del>";
    sListFormatHtmlEnd << "</del>";
    sListFormatStart << "^^(";          // RISED TEXT
    sListFormatEnd << ")^^";
    sListFormatHtmlStart << "<sup>";
    sListFormatHtmlEnd << "</sup>";
    sListFormatStart << ",,(";          // LOWERED TEXT
    sListFormatEnd << "),,";
    sListFormatHtmlStart << "<sub>";
    sListFormatHtmlEnd << "</sub>";
    sListFormatStart << "~+(";          // BIGGER
    sListFormatEnd << ")+~";
    sListFormatHtmlStart << "<big>";
    sListFormatHtmlEnd << "</big>";
    sListFormatStart << "~-(";          // SMALLER
    sListFormatEnd << ")-~";
    sListFormatHtmlStart << "<small>";
    sListFormatHtmlEnd << "</small>";
    /*
    sListFormatStart << "[size=+\d+]";  // DEFINED SIZE
    sListFormatEnd << "[/size]";
    sListFormatHtmlStart << "<span style=\"font-size: %1\">";
    sListFormatHtmlEnd << "</span>";
    */
    /*
    sListFormatStart << "[color=#+[0-9A-F]{5,5}]";  // DEFINED COLOR (hex)
    sListFormatEnd << "[/color]|";
    sListFormatHtmlStart << "<span style=\"color: %1\">";
    sListFormatHtmlEnd << "</span>";
    */
    /*
    sListFormatStart << "[color=[a-z]{3,8}]";               // DEFINED COLOR (word)
    sListFormatEnd << "[/color]";
    sListFormatHtmlStart << "<span-]+\[/color\] style=\"color: %1\">";
    sListFormatHtmlEnd << "</span>";
    */

    // ----------------------------------------------------------------------------------------------------

    // Initialize possible flags
    sListFlags << "cz" << "da" << "de" << "en" << "eo" << "es" << "fi" << "fr" << "hr" << "hu" << "it" << "ja" <<
            "ko" << "lv" << "nl" << "nw" << "pl" << "pt" << "ro" << "rs" << "ru" << "sk" << "sv" << "tr" << "zh" <<
            "at" << "ch" << "gb" << "us" << "cl" << "dl";

    bShowedMsgBoxAlready = false;
}

// Destructor
CParser::~CParser()
{
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

bool CParser::genOutput(const QString sActFile)
{
    // File for temporary html output
    QFile tmphtmlfile(TmpFileDir.absolutePath() + "/tmpinyoka.html");

    // No write permission
    if (!tmphtmlfile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(0, "Warning", tr("Could not create temporary HTML file!"), "Msg: Could not create preview html file");
        return false;
    }

    // Stream for output in file
    QTextStream tmpoutputstream(&tmphtmlfile);
    tmpoutputstream.setCodec("UTF-8");
    tmpoutputstream.setAutoDetectUnicode(true);

    // File name
    QFileInfo fi(sActFile);
    QString sFilename;
    if (sActFile == "")
        sFilename = tr("Untitled", "No file name set");
    else
        sFilename = fi.fileName();

    // Define html header
    // IMPORTANT: PAY ATTENTION TO ORDER OF CSS-FILES!
    QString sHtmlHead("<?xml version=\"1.0\"  encoding=\"utf-8\"?>\n"
                      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
                      "\t\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                      "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
                      "<head>\n"
                      "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
                      "<title>InyokaEdit - " + sFilename + "</title>\n"
                      "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/main-sprite.css\" />\n"
                      "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/markup2.css\" />\n"
                      "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/wiki.css\" />\n"
                      "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/markup.css\" />\n"
                      "</head>\n"
                      "<body>\n"
                      "<div class=\"wrap\">\n"
                      "<div class=\"wrap2\">\n"
                      "<div class=\"wrap3\">\n"
                      "<div class=\"wrap4\">\n"
                      "<div class=\"wrap5\">\n"
                      "<div class=\"body\">\n"
                      "<div class=\"page_content\">\n"
                      "<div class=\"content content_tabbar\">\n"
                      "<h1 class=\"pagetitle\">" + sFilename + "</h1>\n"
                      "<div id=\"page\">\n");

    // Need a copy otherwise text in editor will be changed
    copyOfrawText = rawText->clone();

    // Replace all links
    replaceLinks(copyOfrawText);

    // Replace text format
    replaceTextformat(copyOfrawText);

    // Replace flags
    replaceFlags(copyOfrawText);

    // Replace keys
    replaceKeys(copyOfrawText);

    // Replace images
    replaceImages(copyOfrawText);

    // Replace anchor
    replaceAnchor(copyOfrawText);

    // Get first text block
    QTextBlock it = copyOfrawText->firstBlock();
    QString sHtmlBody, sWikitags;
    QString sSample;

    // Go through each text block and call depending parse function
    for(; it.isValid() && !(copyOfrawText->lastBlock() < it); it = it.next()) {
        // Macro samples
        if (it.text().trimmed().startsWith(trUtf8("[[Vorlage("), Qt::CaseSensitive) &&
            !(it.text().trimmed().startsWith(trUtf8("[[Vorlage(Tasten"), Qt::CaseSensitive)) &&
            !(it.text().trimmed().startsWith(trUtf8("[[Vorlage(Bildersammlung"), Qt::CaseSensitive))){
            sHtmlBody += parseMacro(it);
        }
        // Table of contents
        else if (it.text().trimmed().startsWith(trUtf8("[[Inhaltsverzeichnis"), Qt::CaseSensitive)){
            sHtmlBody += parseTableOfContents(it);
        }
        // Article tags
        else if (it.text().trimmed().startsWith("#tag:") || it.text().trimmed().startsWith("# tag:")){
            sWikitags = generateTags(it);
        }
        // Headline
        else if (it.text().trimmed().startsWith("=")){
            sHtmlBody += parseHeadline(it);
        }
        // Text sample
        else if (it.text().trimmed().startsWith(trUtf8("{{{#!vorlage"), Qt::CaseSensitive)){
            sSample = it.text();
            it = it.next();
            for (; it.isValid() && !(copyOfrawText->lastBlock() < it) && it.text().trimmed() != "}}}"; it = it.next()){
                sSample += "§" + it.text();
            }
            sHtmlBody += parseTextSample(sSample);
            //it = it.next();
        }
        // Codeblock
        else if (it.text().trimmed() == trUtf8("{{{") || it.text().trimmed().startsWith(trUtf8("{{{#!code"), Qt::CaseSensitive)){
            sSample = it.text();
            it = it.next();
            for (; it.isValid() && !(copyOfrawText->lastBlock() < it) && it.text().trimmed() != "}}}"; it = it.next()){
                sSample += "§" + it.text();
            }
            sHtmlBody += parseCodeBlock(sSample);
        }
        // Image collection
        else if (it.text().trimmed().startsWith(trUtf8("[[Vorlage(Bildersammlung"), Qt::CaseSensitive)){
            sSample = it.text();
            it = it.next();
            for (; it.isValid() && !(copyOfrawText->lastBlock() < it) && it.text().trimmed() != ")]]"; it = it.next()){
                sSample += "§" + it.text();
            }
            sHtmlBody += parseImageCollection(sSample);
        }
        // List
        else if (it.text().trimmed().startsWith("* ") || it.text().trimmed().startsWith("1. ")){
            sSample = it.text();
            it = it.next();
            QTextBlock tmpBlock = it;  // Next to last block
            for (; it.isValid() && !(copyOfrawText->lastBlock() < it); it = it.next()){

                if (it.text().trimmed().startsWith("* ") || it.text().trimmed().startsWith("1. ")){
                    sSample += "§" + it.text();
                    tmpBlock = it;
                }
                else {
                    it = tmpBlock;
                    break;
                }

                if (it == copyOfrawText->lastBlock())
                    break;
            }
            sHtmlBody += parseList(sSample);
        }

        // Everything else
        else{
            // Filter comments (##)
            if (!(it.text().trimmed().startsWith("##")))
                sHtmlBody += "<p>" + it.text() + "</p>";
        }
    }

    // Define end of html body
    QString sHtmlBodyEnd = "</div>\n"
                           "<p class=\"meta\">\n";
    sHtmlBodyEnd += trUtf8("Diese Revision wurde am %1 um %2 Uhr erstellt.").arg(QDate::currentDate().toString("dd.MM.yyyy")).arg(QTime::currentTime().toString("hh:mm"));
    sHtmlBodyEnd += "<br />\n";
    sHtmlBodyEnd += trUtf8("Dieser Seite wurden die folgenden Begriffe zugeordnet: %1").arg(sWikitags);
    sHtmlBodyEnd += "\n</p>\n"
                    "</div>\n"
                    "</div>\n"
                    "<div style=\"clear: both;\">&nbsp;</div>\n"
                    "</div>\n"
                    "</div>\n"
                    "</div>\n"
                    "</div>\n"
                    "</div>\n"
                    "</div>\n"
                    "</body>\n"
                    "</html>\n";

    // Write html code to output file
    tmpoutputstream << sHtmlHead << sHtmlBody << sHtmlBodyEnd;

    tmphtmlfile.close();

    // Call showPreview function from CInyokaEdit
    emit callShowPreview(QFileInfo(tmphtmlfile).filePath());

    return true;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// REPLACE TEXT FORMATS
void CParser::replaceTextformat(QTextDocument *myRawDoc)
{
    QRegExp patternTextformat;
    const QString variableText("[\\w\\s-_~:>\"\\(\\)/\\.\\+\\&\\!\\\"\\?/\\%\\*\\>]+"); // Escaped RegExp
    QString sMyDoc = myRawDoc->toPlainText();
    int iLength;
    QString sFormatedText, sTmpRegExp; //, sTmpText;
    int myindex;

    for(int i = 0; i < sListFormatStart.size(); i++){
        sTmpRegExp = QRegExp::escape(sListFormatStart[i]) + variableText + QRegExp::escape(sListFormatEnd[i]);

        patternTextformat.setPattern(sTmpRegExp);
        myindex = patternTextformat.indexIn(sMyDoc);
        while (myindex >= 0) {
            iLength = patternTextformat.matchedLength();
            sFormatedText = patternTextformat.cap();

            /*
            // Hex color
            if (sFormatedText.startsWith("[color=#")){

            }
            // Color as word
            else if (sFormatedText.startsWith("[color=")){
                sFormatedText.remove(sListFormatEnd[i]);
                sFormatedText.remove("[color=");
                int iBracket = sFormatedText.indexOf("]");
                sTmpText = sFormatedText;
                sTmpText.remove(iBracket, sTmpText.length()); // Remove everything besides color
                sFormatedText.remove(0, iBracket);
                sMyDoc.replace(myindex, iLength, QString(sListFormatHtmlStart[i]).arg(sTmpText) + sFormatedText + sListFormatHtmlEnd[i]);
            }
            else {
            */
                // Remove syntax element
                sFormatedText.remove(sListFormatStart[i]);
                sFormatedText.remove(sListFormatEnd[i]);
                sMyDoc.replace(myindex, iLength, sListFormatHtmlStart[i] + sFormatedText + sListFormatHtmlEnd[i]);
            //}

            // Go on with RegExp-Search
            myindex = patternTextformat.indexIn(sMyDoc, myindex + iLength);
        }
    }

    // Replace myRawDoc with document with formated text
    myRawDoc->setPlainText(sMyDoc);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Replace FLAGS
void CParser::replaceFlags(QTextDocument *myRawDoc){

    QRegExp findFlags("\\{[a-z\\w][a-z]+\\}");
    QString sMyDoc = myRawDoc->toPlainText();
    int iLength;
    QString sTmpFlag;

    int myindex = findFlags.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findFlags.matchedLength();
        sTmpFlag = findFlags.cap();
        sTmpFlag.remove("{");
        sTmpFlag.remove("}");

        if (sTmpFlag == trUtf8("Übersicht")){
            sMyDoc.replace(myindex, iLength, "<img src=\"img/flags/overview.png\" alt=\"&#123;" + trUtf8("Übersicht") + "&#125;\" />");
        }
        else if (sTmpFlag.length() == 2){
            if (sListFlags.contains(sTmpFlag)){
                sMyDoc.replace(myindex, iLength, "<img src=\"img/flags/" + sTmpFlag + ".png\" alt=\"&#123;" + sTmpFlag + "&#125;\" />");
            }
        }

        // Go on with RegExp-Search
        myindex = findFlags.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace myRawDoc with document with HTML links
    myRawDoc->setPlainText(sMyDoc);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Replace KEYS
void CParser::replaceKeys(QTextDocument *myRawDoc){
    QRegExp findKeys("\\[\\[Vorlage\\(Tasten,[\\w\\s\\?\\-\\=\\'\\,\\.\\`\\\"\\^\\<\\[\\]\\#\\+]+\\)\\]\\]");
    QString sMyDoc = myRawDoc->toPlainText();
    int iLength;
    QString sTmpKey;
    QStringList sListTmpKeys;

    int myindex = findKeys.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findKeys.matchedLength();
        sTmpKey = findKeys.cap();
        sTmpKey.remove(trUtf8("[[Vorlage(Tasten,"));
        sTmpKey.remove(")]]");

        sListTmpKeys.clear();
        sListTmpKeys = sTmpKey.split("+");
        sTmpKey.clear();

        for (int i = 0; i < sListTmpKeys.size(); i++){
            // Remove possible spaces before and after string
            sListTmpKeys[i] = sListTmpKeys[i].trimmed();
            sListTmpKeys[i][0] = sListTmpKeys[i][0].toLower();

            if (sListTmpKeys[i] == "backspace" || sListTmpKeys[i] == trUtf8("löschen") || sListTmpKeys[i] == trUtf8("rückschritt")){
                sTmpKey += "<span class=\"key\">&#9003;</span>";
            }
            else if (sListTmpKeys[i] == "ctrl"){
                sTmpKey += "<span class=\"key\">Strg</span>";
            }
            else if (sListTmpKeys[i] == "del" || sListTmpKeys[i] == "delete" || sListTmpKeys[i] == "entfernen"){
                sTmpKey += "<span class=\"key\">Entf</span>";
            }
            else if (sListTmpKeys[i] == "return" || sListTmpKeys[i] == "enter" || sListTmpKeys[i] == "eingabe"){
                sTmpKey += "<span class=\"key\">&#9166;</span>";
            }
            else if (sListTmpKeys[i] == "escape"){
                sTmpKey += "<span class=\"key\">Esc</span>";
            }
            else if (sListTmpKeys[i] == "eckig_auf"){
                sTmpKey += "<span class=\"key\">[</span>";
            }
            else if (sListTmpKeys[i] == "eckig_zu"){
                sTmpKey += "<span class=\"key\">]</span>";
            }
            else if (sListTmpKeys[i] == "bild auf" || sListTmpKeys[i] == "bild-auf" || sListTmpKeys[i] == "bild-rauf"){
                sTmpKey += "<span class=\"key\">Bild &uarr;</span>";
            }
            else if (sListTmpKeys[i] == "bild ab" || sListTmpKeys[i] == "bild-ab" || sListTmpKeys[i] == "bild-runter"){
                sTmpKey += "<span class=\"key\">Bild &darr;</span>";
            }
            else if (sListTmpKeys[i] == "print" || sListTmpKeys[i] == "prtsc" || sListTmpKeys[i] == "sysrq"){
                sTmpKey += "<span class=\"key\">Druck</span>";
            }
            else if (sListTmpKeys[i] == "mac" || sListTmpKeys[i] == "appel" || sListTmpKeys[i] == "apfel" || sListTmpKeys[i] == "cmd"){
                sTmpKey += "<span class=\"key\">&#8984;</span>";
            }
            else if (sListTmpKeys[i] == "caps" || sListTmpKeys[i] == "feststell" || sListTmpKeys[i] == "feststelltaste" || sListTmpKeys[i] == trUtf8("groß")){
                sTmpKey += "<span class=\"key\">&dArr;</span>";
            }
            else if (sListTmpKeys[i] == "shift" || sListTmpKeys[i] == "umschalt" || sListTmpKeys[i] == "umsch"){
                sTmpKey += "<span class=\"key\">&uArr;</span>";
            }
            else if (sListTmpKeys[i] == "at"){
                sTmpKey += "<span class=\"key\">&#64;</span>";
            }
            else if (sListTmpKeys[i] == "cherry" || sListTmpKeys[i] == "keyman"){
                sTmpKey += "<span class=\"key\">Keym&#64;n</span>";
            }
            else if (sListTmpKeys[i] == "space" || sListTmpKeys[i] == "leer" || sListTmpKeys[i] == "leertaste" || sListTmpKeys[i] == "leerzeichen" || sListTmpKeys[i] == "leerschritt"){
                sTmpKey += "<span class=\"key\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>";
            }
            else if (sListTmpKeys[i] == "tab" || sListTmpKeys[i] == "tabulator"){
                sTmpKey += "<span class=\"key\">Tab &#8644;</span>";
            }
            else if (sListTmpKeys[i] == "win"){
                sTmpKey += "<span class=\"key\">Windows</span>";
            }
            else if (sListTmpKeys[i] == "hoch" || sListTmpKeys[i] == "auf" || sListTmpKeys[i] == "up" || sListTmpKeys[i] == "rauf" || sListTmpKeys[i] == trUtf8("pfeil-hoch")){
                sTmpKey += "<span class=\"key\">&uarr;</span>";
            }
            else if (sListTmpKeys[i] == "runter" || sListTmpKeys[i] == "ab" || sListTmpKeys[i] == "down" || sListTmpKeys[i] == "pfeil-runter" || sListTmpKeys[i] == trUtf8("pfeil-ab")){
                sTmpKey += "<span class=\"key\">&darr;</span>";
            }
            else if (sListTmpKeys[i] == "links" || sListTmpKeys[i] == "left" || sListTmpKeys[i] == trUtf8("pfeil-links")){
                sTmpKey += "<span class=\"key\">&larr;</span>";
            }
            else if (sListTmpKeys[i] == "rechts" || sListTmpKeys[i] == "right" || sListTmpKeys[i] == trUtf8("pfeil-rechts")){
                sTmpKey += "<span class=\"key\">&rarr;</span>";
            }
            else if (sListTmpKeys[i] == "\",\"" || sListTmpKeys[i] == "\',\'"){
                sTmpKey += "<span class=\"key\">,</span>";
            }
            else if (sListTmpKeys[i] == "minus"){
                sTmpKey += "<span class=\"key\">-</span>";
            }
            else if (sListTmpKeys[i] == "plus"){
                sTmpKey += "<span class=\"key\">+</span>";
            }
            else if (sListTmpKeys[i] == "\"`\""){
                sTmpKey += "<span class=\"key\">`</span>";
            }
            //else if (sListTmpKeys[i] == "^"){
            //    sTmpKey += "<span class=\"key\">^</span>";
            //}
            //else if (sListTmpKeys[i] == "<"){
            //    sTmpKey += "<span class=\"key\"><</span>";
            //}
            else if (sListTmpKeys[i] == "sz"){
                sTmpKey += QString::fromUtf8("<span class=\"key\">&szlig;</span>");
            }
            else if (sListTmpKeys[i] == "gleich"){
                sTmpKey += QString::fromUtf8("<span class=\"key\">=</span>");
            }
            else if (sListTmpKeys[i] == "num" || sListTmpKeys[i] == trUtf8("num-taste") || sListTmpKeys[i] == trUtf8("num-Taste") || sListTmpKeys[i] == trUtf8("num-lock-taste") || sListTmpKeys[i] == trUtf8("num-Lock-Taste")){
                sTmpKey += QString::fromUtf8("<span class=\"key\">num &dArr;</span>");
            }
            else if (sListTmpKeys[i] == "fragezeichen"){
                sTmpKey += "<span class=\"key\">?</span>";
            }
            else if (sListTmpKeys[i] == "break"){
                sTmpKey += "<span class=\"key\">Pause</span>";
            }
            else if (sListTmpKeys[i] == "rollen" || sListTmpKeys[i] == "bildlauf"){
                sTmpKey += QString::fromUtf8("<span class=\"key\">&dArr; Rollen</span>");
            }
            else if (sListTmpKeys[i] == "slash"){
                sTmpKey += "<span class=\"key\">/</span>";
            }
            else if (sListTmpKeys[i] == "any"){
                sTmpKey += "<span class=\"key\">ANY KEY</span>";
            }
            else if (sListTmpKeys[i] == "panic"){
                sTmpKey += "<span class=\"key\">PANIC</span>";
            }
            else if (sListTmpKeys[i] == "koelsch"){
                sTmpKey += QString::fromUtf8("<span class=\"key\">K&ouml;lsch</span>");
            }
            else if (sListTmpKeys[i] == "lmt" || sListTmpKeys[i] == "lmb"){
                sTmpKey += "<img src=\"img/wiki/mouse_left.png\" alt=\"linke Maustaste\" class=\"image-default\" />";
            }
            else if (sListTmpKeys[i] == "rmt" || sListTmpKeys[i] == "rmb"){
                sTmpKey += "<img src=\"img/wiki/mouse_right.png\" alt=\"rechte Maustaste\" class=\"image-default\" />";
            }
            else if (sListTmpKeys[i] == "mmt" || sListTmpKeys[i] == "mmb"){
                sTmpKey += "<img src=\"img/wiki/mouse_midd.png\" alt=\"mittlere Maustaste\" class=\"image-default\" />";
            }

            // Everything else: First character to Upper (first characters had been changed to lower at beginning of function)
            else{
                sListTmpKeys[i][0] = sListTmpKeys[i][0].toUpper();
                sTmpKey += "<span class=\"key\">" + sListTmpKeys[i] + "</span>";
            }

            /*
            // Begins with lower case letters (a-z or ä, ö, ü)
            else if ((sListTmpKeys[i][0] > 96 && sListTmpKeys[i][0] < 122) ||
                     (sListTmpKeys[i][0] == 228 || sListTmpKeys[i][0] == 246 || sListTmpKeys[i][0] == 252)){
                sListTmpKeys[i][0] = sListTmpKeys[i][0].toUpper();
                sTmpKey += "<span class=\"key\">" + sListTmpKeys[i] + "</span>";
            }
            // Everything else will be given out dircetly
            else{
                sTmpKey += "<span class=\"key\">" + sListTmpKeys[i] + "</span>";
            }
            */

            // "+" between keys
            if (i != sListTmpKeys.size()-1)
                sTmpKey += " + ";
        }
        // Replace text with Html code
        sMyDoc.replace(myindex, iLength, sTmpKey);

        // Go on with RegExp-Search
        myindex = findKeys.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace myRawDoc with document with HTML links
    myRawDoc->setPlainText(sMyDoc);

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// REPLACE ALL LINKS
void CParser::replaceLinks(QTextDocument *myRawDoc){

    QRegExp findLink("\\[{1,1}[\\w\\s-_:\\(\\)/\\.#&]+\\]{1,1}");
    QString sMyDoc = myRawDoc->toPlainText();
    int iLength;
    QString sTmpLink;
    QStringList sListLink;

    int myindex = findLink.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findLink.matchedLength();
        sTmpLink = findLink.cap();
        sTmpLink.remove("[");
        sTmpLink.remove("]");

        // Interwiki links
        for (int i = 0; i < sListInterwikiKey.size(); i++){
            if (sTmpLink.startsWith(sListInterwikiKey[i] + ":") && sTmpLink.count(':') == 2 && !sTmpLink.startsWith("user:")){
                sTmpLink.remove(sListInterwikiKey[i] + ":");
                sListLink.clear();
                sListLink = sTmpLink.split(":");

                if (sListLink[1] != ""){
                    sMyDoc.replace(myindex, iLength, "<a href=\"" + sListInterwikiLink[i] + "" + sListLink[0] + "\" class=\"interwiki interwiki-" + sListInterwikiKey[i] + "\">" + sListLink[1] + "</a>");
                }
                else{
                    sMyDoc.replace(myindex, iLength, "<a href=\"" + sListInterwikiLink[i] + sListLink[0] + "\" class=\"interwiki interwiki-" + sListInterwikiKey[i] + "\">" + sListLink[0] + "</a>");
                }
                break;
            }
        }

        // Inyoka wiki link
        if (sTmpLink[0] == ':' && sTmpLink.count(':') == 2){
            sTmpLink.remove(0, 1);
            sListLink.clear();
            sListLink = sTmpLink.split(":");

            if (sListLink[1] != ""){
                sMyDoc.replace(myindex, iLength, "<a href=\"" + sWikiUrl + "/" + sListLink[0] + "\" class=\"internal\">" + sListLink[1] + "</a>");
            }
            else {
                sMyDoc.replace(myindex, iLength, "<a href=\"" + sWikiUrl + "/" + sListLink[0] + "\" class=\"internal\">" + sListLink[0] + "</a>");
            }
        }

        // Inyoka user
        else if (sTmpLink.startsWith("user:") && sTmpLink.count(':') == 2){
            sTmpLink.remove("user:");
            sListLink.clear();
            sListLink = sTmpLink.split(":");
            QString sTmpUrl = sWikiUrl;
            sTmpUrl.remove("wiki.");

            if (sListLink[1] != ""){
                sMyDoc.replace(myindex, iLength, "<a href=\"" + sTmpUrl + "/user/" + sListLink[0] + "\" class=\"crosslink user\">" + sListLink[1] + "</a>");
            }
            else {
                sMyDoc.replace(myindex, iLength, "<a href=\"" + sTmpUrl + "/user/" + sListLink[0] + "\" class=\"crosslink user\">" + sListLink[0] + "</a>");
            }
        }

        // Anchor link
        else if (sTmpLink.startsWith("#")){
            sTmpLink.remove("#");
            int firstSpace = sTmpLink.indexOf(" ");
            QString sAnchorName, sAnchorText, sOutput;

            sAnchorName = sTmpLink;
            sAnchorName.remove(firstSpace, sAnchorName.length());
            sAnchorText = sTmpLink.remove(0, firstSpace);

            sOutput = "<a href=\"#" + sAnchorName + "\" class=\"crosslink\">" + sAnchorText.trimmed() + "</a>";
            sMyDoc.replace(myindex, iLength, sOutput);
        }

        // External links
        else if (sTmpLink.startsWith("http://") || sTmpLink.startsWith("https://")){
            int iSpace = sTmpLink.indexOf(" ", 0);
            QString sHref = sTmpLink;
            sHref.remove(iSpace, sHref.length()-iSpace);
            if (iSpace != -1){
                sMyDoc.replace(myindex, iLength, "<a href=\"" + sHref + "\" rel=\"nofollow\" class=\"external\">" + sTmpLink.remove(0, iSpace) + "</a>");
            }
            else{
                sMyDoc.replace(myindex, iLength, "<a href=\"" + sHref + "\" rel=\"nofollow\" class=\"external\">" + sHref + "</a>");
            }
        }

        // Link to knowledge box (is a number)
        else if (sTmpLink.toUShort() != 0){
            sMyDoc.replace(myindex, iLength, "<sup><a href=\"#source-" + sTmpLink + "\">&#091;" + sTmpLink + "&#093;</a></sup>");
        }

        // Go on with RegExp-Search
        myindex = findLink.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace myRawDoc with document with HTML links
    myRawDoc->setPlainText(sMyDoc);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// MACROS [[Vorlage(...)]]
QString CParser::parseMacro(QTextBlock actParagraph){
    //QString CParser::convertBlock(QString actParagraph){

    QString sParagraph = actParagraph.text();
    //QString sParagraph = actParagraph;

    // Remove brackets
    sParagraph.remove(trUtf8("[[Vorlage("), Qt::CaseSensitive);
    sParagraph.remove(")]]");

    // Separate elementes from macro (between ,)
    QStringList sListElements = sParagraph.split(",");

    QString sOutput("<strong>ERROR: Found unknown item [[Vorlage(" + sListElements[0] + "</strong>\n");

    // Remove every quote sign and spaces before / behind strings (trimmed)
    for (int i = 0; i < sListElements.size(); i++){
        sListElements[i].remove("\"");
        sListElements[i] = sListElements[i].trimmed();
    }

    // Under construction (Baustelle)
    if (sListElements[0] == trUtf8("Baustelle") || sListElements[0] == trUtf8("InArbeit")){
        sOutput = "<div class=\"box workinprogress\">\n"
                  "<h3 class=\"box workinprogress\">";
        sOutput += trUtf8("Artikel in Arbeit");
        sOutput += "</h3>\n"
                   "<div class=\"contents\">\n";

        // Replace possible spaces
        for (int i = 1; i < sListElements.size(); i++){
            sListElements[i].replace(" ", "_");
        }

        // One editor
        QString sTmpUrl = sWikiUrl;
        sTmpUrl.remove("wiki.");
        if (sListElements.size() == 3){
            QString sLinkUser1 = "<a href=\"" + sTmpUrl + "/user/" + sListElements[2] + "/ \" class=\"crosslink user\">" + sListElements[2] + "</a>";
            sOutput += "<p>" + trUtf8("Dieser Artikel wird momentan überarbeitet von %1 erstellt.").arg(sLinkUser1);
        }
        // Two editors
        else if (sListElements.size() >= 4){
            QString sLinkUser2 = "<a href=\"" + sTmpUrl + "/user/" + sListElements[2] + "/ \" class=\"crosslink user\">" + sListElements[2] + "</a>";
            QString sLinkUser3 = "<a href=\"" + sTmpUrl + "/user/" + sListElements[3] + "/ \" class=\"crosslink user\">" + sListElements[3] + "</a>";
            sOutput += "<p>" + trUtf8("Dieser Artikel wird momentan überarbeitet von %1 und %2 erstellt.").arg(sLinkUser2).arg(sLinkUser3);
        }

        sOutput += " " + trUtf8("Als Fertigstellungsdatum wurde der %1 angegeben.").arg(sListElements[1]);
        sOutput += "</p>\n"
                   "<hr />\n";
        sOutput += "<p><strong>" + trUtf8("Achtung:") + " </strong>\n";
        sOutput += trUtf8("Insbesondere heißt das, dass dieser Artikel noch nicht fertig ist und dass wichtige Teile fehlen oder sogar falsch sein können. Bitte diesen Artikel nicht als Anleitung für Problemlösungen benutzen.");
        sOutput += "</p>\n"
                   "</div>\n"
                   "</div>\n";

    }
    // -----------------------------------------------------------------------------------------------

    // TESTED (Getestet)
    else if(sListElements[0] == trUtf8("Getestet")){
        sOutput = "<div class=\"box tested_for\">\n"
                  "<h3 class=\"box tested_for\">";
        sOutput += trUtf8("Dieser Artikel wurde für die folgenden Ubuntu-Versionen getestet:");
        sOutput += "</h3>\n"
                   "<div class=\"contents\">\n";

        if (sListElements.size() >= 2){
            // Article untested
            if (sListElements[1] == "" || sListElements[1] == " "){
                sOutput += "<p>";
                sOutput += trUtf8("Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte diesen Artikel testen und das getestet-Tag entsprechend anpassen.");
                sOutput += "</p>"
                           "</div>\n"
                           "</div>\n";
            }
            // Tested "general"
            else if (sListElements[1] == trUtf8("general")){
                sOutput += "<p>" + trUtf8("Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.") + "</p>\n";
                sOutput += "</div>\n"
                           "</div>\n";
            }
            // Article tested with ubuntu versions
            else{
                sOutput += "<ul>\n";

                for (int i = 1; i < sListElements.size(); i++){
                    sOutput += "<li>\n"
                               "<p>\n";
                    if (sListElements[i].toLower() == "precise")
                        sOutput += "<a href=\"" + sWikiUrl + "/Precise_Pangolin\" class=\"internal\"> Ubuntu Precise Pangolin 12.04 </a>\n";
                    else if (sListElements[i].toLower() == "oneiric")
                        sOutput += "<a href=\"" + sWikiUrl + "/Oneiric_Ocelot\" class=\"internal\"> Ubuntu Oneiric Ocelot 11.10 </a>\n";
                    else if (sListElements[i].toLower() == "natty")
                        sOutput += "<a href=\"" + sWikiUrl + "/Natty_Narwhal\" class=\"internal\"> Ubuntu Natty Narwhal 11.04 </a>\n";
                    else if (sListElements[i].toLower() == "maverick")
                        sOutput += "<a href=\"" + sWikiUrl + "/Maverick_Meerkat\" class=\"internal\"> Ubuntu Maverick Meerkat 10.10 </a>\n";
                    else if (sListElements[i].toLower() == "lucid")
                        sOutput += "<a href=\"" + sWikiUrl + "/Lucid_Lynx\" class=\"internal\"> Ubuntu Lucid Lynx 10.04 </a>\n";
                    else if (sListElements[i].toLower() == "hardy")
                        sOutput += "<a href=\"" + sWikiUrl + "/Hardy_Heron\" class=\"internal\"> Ubuntu Hardy Heron 8.04 </a>\n";
                    else
                        sOutput +=  "Unknown Ubuntu Version\n";

                    sOutput += "</p>\n"
                               "</li>\n";
                }
                sOutput += "</ul>\n"
                           "</div>\n"
                           "</div>\n";
            }
        }
        else {
            sOutput += "<p>";
            sOutput += trUtf8("Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte diesen Artikel testen und das getestet-Tag entsprechend anpassen.");
            sOutput += "</p>"
                       "</div>\n"
                       "</div>\n";
        }
    }
    // -----------------------------------------------------------------------------------------------

    // ADVANCED (Fortgeschritten)
    else if (sListElements[0] == trUtf8("Fortgeschritten")){
        // Generate output
        sOutput = "<div class=\"box advanced\">\n"
                  "<h3 class=\"box advanced\">";
        sOutput += trUtf8("Artikel für fortgeschrittene Anwender");
        sOutput += "</h3>\n"
                   "<div class=\"contents\">\n"
                   "<p>";
        sOutput += trUtf8("Dieser Artikel erfordert mehr Erfahrung im Umgang mit Linux und ist daher nur für fortgeschrittene Benutzer gedacht.");
        sOutput += "</p>\n"
                   "</div>\n"
                   "</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // AWARD (Auszeichnung)
    else if (sListElements[0] == trUtf8("Award")){

        sOutput = "<div class=\"box award\">\n"
                  "<h3 class=\"box award\">";
        if (sListElements.size() >= 4){
            sOutput += sListElements[1] + "</h3>\n"
                       "<div class=\"contents\">\n"
                       "<p>";
            QString sTmpAwardLink = "<a href=\"" + sListElements[2] + "\" rel=\"nofollow\" class=\"external\">" + sListElements[1] + "</a>";
            sOutput += trUtf8("Diese Anwendung hat die Auszeichnung %1 in der Kategorie %2 gewonnen.").arg(sTmpAwardLink).arg(sListElements[3]);
            if(sListElements.size() == 5){
                sOutput += " ";
                sOutput += trUtf8("Die Auszeichnung wurde an %1 überreicht.").arg(sListElements[4]);
                sOutput += "</p></div>\n";
            }
            else{
                sOutput += "</p></div>\n";
            }
        }
        else{
            sOutput += "</h3>\n";
        }
        sOutput += "</div>\n";

    }
    // -----------------------------------------------------------------------------------------------

    // PACKAGE-LIST (Pakete-Makro)  -- OBSOLETE --
    else if (sListElements[0] == trUtf8("Pakete")){

        // Generate output
        sOutput = "<div class=\"package-list\">\n"
                  "<div class=\"contents\">\n"
                  "<p>";
        sOutput += trUtf8("Paketliste zum Kopieren:");
        sOutput += " <strong>apt-get</strong></p>\n"
                   "<div class=\"bash\" style=\"display: block;\">"
                   "<div class=\"contents\">\n"
                   "<pre> sudo apt-get install";
        for (int i = 1; i < sListElements.size(); i++){
            sOutput += " " + sListElements[i];
        }
        sOutput += "</pre>\n</div>\n</div>\n</div>\n</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // IMPROVABLE (Ausbaufähig)
    else if (sListElements[0] == trUtf8("Ausbaufähig")){

        // Generate output
        sOutput = "<div class=\"box improvable\">\n"
                  "<h3 class=\"box improvable\">";
        sOutput += trUtf8("Ausbaufähige Anleitung");
        sOutput += "</h3>\n"
                   "<div class=\"contents\">\n"
                   "<p>";
        sOutput += trUtf8("Dieser Anleitung fehlen noch einige Informationen. Wenn Du etwas verbessern kannst, dann editiere den Beitrag, um die Qualität des Wikis noch weiter zu verbessern.");
        sOutput += "</p>\n";

        // Remark available
        if (sListElements.size() == 2){
            if (sListElements[1] != "" && sListElements[1] != " "){
                sOutput += "<hr />\n<p><strong>" + trUtf8("Anmerkung:") + "</strong> " + sListElements[1] + "</p>\n";
            }
        }

        sOutput += "</div>\n"
                   "</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // FIXME (Fehlerhaft)
    else if (sListElements[0] == trUtf8("Fehlerhaft")){

        sOutput = "<div class=\"box fixme\">\n";
        sOutput += "<h3 class=\"box fixme\">" + trUtf8("Fehlerhafte Anleitung") +  "</h3>\n";
        sOutput += "<div class=\"contents\">\n";
        sOutput += "<p>" + trUtf8("Diese Anleitung ist fehlerhaft. Wenn du weißt, wie du sie ausbessern kannst, nimm dir bitte die Zeit und bessere sie aus.") + "</p>\n";

        // Remark available
        if (sListElements.size() == 2){
            if (sListElements[1] != "" && sListElements[1] != " ")
                sOutput += "<hr />\n<p><strong>" + trUtf8("Anmerkung:") + "</strong> "  + sListElements[1] + "</p>\n";
        }

        sOutput += "</div>\n"
                   "</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // LEFT (Verlassen)
    else if (sListElements[0] == trUtf8("Verlassen")){

        // Generate output
        sOutput = "<div class=\"box left\">\n";
        sOutput += "<h3 class=\"box left\">" + trUtf8("Verlassene Anleitung") +  "</h3>\n";
        sOutput += "<div class=\"contents\">\n";
        sOutput += "<p>" + trUtf8("Dieser Artikel wurde von seinem Ersteller verlassen und wird nicht mehr weiter von ihm gepflegt. Wenn Du den Artikel fertigstellen oder erweitern kannst, dann bessere ihn bitte aus.") + "</p>\n";
        // Remark available
        if (sListElements.size() == 2){
            if (sListElements[1] != "" && sListElements[1] != " ")
                sOutput += "<hr />\n<p><strong>" + trUtf8("Anmerkung:") + "</strong> " + sListElements[1] + "</p>\n";
        }

        sOutput += "</div>\n"
                   "</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // ARCHIVED (Archiviert)
    else if (sListElements[0] == trUtf8("Archiviert")){

        // Generate output
        sOutput = "<div class=\"box improvable\">\n";
        sOutput += "<h3 class=\"box improvable\">" + trUtf8("Archivierte Anleitung") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n";
        sOutput += "<p>" + trUtf8("Dieser Artikel wurde archiviert, da er - oder Teile daraus - nur noch unter einer älteren Ubuntu-Version nutzbar ist. Diese Anleitung wird vom Wiki-Team weder auf Richtigkeit überprüft noch anderweitig gepflegt. Zusätzlich wurde der Artikel für weitere Änderungen gesperrt.") + "</p>\n";
        // Remark available
        if (sListElements.size() == 2){
            if (sListElements[1] != "" && sListElements[1] != " ")
                sOutput += "<hr />\n<p><strong>"+ trUtf8("Anmerkung:") + "</strong> " + sListElements[1] + "</p>\n";
        }

        sOutput += "</div>\n"
                   "</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // COPY (Kopie)
    else if (sListElements[0] == trUtf8("Kopie") && sListElements.size() >= 2){

        // Replace possible spaces
        for (int i = 1; i < sListElements.size(); i++){
            sListElements[i].replace(" ", "_");
        }

        // Generate output
        sOutput = "<div class=\"box warning\">\n";
        sOutput += "<h3 class=\"box warning\">" + trUtf8("Achtung!") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n";
        QString sTmpLink = "<a href=\"" + sWikiUrl + "/" + trUtf8("Baustelle") + "/" + sListElements[1] + "\" class=\"internal missing\">" + trUtf8("Baustelle") + "/" + sListElements[1] + "</a>";
        sOutput += "<p>" + trUtf8("Diese Seite wird aktuell überarbeitet. Bitte hier keine Änderungen mehr vornehmen, sondern in %1!").arg(sTmpLink) + "</p>\n";
        sOutput += "</div>\n"
                   "</div>\n";

    }
    // -----------------------------------------------------------------------------------------------

    // WORK IN PROGRESS (Überarbeitung)
    else if (sListElements[0] == trUtf8("Überarbeitung")){
        sOutput = "<div class=\"box workinprogress\">\n";
        sOutput += "<h3 class=\"box workinprogress\">" + trUtf8("Artikel wird überarbeitet") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n";
        sOutput += "<p>" + trUtf8("Dieser Artikel wird momentan überarbeitet.") + "</p>\n";

        // Correct number of elements?
        if (sListElements.size() == 4){

            // Replace possible spaces
            for (int i = 1; i < sListElements.size(); i++){
                sListElements[i].replace(" ", "_");
            }

            QString sTmpUrl = sWikiUrl;
            sTmpUrl.remove("wiki.");
            // Generate output
            sOutput += "<ul>";
            sOutput += "<li><p>" + trUtf8("geplante Fertigstellung:") + " " + sListElements[1] + "</p></li>\n";
            QString sTmpLink = "<a href=\"" + sWikiUrl + "/" + sListElements[2] + "\" class=\"internal missing\"> " + sListElements[2] +" </a>";
            sOutput += "<li><p>" + trUtf8("derzeit gültiger Artikel:") + " " + sTmpLink + "</p></li>\n";
            QString sTmpLink2 = " <a href=\"" + sTmpUrl + "/user/" + sListElements[3] + "\" class=\"crosslink user\"> " + sListElements[3] + " </a>";
            sOutput += "<li><p>" + trUtf8("Bearbeiter:") + sTmpLink2 + "</p></li>\n";
            sOutput += "</ul>\n";
            QString sTmpLink3 = "<a href=\"" + sWikiUrl + "/" + trUtf8("Baustelle") + "/" + sListElements[2] + "?action=log\" class=\"crosslink\">" + trUtf8("letzten Änderung") + "</a>";
            sOutput += "<p>" + trUtf8("Solltest du dir nicht sicher sein, ob an dieser Anleitung noch gearbeitet wird, kontrolliere das Datum der %1 und entscheide, wie du weiter vorgehst.").arg(sTmpLink3) + "</p>\n";
        }

        sOutput += "<hr />\n";
        sOutput += "<p><strong>" + trUtf8("Achtung") + ": </strong>" + trUtf8("Insbesondere heißt das, dass dieser Artikel noch nicht fertig ist und dass wichtige Teile fehlen oder sogar falsch sein können. Bitte diesen Artikel nicht als Anleitung für Problemlösungen benutzen!") + "</p>\n";
        sOutput += "</div>\n"
                   "</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // THIRD-PARTY SOURCE / PACKAGE / SOFTWARE WARNING (Fremdquellen / -pakete / -software Warnung)
    else if (sListElements[0] == trUtf8("Fremd")){
        sOutput = "<div class=\"box warning\">\n";
        sOutput += "<h3 class=\"box warning\">" + trUtf8("Hinweis!") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n";

        if (sListElements.size() >= 2){

            // Package
            if (sListElements[1] == trUtf8("Paket")){
                sOutput += "<p><a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdpakete") + "</a> " + trUtf8("können das System gefährden.") + "</p>\n";
            }
            // Source
            else if (sListElements[1] == trUtf8("Quelle")){
                sOutput += "<p>" + trUtf8("Zusätzliche") + " <a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdquellen") + "</a> " + trUtf8("können das System gefährden.") + "</p>\n";
            }
            //Software
            else if (sListElements[1] == trUtf8("Software")){
                sOutput += "<p><a href=\"" + sWikiUrl + "/" + trUtf8("Fremdsoftware") + "\" class=\"internal\">" + trUtf8("Fremdsoftware") + "</a> " + trUtf8("kann das System gefährden.") + "</p>\n";
            }

            // Remark available
            if (sListElements.size() >= 3){
                if (sListElements[2].startsWith(" "))
                    sListElements[2].remove(0, 1);
                if (sListElements[2] != "" && sListElements[2] != " "){
                    sOutput += "<hr />\n";
                    sOutput += "<p><strong>" + trUtf8("Anmerkung") + ": </strong>" + sListElements[2] + "</p>";
                }
            }
        }

        sOutput += "</div>\n</div>\n";
    }
    // -----------------------------------------------------------------------------------------------

    // PPA
    else if (sListElements[0] == trUtf8("PPA")){
        if (sListElements.size() == 3){
            // Replace possible spaces
            for (int i = 1; i < sListElements.size(); i++){
                sListElements[i].replace(" ", "_");
            }

            // Generate output
            sOutput = "<ul>\n"
                      "<li>\n";
            sOutput += "<p><strong>ppa:" + sListElements[1] + "/" + sListElements[2] + "</strong></p>\n";
            sOutput += "</li>\n"
                       "</ul>";

            sOutput += "<div class=\"box warning\">\n";
            sOutput += "<h3 class=\"box warning\">" + trUtf8("Hinweis!") + "</h3>\n";
            sOutput += "<div class=\"contents\">\n";
            sOutput += "<p>" + trUtf8("Zusätzliche") + " <a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdquellen") + "</a> " + trUtf8("können das System gefährden.") + "</p>\n<hr />";
            QString sTmpLink = "<img src=\"img/interwiki/ppa.png\" class=\"image-default\" alt=\"PPA\" /> <a href=\"https://launchpad.net/~" + sListElements[1] + "/+archive/" + sListElements[2] + "\" rel=\"nofollow\" class=\"external\">" + trUtf8("PPA Beschreibung") + "</a>";
            QString sTmpLink2 = "<a href=\"https://launchpad.net/~" + sListElements[1] + "\" class=\"interwiki interwiki-lpuser\">" + sListElements[1] + "</a>";
            sOutput += "<p>" + trUtf8("Weitere Informationen bietet die %1 vom Benutzer/Team %2.").arg(sTmpLink).arg(sTmpLink2) + "</p>\n";
            sOutput += "</div>\n"
                       "</div>\n";
        }
    }
    // -----------------------------------------------------------------------------------------------

    // AUTHENTICATE THIRD-PARTY REPO (Fremdquelle authentifizieren)
    else if (sListElements[0] == trUtf8("Fremdquelle-auth")){
        if (sListElements.size() == 2){

            // Key
            if (sListElements[1].startsWith("key")){
                sListElements[1].remove("key");
                sListElements[1].remove(" ");

                QString sTmpLink = "<a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdquelle") + "</a>";
                sOutput = "<p>" + trUtf8("Um die %1 zu authentifizieren, kann man den Signierungsschlüssel mit folgendem Befehl importieren:").arg(sTmpLink);
                sOutput += "</p>\n"
                           "<div class=\"bash\">\n"
                           "<div class=\"contents\">\n"
                           "<pre> sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com " + sListElements[1] + " </pre>\n"
                           "</div>\n"
                           "</div>\n";
            }
            // Url
            else{
                QString sTmpLink = "<a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdquelle") + "</a>";
                QString sTmpLink2 = "<a href=\"" + sListElements[1] + "\" rel=\"nofollow\" class=\"external\">" + trUtf8("Signierungsschlüssel herunterladen") + "</a>";
                QString sTmpLink3 = "<a href=\"" + sWikiUrl + "/" + trUtf8("Paketquellen_freischalten") + "\" class=\"internal\">" + trUtf8("Paketverwaltung hinzufügen") + "</a>";
                sOutput = "<p>" + trUtf8("Um die %1 zu authentifizieren, kann man entweder den %2 und in der %3 oder folgenden Befehl ausführen:").arg(sTmpLink).arg(sTmpLink2).arg(sTmpLink3) + "</p>\n";
                sOutput += "<div class=\"bash\">\n"
                           "<div class=\"contents\">\n"
                           "<pre>wget -q " + sListElements[1] + " -O- | sudo apt-key add - </pre>\n"
                           "</div>\n"
                           "</div>\n";
            }
        }
    }

    // -----------------------------------------------------------------------------------------------

    // THIRD-PARTY REPO (Fremdquelle)
    else if (sListElements[0] == trUtf8("Fremdquelle")){
        if (sListElements.size() >= 3){

            // Generate output
            QString sTmpLink = "<a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdquelle") + "</a>";
            QString sTmpLink2 = "<a href=\"" + sWikiUrl + "/" + trUtf8("Paketquellen_freischalten") + "\" class=\"internal\">" + trUtf8("Paketquellen freischalten") + "</a>";
            sOutput = "<p>" + trUtf8("Um aus der %1 zu installieren, muss man die folgenden %2:").arg(sTmpLink).arg(sTmpLink2) + "</p>\n";
            sOutput += "<div class=\"box warning\">\n"
                       "<h3 class=\"box warning\">" + trUtf8("Hinweis!") + "</h3>\n"
                       "<div class=\"contents\">\n";
            QString sTmpLink3 = "<a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdquelle") + "</a>";
            sOutput += "<p>" + trUtf8("Zusätzliche %3 können das System gefährden.").arg(sTmpLink3) + "</p>\n"
                       "</div>\n"
                       "</div>\n"
                       "<div class=\"thirpartyrepo-outer\">\n"
                       "<div class=\"contents\">\n"
                       "<div class=\"selector\">\n"
                       "<strong>" + trUtf8("Version:") + " </strong>" + sListElements[2] + "\n"
                       "</div>\n";

            if (sListElements.size() == 3){
                sOutput += "<pre> deb " + sListElements[1] + " " + sListElements[2] + " </pre>";
            }
            else{
                if (sListElements[3].startsWith(" "))
                    sListElements[3].remove(0, 1);
                sOutput += "<pre> deb " + sListElements[1] + " "+ sListElements[2] + " " + sListElements[3] + "</pre>";
            }

            sOutput += "</div>\n"
                       "</div>\n";
        }


    }

    // -----------------------------------------------------------------------------------------------

    // THIRD-PARTY PACKAGE (Fremdpaket)
    else if (sListElements[0] == trUtf8("Fremdpaket")){
        if (sListElements.size() >= 3){

            // Case 1: [[Vorlage(Fremdpaket, Projekthoster, Projektname, Ubuntuversion(en))]]
            if (!(sListElements[2].startsWith("http")) && sListElements[2] != "dl"){
                // LAUNCHPAD
                if (sListElements[1] == "launchpad"){
                    sOutput = "<p>" + trUtf8("Beim <a href=\"%1/Launchpad\" class=\"internal\">Launchpad</a>-Projekt "
                                     "<a href=\"https://launchpad.net/%2\" class=\"interwiki interwiki-launchpad\">%3</a> "
                                     "werden <a href=\"https://launchpad.net/%4/+download\" class=\"interwiki interwiki-launchpad\">DEB-Pakete</a> "
                                     "angeboten. ")
                            .arg(sWikiUrl)
                            .arg(sListElements[2])
                            .arg(sListElements[2])
                            .arg(sListElements[2]);
                }
                // SOURCEFORGE
                else if (sListElements[1] == "sourceforge"){
                    sOutput = "<p>" + trUtf8("Beim <a href=\"http://de.wikipedia.org/wiki/SourceForge\" class=\"interwiki interwiki-wikipedia\">SourceForge</a>-Projekt "
                                     "<a href=\"http://sourceforge.net/projets/%1\" class=\"interwiki interwiki-sourceforge\">%2</a> "
                                     "werden <a href=\"http://sourceforge.net/projects/%3/files\" class=\"interwiki interwiki-sourceforge\">DEB-Pakete</a> "
                                     "angeboten. ")
                            .arg(sListElements[2])
                            .arg(sListElements[2])
                            .arg(sListElements[2]);
                }
                // GOOGLE CODE
                else if (sListElements[1] == "googlecode"){
                    sOutput = "<p>" + trUtf8("Beim <a href=\"http://code.google.com/intl/de\" rel =\"nofollow\" class=\"external\">Google Code</a> "
                                     "<img src=\"img/flags/de.png\" alt=\"(de)\" /> -Projekt "
                                     "<a href=\"http://code.google.com/p/%1\" class=\"interwiki interwiki-googlecode\">%2</a> "
                                     "werden <a href=\"http://code.google.com/p/%3/downloads\" class=\"interwiki interwiki-googlecode\">DEB-Pakete</a> "
                                     "angeboten. ")
                            .arg(sListElements[2])
                            .arg(sListElements[2])
                            .arg(sListElements[2]);
                }
            }

            // Case 2: [[Vorlage(Fremdpaket, "Anbieter", URL Downloadübersicht, Ubuntuversion(en))]]
            else if (sListElements[2].startsWith("http")){
                sOutput = "<p>" + trUtf8("Von %1 werden <a href=\"%2\" class=\"external\">DEB-Pakete</a> <img src=\"img/flags/dl.png\" alt=\"(dl)\" /> angeboten. ")
                          .arg(sListElements[1]).arg(sListElements[2]); // [http: ] WAS REPLACED AUTOMATICALY BEVORE!!!
            }

            // Case 3: [[Vorlage(Fremdpaket, "Anbieter", dl, URL zu einem Download, Ubuntuversion(en))]]
            else if (sListElements[2].startsWith("dl")){
                sOutput = "<p>" + trUtf8("Von %1 werden folgende DEB-Pakete angeboten:").arg(sListElements[1]) + "</p>"
                          "<ul>\n<li><p><a href=\"" + sListElements[3] + "\" rel=\"nofollow\" class=\"external\">" + sListElements[3] + "</a> "
                          "<img src=\"img/flags/dl.png\" alt=\"(dl)\" /></p></li>\n</ul>\n";
            }


        }

        // No ubuntu version
        if ((sListElements.size() == 3 && !(sListElements[2].startsWith("dl"))) || (sListElements.size() == 4 && sListElements[2].startsWith("dl"))){
            sOutput += trUtf8("Die unterstützten Ubuntuversionen und Architekturen werden aufgelistet.") + " ";
        }
        else{
            if (sListElements.size() >= 3){
                int i = 3;
                if (sListElements[2].startsWith("dl"))
                    i = 4;

                QString sUbuntuVersions("");
                for (int j = i; j < sListElements.size(); j++){
                    sUbuntuVersions += sListElements[j] + " ";
                }
                sOutput += trUtf8("Die Pakete können für %1 heruntergeladen werden. ").arg(sUbuntuVersions);
            }
        }
        QString sTmpLink = "<a href=\"" + sWikiUrl + "/" + trUtf8("Paketinstallation_DEB") + "\" class=\"internal\">" + trUtf8("DEB-Pakete noch installiert werden") + "</a>";
        sOutput += trUtf8("Nachdem man sie für die korrekte Ubuntuversion geladen hat, müssen die %1.").arg(sTmpLink) + "</p>\n";

        // Warning box
        sOutput += "<div class=\"box warning\">\n"
                   "<h3 class=\"box warning\">" + trUtf8("Hinweis!") + "</h3>\n"
                   "<div class=\"contents\"\n"
                   "<p><a href=\"" + sWikiUrl + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">" + trUtf8("Fremdpakete") + "</a> " + trUtf8("können das Sytem gefährden.") + "</p>\n"
                   "</div>\n</div>\n";

    }
    // -----------------------------------------------------------------------------------------------

    // IMAGE WITH SUBSCRIPTION
    else if (sListElements[0] == trUtf8("Bildunterschrift")){
        QString sImageLink("");
        QString sImageWidth("");
        QString sImageDescription("");
        QString sImageAlign("left");
        QString sImageStyle("");
        double iImgHeight, iImgWidth;

        sImageLink = sListElements[1].trimmed();

        for (int i = 2; i < sListElements.length(); i++){
            // Found integer -> width
            if(sListElements[i].trimmed().toUInt() != 0){
                sImageWidth = sListElements[i].trimmed();
            }
            // Alignment
            else if (sListElements[i].trimmed() == "left" || sListElements[i].trimmed() == "right"){
                sImageAlign = sListElements[i].trimmed();
            }
            // Style
            else if (sListElements[i].trimmed() == "xfce-style" || sListElements[i].trimmed() == "kde-style" ||
                     sListElements[i].trimmed() == "edu-style" || sListElements[i].trimmed() == "lxde-style" ||
                     sListElements[i].trimmed() == "studio-style"){
                sImageStyle = sListElements[i].trimmed().remove("-style");
            }
            // Everything else is description...
            else {
                sImageDescription += sListElements[i];
            }
        }

        iImgWidth = QImage(sImageLink).width();
        if (sImageWidth != ""){
            iImgHeight = (double)QImage(sImageLink).height() / (iImgWidth / sImageWidth.toDouble());
        }
        // Default
        else {
            sImageWidth = "140";
            iImgHeight = (double)QImage(sImageLink).height() / (iImgWidth / 140);
        }

        sOutput = "<table style=\"float: " + sImageAlign + "; clear: both; border: none\">\n<tbody>\n";

        // No style info -> default
        if (sImageStyle == ""){
            sOutput += "<tr class=\"titel\">\n";
        }
        else{
            sOutput += "<tr class=\"" + sImageStyle + "-titel\">\n";
        }

        sOutput += "<td>\n<a href=\"" + sImageLink + "\" class=\"crosslink\">\n" +
                   "<img src=\"" + sImageLink + "\" alt=\"" + sImageLink + "\" class=\"image-default\" " +
                   "height=\"" + QString::number((int)iImgHeight) + "\" width=\"" + sImageWidth + "\"/>\n</a>\n" +
                   "</td>\n</tr>\n";

        // No style info -> default
        if (sImageStyle == ""){
            sOutput += "<tr class=\"kopf\">\n";
        }
        else{
            sOutput += "<tr class=\"" + sImageStyle + "-kopf\">\n";
        }

        sOutput += "<td style=\"text-align: center\">" + sImageDescription + "</td>\n</tr>" +
                   "</tbody>\n</table>\n";

    }


    // -----------------------------------------------------------------------------------------------

    return (sOutput);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Inhaltsverzeichnis erstellen
QString CParser::parseTableOfContents(QTextBlock tabofcontents){

    QString sLine = tabofcontents.text();
    QString sOutput("TABLE OF CONTENT");

    // Remove brackets
    sLine.remove(trUtf8("[[Inhaltsverzeichnis"), Qt::CaseSensitive);
    sLine.remove("(");
    sLine.remove(")");

    /*
    if (sLine == "" || sLine != "1"){
        QMessageBox::information(0, "Information", tr("The preview of table of contents does not supports sub headlines currently.", "Msg: Table of contents does bot support sub headlines"));
    }
    */

    QTextBlock curBlock = rawText->firstBlock();
    QStringList sListHeadlineLevel1, sListHeadlineLevel1_Links;
    QString sTmpString;

    for(; curBlock.isValid() && !(rawText->lastBlock() < curBlock); curBlock = curBlock.next()) {
        if (curBlock.text().startsWith("= ") || curBlock.text().startsWith(" = ")){
            sTmpString = curBlock.text();

            sTmpString.remove("=");
            sTmpString = sTmpString.trimmed();
            sListHeadlineLevel1 << sTmpString;

            // Replace characters for valid links (ä, ü, ö, spaces)
            sTmpString.replace(" ", "-");
            sTmpString.replace(QString::fromUtf8("Ä"), "Ae");
            sTmpString.replace(QString::fromUtf8("Ü"), "Ue");
            sTmpString.replace(QString::fromUtf8("Ö"), "Oe");
            sTmpString.replace(QString::fromUtf8("ä"), "ae");
            sTmpString.replace(QString::fromUtf8("ü"), "ue");
            sTmpString.replace(QString::fromUtf8("ö"), "oe");
            sListHeadlineLevel1_Links << sTmpString;
        }
    }

    sOutput = "<div class=\"toc\">\n<div class=\"head\">" + trUtf8("Inhaltsverzeichnis") + "</div>\n<ol class=\"arabic\">\n";

    for (int i = 0; i < sListHeadlineLevel1.size(); i++){
        sOutput += "<li>\n<a href=\"#" + sListHeadlineLevel1_Links[i] + "\" class=\"crosslink\">" + sListHeadlineLevel1[i] + "</a>\n</li>\n";
    }

    sOutput += "</ol>\n</div>\n";

    return sOutput;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// TAGS (end of page)
QString CParser::generateTags(QTextBlock actParagraph){

    QString sParagraph = actParagraph.text();
    QString sOutput("");

    // Remove tag sample
    sParagraph.remove("#tag:");
    sParagraph.remove("# tag:");

    // Separate elements
    QStringList sListElements = sParagraph.split(",");
    if  (sListElements.size() == 0){
        sOutput = " ";
    }
    else{
        // Remove spaces and generate output
        for (int i = 0; i < sListElements.size(); i++){
            sListElements[i].remove(" ");
            sOutput += " <a href=\"" + sWikiUrl + "/Wiki/Tags?tag=" + sListElements[i] + "\">" + sListElements[i] + "</a>";
            if (i < sListElements.size()-1)
                sOutput += ",";
        }
    }
    return sOutput;

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Headline
QString CParser::parseHeadline(QTextBlock actParagraph){
    QString sParagraph = actParagraph.text();
    QString sOutput("<strong>FOUND WRONG FORMATED HEADLINE</strong>\n");
    QString sLink("");
    unsigned short usHeadlineLevel = 5;

    // Remove spaces at beginning and end
    sParagraph = sParagraph.trimmed();

    // Order is important! First level 5, 4, 3, 2, 1
    if (sParagraph.startsWith("=====")){
        usHeadlineLevel = 5;
    }
    else if (sParagraph.startsWith("====")){
        usHeadlineLevel = 4;
    }
    else if (sParagraph.startsWith("===")){
        usHeadlineLevel = 3;
    }
    else if (sParagraph.startsWith("==")){
        usHeadlineLevel = 2;
    }
    else if (sParagraph.startsWith("=")){
        usHeadlineLevel = 1;
    }

    // Remove = and spaces at beginning and end
    sParagraph.remove("=");
    sParagraph = sParagraph.trimmed();

    // Replace characters for valid link
    sLink = sParagraph;
    sLink.replace(" ", "-");
    sLink.replace(QString::fromUtf8("Ä"), "Ae");
    sLink.replace(QString::fromUtf8("Ü"), "Ue");
    sLink.replace(QString::fromUtf8("Ö"), "Oe");
    sLink.replace(QString::fromUtf8("ä"), "ae");
    sLink.replace(QString::fromUtf8("ü"), "ue");
    sLink.replace(QString::fromUtf8("ö"), "oe");

    // usHeadlineLevel + 1 !!!
    sOutput = "<h" + QString::number(usHeadlineLevel+1) + " id=\"" + sLink + "\">" + sParagraph + " <a href=\"#" + sLink + "\" class=\"headerlink\"> &para;</a></h" + QString::number(usHeadlineLevel+1) + ">\n";

    return sOutput;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Text samples {{{!#vorlage
QString CParser::parseTextSample(QString actParagraph){
    QString sParagraph = actParagraph;

    sParagraph.remove("{{{#!vorlage ");

    // Separate elementes from macro (between §)
    QStringList sListElements = sParagraph.split("§");
    for (int i = 0; i < sListElements.length(); i++){
        sListElements[i] = sListElements[i].trimmed();
    }

    QString sOutput("<strong>FOUND UNKNOWN ITEM: {{{#!vorlage " + sListElements[0] + "</strong>\n");

    // KNOWLEGE BOX (Wissensblock)
    if (sListElements[0] == trUtf8("Wissen")){
        sOutput = "<div class=\"box knowledge\">\n";
        sOutput += "<h3 class=\"box knowledge\">" + trUtf8("Zum Verständnis dieses Artikels sind folgende Seiten hilfreich:") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n"
                   "<ol class=\"arabic\">\n";

        for (int i = 1; i < sListElements.length(); i++){
            sOutput += "<li>\n<p><a id=\"source-" + QString::number(i) + "\" href=\"#source-" + QString::number(i) + "\" class=\"crosslink anchor\"> </a>\n" + sListElements[i] + "</p>\n</li>\n";
        }

        sOutput += "</ol>\n"
                   "</div>\n"
                   "</div>\n";
    }

    // WARNING (Warnung)
    else if (sListElements[0] == trUtf8("Warnung")){
        sOutput = "<div class=\"box warning\">\n";
        sOutput += "<h3 class=\"box warning\">" + trUtf8("Achtung!") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n"
                   "<p>";
        for (int i = 1; i < sListElements.length(); i++){
            sOutput += sListElements[i] + " ";
        }
        sOutput += "</p>\n"
                   "</div>\n"
                   "</div>\n";
    }

    // NOTICE (Hinweis)
    else if (sListElements[0] == trUtf8("Hinweis")){
        sOutput = "<div class=\"box notice\">\n";
        sOutput += "<h3 class=\"box notice\">" + trUtf8("Hinweis:") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n"
                   "<p>";
        for (int i = 1; i < sListElements.length(); i++){
            sOutput += sListElements[i] + " ";
        }
        sOutput += "</p>\n"
                   "</div>\n"
                   "</div>\n";
    }

    // EXPERT-INFO (Experteninformationen)
    else if (sListElements[0] == trUtf8("Experten")){
        sOutput = "<div class=\"box experts\">\n";
        sOutput += "<h3 class=\"box experts\">" + trUtf8("Experten-Info:") + "</h3>\n";
        sOutput += "<div class=\"contents\">\n"
                   "<p>";
        for (int i = 1; i < sListElements.length(); i++){
            sOutput += sListElements[i] + " ";
        }
        sOutput += "</p>\n"
                   "</div>\n"
                   "</div>\n";
    }

    // BASH (Befehl)
    else if (sListElements[0] == trUtf8("Befehl")){
        sOutput = "<div class=\"bash\">\n"
                  "<div class=\"contents\">\n"
                  "<pre>";
        for (int i = 1; i < sListElements.length(); i++){
            sListElements[i].replace("<", "&lt;");
            sOutput += sListElements[i] + "\n";
        }
        sOutput += "</pre>\n"
                   "</div>\n"
                   "</div>\n";
    }

    // PACKAGE INSTALLATION (Paketinstallation)
    else if (sListElements[0] == trUtf8("Paketinstallation")){
        QStringList sListPackages, sListPackagesTMP;

        sOutput = "<ul>\n";

        for (int i = 1; i < sListElements.length(); i++){
            sListPackagesTMP.clear();
            sListPackagesTMP = sListElements[i].split(",");

            // Package with source
            if (sListPackagesTMP.length() >= 2){
                sOutput += "<li><p><strong>" + sListPackagesTMP[0].trimmed() + "</strong> (<em>" + sListPackagesTMP[1].trimmed() + "</em>)</p></li>";
            }
            // Only package
            else {
                sOutput += "<li><p><strong>" + sListPackagesTMP[0].trimmed() + "</strong></p></li>";
            }

            sListPackages << sListPackagesTMP[0].trimmed();
        }
        sOutput += "</ul>\n"
                   "<p>\n"
                   "<a href=\"apt://";

        // Apt packages install list / button
        for (int i = 0; i < sListPackages.length(); i++){
            sOutput += sListPackages[i].trimmed();
            if (i < sListPackages.size()-1)
                sOutput += ",";
        }

        sOutput += "\" rel=\"nofollow\" class=\"external\">"
                   "<img src=\"img/wiki/button.png\" alt=\"Wiki-Installbutton\" class=\"image-default\" /></a>"
                   "</p>";
    }

    // TABLE (Tabelle)
    else if (sListElements[0] == trUtf8("Tabelle")){
        QRegExp tablePattern("\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
        QRegExp connectCells("-\\d{1,2}");
        QRegExp connectRows("\\|\\d{1,2}");
        QRegExp rowclassPattern("rowclass=\\\"[\\w.%-]+\\\"");
        QRegExp cellclassPattern("cellclass=\\\"[\\w.%-]+\\\"");

        bool bCellStyleWasSet;
        QRegExp cellStylePattern("cellstyle=\\\"[\\w\\s:;%#-]+\\\"");
        QRegExp rowStylePattern("rowstyle=\\\"[\\w\\s:;%#-]+\\\"");
        QRegExp tableStylePattern("tablestyle=\\\"[\\w\\s:;%#-]+\\\"");

        int iLength;
        QString sTmpCellStyle, sStyleInfo, sTmpTD;

        if (sListElements.length() >= 2){
            if (tableStylePattern.indexIn(sListElements[1]) >= 0){
                sTmpCellStyle = tableStylePattern.cap();
                sOutput = "<table style=" + sTmpCellStyle.remove("tablestyle=") + ">\n<tbody>\n";
            }
            else{
                sOutput = "<table>\n<tbody>\n";
            }
        }
        else {
            sOutput = "<table>\n<tbody>\n";
        }

        for (int i = 1; i < sListElements.length(); i++){

            bCellStyleWasSet = false;
            sTmpTD.clear();
            // New line
            if (sListElements[i] == "+++"){
                sOutput += "</tr>\n";
            }
            // New cell
            else{
                int myindex = tablePattern.indexIn(sListElements[i]);
                // Found style info && pattern which was found is not a <span class=...> element
                if (myindex >= 0 && !sListElements[i].trimmed().startsWith("<span")){

                    iLength = tablePattern.matchedLength();
                    sStyleInfo = tablePattern.cap();

                    // Start tr
                    if (i == 1 || sListElements[i-1] == "+++" ||
                        rowclassPattern.indexIn(sStyleInfo) >= 0 ||
                        rowStylePattern.indexIn(sStyleInfo) >= 0){
                        sOutput += "<tr";
                    }

                    // Found row class info --> in tr
                    if (rowclassPattern.indexIn(sStyleInfo) >= 0){
                        sTmpCellStyle = rowclassPattern.cap();
                        sOutput += " class=" + sTmpCellStyle.remove("rowclass=");
                    }
                    // Found row sytle info --> in tr
                    if (rowStylePattern.indexIn(sStyleInfo) >= 0){
                        sTmpCellStyle = rowStylePattern.cap();
                        sOutput += " style=\"" + sTmpCellStyle.remove("rowstyle=").remove("\"") + "\"";
                    }

                    // Close tr
                    if (i == 1 || sListElements[i-1] == "+++" ||
                        rowclassPattern.indexIn(sStyleInfo) >= 0 ||
                        rowStylePattern.indexIn(sStyleInfo) >= 0){
                        sOutput += ">\n";
                    }

                    // Start td
                    sOutput += "<td";

                    // Found cellclass info
                    if (cellclassPattern.indexIn(sStyleInfo) >= 0){
                        sTmpCellStyle = cellclassPattern.cap();
                        sTmpTD += " class=" + sTmpCellStyle.remove("cellclass=");
                    }

                    // Connect cells info (-integer, e.g. -3)
                    if (connectCells.indexIn(sStyleInfo) >= 0){
                        sTmpTD += " colspan=\"" + connectCells.cap().remove("-") + "\"";
                    }

                    // Connect ROWS info (|integer, e.g. |2)
                    if (connectRows.indexIn(sStyleInfo) >= 0){
                        sTmpTD += " rowspan=\"" + connectRows.cap().remove("|") + "\"";
                    }

                    // Cell style attributs
                    if (cellStylePattern.indexIn(sStyleInfo) >= 0){
                        sTmpTD += " style=\"" + cellStylePattern.cap().remove("cellstyle=").remove("\"");
                        bCellStyleWasSet = true;
                    }

                    // Text align center
                    if (sStyleInfo.contains("<:") || sStyleInfo.contains(" : ") || sStyleInfo.contains(":>")){
                        if (bCellStyleWasSet)
                            sTmpTD += " text-align: center;";
                        else
                            sTmpTD += " style=\"text-align: center;";
                        bCellStyleWasSet = true;
                    }
                    // Text align left
                    if (sStyleInfo.contains("<(") || sStyleInfo.contains(" ( ") || sStyleInfo.contains("(>")){
                        if (bCellStyleWasSet)
                            sTmpTD += " text-align: left;";
                        else
                            sTmpTD += " style=\"text-align: left;";
                        bCellStyleWasSet = true;
                    }
                    // Text align center
                    if (sStyleInfo.contains("<)") || sStyleInfo.contains(" ) ") || sStyleInfo.contains(")>")){
                        if (bCellStyleWasSet)
                            sTmpTD += " text-align: right;";
                        else
                            sTmpTD += " style=\"text-align: right;";
                        bCellStyleWasSet = true;
                    }
                    // Text vertical align top
                    if (sStyleInfo.contains("<^") || sStyleInfo.contains(" ^ ") || sStyleInfo.contains("^>")){
                        if (bCellStyleWasSet)
                            sTmpTD += " text-align: top;";
                        else
                            sTmpTD += " style=\"vertical-align: top;";
                        bCellStyleWasSet = true;
                    }
                    // Text vertical align bottom
                    if (sStyleInfo.contains("<v") || sStyleInfo.contains(" v ") || sStyleInfo.contains("v>")){
                        if (bCellStyleWasSet)
                            sTmpTD += " text-align: bottom;";
                        else
                            sTmpTD += " style=\"vertical-align: bottom;";
                        bCellStyleWasSet = true;
                    }

                    // Closing style section
                    if (bCellStyleWasSet)
                        sTmpTD += "\"";


                    // Remove style info (only remove, if line starts with "<" otherwise elemn)
                    if (sListElements[i].startsWith("<"))
                        sListElements[i].replace(myindex, iLength, "");

                    sOutput += sTmpTD + ">" + sListElements[i] + "</td>\n";
                }



                // Normal cell without style info
                else{
                    if (i == 1)
                        sOutput += "<tr>\n";
                    if (sListElements[i-1] == "+++")
                        sOutput += "<tr>\n";
                    sOutput += "<td>" + sListElements[i] + "</td>\n";
                }
            }


        }

        sOutput += "</tr></tbody>\n</table>";
    }
    return sOutput;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Code blocks {{{ and {{{!#code
QString CParser::parseCodeBlock(QString actParagraph){
    QString sParagraph = actParagraph;
    QString sOutput("<strong>FOUND WRONG FORMATED CODE BLOCK</strong>");

    sParagraph.remove("{{{#!code ");
    sParagraph.remove("{{{");

    // Separate elementes from macro (between §)
    QStringList sListElements = sParagraph.split("§");
    for (int i = 0; i < sListElements.length(); i++){
        sListElements[i] = sListElements[i].trimmed();
    }

    // Only plain code
    if (sListElements[0] == ""){
        sOutput = "<pre>";

        for (int i = 1; i < sListElements.length(); i++){
            sOutput += sListElements[i];
            if (i < sListElements.size()-1)
                sOutput += "\n";
        }

        sOutput += "</pre>\n";
    }
    // Syntax highlighting
    else{
        if (bShowedMsgBoxAlready == false){
            QMessageBox::information(0, "Information", tr("The preview does not support syntax highlighting in code block currently."));
            bShowedMsgBoxAlready = true;
        }

        //int iNumberOfLines = sListElements.size()-1;
        sOutput = "<div class=\"code\">\n<table class=\"syntaxtable\"><tbody>\n<tr>\n<td class=\"linenos\">\n<div class=\"linenodiv\"><pre>";

        // First column (line numbers)
        for (int i = 1; i < sListElements.size(); i++){
            sOutput += QString::number(i);
            if (i < sListElements.size()-1)
                sOutput += "\n";
        }

        sOutput += "</pre>\n</div>\n</td>\n<td class=\"code\">\n<div class=\"syntax\">\n<pre>\n";

        // Second column (code)
        for (int i = 1; i < sListElements.length(); i++){
            sOutput += sListElements[i];
            if (i < sListElements.size()-1)
                sOutput += "\n";
        }

        sOutput += "</pre>\n</div>\n</td>\n</tr>\n</tbody>\n</table>\n</div>";
    }

    return sOutput;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Image collection [[Vorlage(Bildersammlung, ... )]]
QString CParser::parseImageCollection(QString actParagraph){
    QString sParagraph = actParagraph;
    QString sOutput("");

    sParagraph.remove("[[Vorlage(Bildersammlung");
    sParagraph.remove(")]]");

    QString sImageCollHeight("140");
    QString sImageCollAlign("");
    QStringList sListImages;
    double iImgHeight, iImgWidth;

    // Separate elementes from macro (between §)
    QStringList sListElements = sParagraph.split("§");
    if (sListElements.length() == 0)
        return "<strong>ERROR: Image collection</strong>\n";

    for (int i = 0; i < sListElements.length(); i++){
        sListElements[i] = sListElements[i].trimmed();
        sListElements[i].remove("\"");

        // First entry (height, align)
        if (i == 0){
            sListImages << sListElements[0].split(",");
            for (int j = 0; j < sListImages.length(); j++){
                if (sListImages[j].trimmed().toUInt() != 0)
                    sImageCollHeight = sListImages[j].trimmed();
                else if (sListImages[j].trimmed() == "left" || sListImages[j].trimmed() == "right")
                    sImageCollAlign = sListImages[j].trimmed();
            }
        }
        else{
            sListImages.clear();
            sListImages << sListElements[i].split(",");
            if (sListImages.length() <= 1)
                return "<strong>ERROR: Image collection</strong>\n";
        }

        // Not in Fliesstext
        if (sImageCollAlign == "" && i > 0){

            if (i == 1)
                sOutput += "<div style=\"clear: both\">\n<div class=\"contents\"> </div>\n</div>";

            iImgHeight = QImage(sListImages[0].trimmed()).height();
            iImgWidth = (double)QImage(sListImages[0].trimmed()).width() / (iImgHeight / sImageCollHeight.toDouble());

            sOutput += "<table style=\"float: left; margin: 10px 5px; border: none\">\n<tbody>\n<tr>\n";
            sOutput += "<td style=\"text-align: center; background-color: #E2C889; border: none\">";
            sOutput += "<a href=\"" + sListImages[0].trimmed() + "\" class=\"crosslink\">";
            sOutput += "<img src=\"" + sListImages[0].trimmed() + "\" alt=\"" + sListImages[0].trimmed() + "\" class=\"image-default\" ";
            sOutput += "width=\"" + QString::number((int)iImgWidth) + "\" height=\"" + sImageCollHeight + "\"/></a></td>\n</tr>\n";
            sOutput += "<tr>\n<td style=\"text-align: center; background-color: #F9EAAF; border: none\">" + sListImages[1] + "</td>\n</tr>\n";
            sOutput += "</tbody>\n</table>\n";
        }
    }

    if (sImageCollAlign == "")
        sOutput += "<div style=\"clear: both\">\n<div class=\"contents\"> </div>\n</div>";

    // In Fliesstext
    if (sImageCollAlign != ""){
        sOutput = "<table style=\"float: " + sImageCollAlign + "; clear: both; border: none\">\n<tbody>\n<tr style=\"background-color: #E2C889\">\n";
        for (int i = 1; i < sListElements.length(); i++){
            sListImages.clear();
            sListImages << sListElements[i].split(",");
            if (sListImages.length() <= 1)
                return "<strong>ERROR: Image collection</strong>\n";

            iImgHeight = QImage(sListImages[0].trimmed()).height();
            iImgWidth = (double)QImage(sListImages[0].trimmed()).width() / (iImgHeight / sImageCollHeight.toDouble());

            sOutput += "<td style=\"text-align: center; border-width: 0 10px 0 0; border-color: #FFFFFF \">\n";
            sOutput += "<img src=\"" + sListImages[0] + "\" alt=\"" + sListImages[0] + "\" class=\"image-default\" ";
            sOutput += "width=\"" + QString::number((int)iImgWidth) + "\" height=\"" + sImageCollHeight + "\"/></td>\n";
        }
        sOutput += "</tr>\n<tr style=\"background-color: #F9EAAF\">";

        for (int i = 1; i < sListElements.length(); i++){
            sListImages.clear();
            sListImages << sListElements[i].split(",");

            sOutput += "<td style=\"text-align: center; border-width: 0 10px 0 0; border-color: #FFFFFF\">\n" +
                       sListImages[1].trimmed() + "</td>\n";
        }
        sOutput += "</tr></tbody>\n</table>\n";
    }

    return sOutput;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// REPLACE IMAGES

void CParser::replaceImages(QTextDocument *myRawDoc){


    QRegExp findImages("\\[\\[Bild\\([\\w\\s\\-,./=\"]+\\)\\]\\]");
    QString sMyDoc = myRawDoc->toPlainText();
    int iLength;
    QString sTmpImage;
    QStringList sListTmpImageInfo;

    QString sImageUrl("");
    QString sImageWidth("");
    QString sImageHeight("");
    QString sImageAlign("left");
    //QString sImageAlt("");

    double iImgHeight, iImgWidth;
    double tmpH, tmpW;

    int myindex = findImages.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findImages.matchedLength();
        sTmpImage = findImages.cap();

        sTmpImage.remove("[[Bild(");
        sTmpImage.remove(")]]");

        sImageWidth = "";
        sImageHeight = "";
        sImageAlign = "left";
        iImgHeight = 0;
        iImgWidth = 0;
        tmpH = 0;
        tmpW = 0;

        sListTmpImageInfo.clear();
        sListTmpImageInfo << sTmpImage.split(",");

        sImageUrl = sListTmpImageInfo[0].trimmed();
        for (int i = 1; i < sListTmpImageInfo.length(); i++){
            // Found integer (width)
            if (sListTmpImageInfo[i].trimmed().toUInt() != 0){
                sImageWidth = sListTmpImageInfo[i].trimmed();
                tmpW = sListTmpImageInfo[i].trimmed().toUInt();
            }
            // Found x+integer (height)
            else if (sListTmpImageInfo[i].startsWith("x\\d")){
                sImageHeight = sListTmpImageInfo[i].remove("x").trimmed();
                tmpH = sImageHeight.toUInt();
            }
            // Found int x int (width x height)
            else if (sListTmpImageInfo[i].contains("\\dx\\d")){
                QString sTmp;
                sTmp = sListTmpImageInfo[i];
                sImageWidth = sListTmpImageInfo[i].remove(sListTmpImageInfo[i].indexOf("x"), sListTmpImageInfo[i].length()).trimmed();
                tmpW = sImageWidth.toUInt();
                sImageHeight = sTmp.remove(0, sTmp.indexOf("x")).trimmed();
                tmpH = sImageHeight.toUInt();
            }

            // Found alignment
            else if (sListTmpImageInfo[i].trimmed() == "left" || sListTmpImageInfo[i].trimmed() == "align=left"){
                sImageAlign = "left";
            }
            else if (sListTmpImageInfo[i].trimmed() == "right" || sListTmpImageInfo[i].trimmed() == "align=right"){
                sImageAlign = "right";
            }
        }

        // No size given
        if (tmpH == 0 && tmpW == 0){
            iImgHeight = QImage(sImageUrl).height();
            tmpH = iImgHeight;
            iImgWidth = QImage(sImageUrl).width();
            tmpW = iImgWidth;
        }

        if (tmpH > tmpW){
            iImgHeight = QImage(sImageUrl).height();
            tmpW = (double)QImage(sImageUrl).width() / (iImgHeight / (double)tmpH);
        }
        else if (tmpW > tmpH){
            iImgWidth = QImage(sImageUrl).width();
            tmpH = (double)QImage(sImageUrl).height() / (iImgWidth / (double)tmpW);
        }

        // HTML code
        sTmpImage = "<a href=\"" + sImageUrl + "\" class=\"crosslink\">\n";
        sTmpImage += "<img src=\"" + sImageUrl + "\" alt=\"" + sImageUrl + "\" height=\"" + QString::number(tmpH) + "\" width=\"" + QString::number(tmpW) + "\" ";
        sTmpImage += "class=\"image-" + sImageAlign + "\" /></a>";

        // Replace
        sMyDoc.replace(myindex, iLength, sTmpImage);

        // Go on with RegExp-Search
        myindex = findImages.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace myRawDoc with document with HTML links
    myRawDoc->setPlainText(sMyDoc);

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// REPLACE ANCHOR

void CParser::replaceAnchor(QTextDocument *myRawDoc){


    QRegExp findAnchor("\\[\\[Anker\\([A-Za-z_\\s-]+\\)\\]\\]");
    QString sMyDoc = myRawDoc->toPlainText();
    int iLength;
    QString sTmpAnchor, sOutput;

    int myindex = findAnchor.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findAnchor.matchedLength();
        sTmpAnchor = findAnchor.cap();

        sTmpAnchor.remove("[[Anker(");
        sTmpAnchor.remove(")]]");
        sTmpAnchor = sTmpAnchor.trimmed();

        // Replace characters for valid links (ä, ü, ö, spaces)
        sTmpAnchor.replace(" ", "-");
        sTmpAnchor.replace(QString::fromUtf8("Ä"), "Ae");
        sTmpAnchor.replace(QString::fromUtf8("Ü"), "Ue");
        sTmpAnchor.replace(QString::fromUtf8("Ö"), "Oe");
        sTmpAnchor.replace(QString::fromUtf8("ä"), "ae");
        sTmpAnchor.replace(QString::fromUtf8("ü"), "ue");
        sTmpAnchor.replace(QString::fromUtf8("ö"), "oe");

        sOutput = "<a id=\"" + sTmpAnchor + "\" href=\"#" + sTmpAnchor + "\" class=\"crosslink anchor\"> </a>";

        // Replace
        sMyDoc.replace(myindex, iLength, sOutput);

        // Go on with RegExp-Search
        myindex = findAnchor.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace myRawDoc with document with HTML links
    myRawDoc->setPlainText(sMyDoc);

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// List
QString CParser::parseList(QString actParagraph){
    QString sParagraph = actParagraph;
    QString sOutput("<strong>ERROR: List</strong>\n");

    // Separate elementes from macro (between §)
    QStringList sListElements = sParagraph.split("§");
    int iArrayLevel[sListElements.length()];
    bool iArrayArabic[sListElements.length()];

    for (int i = 0; i < sListElements.length(); i++){

        if (sListElements[i].trimmed().startsWith("*")){
            if (i == 0)
                sOutput = "<ul>\n";

            iArrayLevel[i] = sListElements[i].indexOf("*");
            iArrayArabic[i] = false;
            sListElements[i] = sListElements[i].remove(0, iArrayLevel[i]+1).trimmed();
        }
        else {
            if (i == 0)
                sOutput = "<ol class=\"arabic\">\n";

            iArrayLevel[i] = sListElements[i].indexOf("1.");
            iArrayArabic[i] = true;
            sListElements[i] = sListElements[i].remove(0, iArrayLevel[i]+2).trimmed();
        }

        if (i > 0 && (iArrayLevel[i-1] < iArrayLevel[i])){
            for (int j = iArrayLevel[i] - iArrayLevel[i-1]; j > 0; j--){
                if (iArrayArabic[i] == false)
                    sOutput += "<ul>\n";
                else
                    sOutput += "<ol class=\"arabic\">\n";
            }
        }
        if (i > 0 && (iArrayLevel[i-1] > iArrayLevel[i])){
            for (int j = iArrayLevel[i-1] - iArrayLevel[i]; j > 0; j--){
                if (iArrayArabic[i] == false)
                    sOutput += "</ul>\n";
                else
                    sOutput += "</ol>\n";
            }
        }

        sOutput += "<li><p>" + sListElements[i] + "</p></li>\n";

        if (i == sListElements.length()-1){
            for (int k = iArrayLevel[i]; k > 0; k--)
                if (iArrayArabic[i] == false)
                    sOutput += "</ul>\n";
                else
                    sOutput += "</ol>\n";
        }
    }

    sOutput += "</ul>";

    return sOutput;
}
