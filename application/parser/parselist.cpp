/**
 * \file parselist.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
 *
 * This file is part of InyokaEdit.
 *
 * InyokaEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * InyokaEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Parse sorted/unsorted lists.
 */

#include "./parselist.h"

#include <QTextBlock>

ParseList::ParseList() = default;

void ParseList::startParsing(QTextDocument *pRawDoc) {
  QString sDoc("");
  QString sLine("");
  QString sClass("arabic");
  int nPreviousIndex = -1;
  int nCurrentIndex = -1;
  QList<bool> bArrayListType;  // Unsorted = false, sorted = true

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (block.text().trimmed().startsWith("*") ||
        block.text().trimmed().startsWith("1.") ||
        block.text().trimmed().startsWith("a.") ||
        block.text().trimmed().startsWith("A.") ||
        block.text().trimmed().startsWith("i.") ||
        block.text().trimmed().startsWith("I.")) {
      sLine = block.text();

      if (sLine.indexOf(" * ") >= 0) {  // Unsorted list
        nPreviousIndex = nCurrentIndex;
        nCurrentIndex = sLine.indexOf(" * ");
        sLine.remove(0, sLine.indexOf(" * ") + 3);
        // qDebug() << "LIST:" << sLine << nCurrentIndex << false;

        if (nCurrentIndex != nPreviousIndex) {
          if (nCurrentIndex > nPreviousIndex) {  // New tag
            sDoc += "<ul>\n";
            bArrayListType << false;
          } else {  // Close previous tag and maybe create new
            if (!bArrayListType.isEmpty()) {
              if (!bArrayListType.last()) {
                sDoc += "</ul>\n";
              } else {
                sDoc += "</ol>\n";
              }
              bArrayListType.removeLast();
            }

            if (!bArrayListType.isEmpty()) {
              if (bArrayListType.last()) {
                sDoc += "</ol>\n<ul>\n";
                bArrayListType.removeLast();
                bArrayListType << false;
              }
            }
          }
        }
        sDoc += "<li>" + sLine + "</li>\n";

      } else if (sLine.indexOf(" 1. ") >= 0 ||
                 sLine.indexOf(" a. ") >= 0 ||
                 sLine.indexOf(" A. ") >= 0 ||
                 sLine.indexOf(" i. ") >= 0 ||
                 sLine.indexOf(" I. ") >= 0) {  // Sorted list
        nPreviousIndex = nCurrentIndex;

        nCurrentIndex = sLine.indexOf(" 1. ");
        if (nCurrentIndex >= 0) {
          sClass = "arabic";
        } else {
          nCurrentIndex = sLine.indexOf(" a. ");
          if (nCurrentIndex >= 0) {
            sClass = "alphalower";
          } else {
            nCurrentIndex = sLine.indexOf(" A. ");
            if (nCurrentIndex >= 0) {
              sClass = "alphaupper";
            } else {
              nCurrentIndex = sLine.indexOf(" i. ");
              if (nCurrentIndex >= 0) {
                sClass = "romanlower";
              } else {
                nCurrentIndex = sLine.indexOf(" I. ");
                if (nCurrentIndex >= 0) {
                  sClass = "romanupper";
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
                sDoc += "</ul>\n";
              } else {
                sDoc += "</ol>\n";
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
            sDoc += "</ul>\n";
          } else {
            sDoc += "</ol>\n";
          }
          bArrayListType.removeLast();
        }
        nPreviousIndex = -1;
        nCurrentIndex = -1;
        sDoc += block.text() + "\n";
        // qDebug() << "LIST END";
      }

    } else {  // Everything else
      // Close all open tags
      while (!bArrayListType.isEmpty()) {
        if (!bArrayListType.last()) {
          sDoc += "</ul>\n";
        } else {
          sDoc += "</ol>\n";
        }
        bArrayListType.removeLast();
      }
      nPreviousIndex = -1;
      nCurrentIndex = -1;
      sDoc += block.text() + "\n";
      // qDebug() << "LIST END";
    }
  }

  pRawDoc->setPlainText(sDoc);
}
