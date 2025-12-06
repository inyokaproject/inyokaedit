// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./parselist.h"

#include <QTextBlock>
#include <QTextDocument>

ParseList::ParseList() = default;

void ParseList::startParsing(QTextDocument *pRawDoc) {
  QString sDoc(QLatin1String(""));
  QString sLine;
  QString sClass(QStringLiteral("arabic"));
  int nPreviousIndex;
  int nCurrentIndex = -1;
  QList<bool> bArrayListType;  // Unsorted = false, sorted = true

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (block.text().trimmed().startsWith(QLatin1String("*")) ||
        block.text().trimmed().startsWith(QLatin1String("1.")) ||
        block.text().trimmed().startsWith(QLatin1String("a.")) ||
        block.text().trimmed().startsWith(QLatin1String("A.")) ||
        block.text().trimmed().startsWith(QLatin1String("i.")) ||
        block.text().trimmed().startsWith(QLatin1String("I."))) {
      sLine = block.text();

      if (sLine.indexOf(QLatin1String(" * ")) >= 0) {  // Unsorted list
        nPreviousIndex = nCurrentIndex;
        nCurrentIndex = sLine.indexOf(QLatin1String(" * "));
        sLine.remove(0, sLine.indexOf(QLatin1String(" * ")) + 3);
        // qDebug() << "LIST:" << sLine << nCurrentIndex << false;

        if (nCurrentIndex != nPreviousIndex) {
          if (nCurrentIndex > nPreviousIndex) {  // New tag
            sDoc += QLatin1String("<ul>\n");
            bArrayListType << false;
          } else {  // Close previous tag and maybe create new
            if (!bArrayListType.isEmpty()) {
              if (!bArrayListType.last()) {
                sDoc += QLatin1String("</ul>\n");
              } else {
                sDoc += QLatin1String("</ol>\n");
              }
              bArrayListType.removeLast();
            }

            if (!bArrayListType.isEmpty()) {
              if (bArrayListType.last()) {
                sDoc += QLatin1String("</ol>\n<ul>\n");
                bArrayListType.removeLast();
                bArrayListType << false;
              }
            }
          }
        }
        sDoc += "<li>" + sLine + "</li>\n";

      } else if (sLine.indexOf(QLatin1String(" 1. ")) >= 0 ||
                 sLine.indexOf(QLatin1String(" a. ")) >= 0 ||
                 sLine.indexOf(QLatin1String(" A. ")) >= 0 ||
                 sLine.indexOf(QLatin1String(" i. ")) >= 0 ||
                 sLine.indexOf(QLatin1String(" I. ")) >= 0) {  // Sorted list
        nPreviousIndex = nCurrentIndex;

        nCurrentIndex = sLine.indexOf(QLatin1String(" 1. "));
        if (nCurrentIndex >= 0) {
          sClass = QStringLiteral("arabic");
        } else {
          nCurrentIndex = sLine.indexOf(QLatin1String(" a. "));
          if (nCurrentIndex >= 0) {
            sClass = QStringLiteral("alphalower");
          } else {
            nCurrentIndex = sLine.indexOf(QLatin1String(" A. "));
            if (nCurrentIndex >= 0) {
              sClass = QStringLiteral("alphaupper");
            } else {
              nCurrentIndex = sLine.indexOf(QLatin1String(" i. "));
              if (nCurrentIndex >= 0) {
                sClass = QStringLiteral("romanlower");
              } else {
                nCurrentIndex = sLine.indexOf(QLatin1String(" I. "));
                if (nCurrentIndex >= 0) {
                  sClass = QStringLiteral("romanupper");
                }
              }
            }
          }
        }

        sLine.remove(0, nCurrentIndex + 4);
        // qDebug() << "LIST:" << sLine << nCurrentIndex << true;

        if (nCurrentIndex != nPreviousIndex) {
          if (nCurrentIndex > nPreviousIndex) {  // New tag
            sDoc += "<ol class=\"" + sClass + "\">\n";
            bArrayListType << true;
          } else {  // Close previous tag and maybe create new
            if (!bArrayListType.isEmpty()) {
              if (!bArrayListType.last()) {
                sDoc += QLatin1String("</ul>\n");
              } else {
                sDoc += QLatin1String("</ol>\n");
              }
              bArrayListType.removeLast();
            }

            if (!bArrayListType.isEmpty()) {
              if (!bArrayListType.last()) {
                sDoc += "</ul>\n<ol class=\"" + sClass + "\">\n";
                bArrayListType.removeLast();
                bArrayListType << true;
              }
            }
          }
        }
        sDoc += "<li>" + sLine + "</li>\n";

      } else {  // Not a list element
        // Close all open tags
        while (!bArrayListType.isEmpty()) {
          if (!bArrayListType.last()) {
            sDoc += QLatin1String("</ul>\n");
          } else {
            sDoc += QLatin1String("</ol>\n");
          }
          bArrayListType.removeLast();
        }
        nCurrentIndex = -1;
        sDoc += block.text() + "\n";
        // qDebug() << "LIST END";
      }

    } else {  // Everything else
      // Close all open tags
      while (!bArrayListType.isEmpty()) {
        if (!bArrayListType.last()) {
          sDoc += QLatin1String("</ul>\n");
        } else {
          sDoc += QLatin1String("</ol>\n");
        }
        bArrayListType.removeLast();
      }
      nCurrentIndex = -1;
      sDoc += block.text() + "\n";
      // qDebug() << "LIST END";
    }
  }

  pRawDoc->setPlainText(sDoc);
}
