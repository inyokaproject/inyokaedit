/**
 * \file parsetable.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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
 * Parse tables.
 */

#include "./parsetable.h"

#include <QRegExp>
#include <QStringList>
#include <QTextBlock>

ParseTable::ParseTable() {
}

void ParseTable::startParsing(QTextDocument *pRawDoc) {
  QString sDoc("");
  QString sLine("");
  QStringList sListLines;
  bool bTable = false;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    // New cell or still in table with unfinished line
    if (block.text().trimmed().startsWith("||") || bTable) {
      bTable = true;
      sLine += block.text();

      // Line completed
      if (block.text().trimmed().endsWith("||")) {
        sListLines << sLine.trimmed();
        sLine.clear();

        // Table finished
        if (!(block.next().text().trimmed().startsWith("||"))) {
          sDoc += createTable(sListLines);
          sListLines.clear();
          sLine.clear();
          bTable = false;
        }
      }
    } else {  // Everything else
      sDoc += block.text() + "\n";
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------

QString ParseTable::createTable(const QStringList &sListLines) {
  QString sRet("");
  QStringList sListCells;
  QString sLine("");
  QString sCell("");
  QString sFormating("");
  QString sTmpStyle("");

  QRegExp formatPattern("\\<{1,1}.+\\>{1,1}");
  QRegExp tableStylePattern("tablestyle=\\\"[\\w\\s:;%#-=]+\\\"");
  QRegExp rowClassPattern("rowclass=\\\"[\\w.%-]+\\\"");
  QRegExp rowStylePattern("rowstyle=\\\"[\\w\\s:;%#-=]+\\\"");
  QRegExp cellClassPattern("cellclass=\\\"[\\w.%-]+\\\"");
  QRegExp cellStylePattern("cellstyle=\\\"[\\w\\s:;%#-=]+\\\"");
  bool bCellStyle = false;

  QRegExp connectCells("-\\d{1,2}");
  QRegExp connectRows("\\|\\d{1,2}");

  for (int nLine = 0; nLine < sListLines.size(); nLine++) {
    sLine = sListLines[nLine];
    sLine.remove(0, 2);  // Remove || at beginning and end
    sLine.remove(sLine.length() - 2, 2);

    sListCells = sLine.split("||");
    for (int nCell = 0; nCell < sListCells.size(); nCell++) {
      sCell = sListCells[nCell];
      bCellStyle = false;

      // Look for formating
      if (formatPattern.indexIn(sCell) >= 0) {
        sFormating = formatPattern.cap();
        sCell.remove(sFormating);
      } else {
        sFormating.clear();
      }

      if (0 == nCell) {
        if (0 == nLine) {
          if (tableStylePattern.indexIn(sFormating) >= 0) {
            sTmpStyle = tableStylePattern.cap();
            sRet = "<table style=" +sTmpStyle.remove("tablestyle=")
                   + ">\n<tbody>\n";
          } else {
            sRet = "<table>\n<tbody>\n";
          }
        }

        // New row
        sRet += "<tr";  // Start tr
        // Found row class info --> in tr
        if (rowClassPattern.indexIn(sFormating) >= 0) {
          sTmpStyle = rowClassPattern.cap();
          sRet += " class="
                  + sTmpStyle.remove("rowclass=");
        }
        // Found row sytle info --> in tr
        if (rowStylePattern.indexIn(sFormating) >= 0) {
          sTmpStyle = rowStylePattern.cap();
          sRet += " style=\""
                  + sTmpStyle.remove("rowstyle=")
                  .remove("\"") + "\"";
        }
        sRet += ">\n";  // Close tr
      }

      // New cell
      sRet += "<td";  // Start td

      // Found cell class info --> in td
      if (cellClassPattern.indexIn(sFormating) >= 0) {
        sTmpStyle = cellClassPattern.cap();
        sRet += " class="
                + sTmpStyle.remove("cellclass=");
      }

      // Connect cells info (-integer, e.g. -3)
      if (connectCells.indexIn(sFormating) >= 0) {
        sRet += " colspan=\""
                + connectCells.cap().remove("-") + "\"";
      }

      // Connect ROWS info (|integer, e.g. |2)
      if (connectRows.indexIn(sFormating) >= 0) {
        sRet += " rowspan=\""
                + connectRows.cap().remove("|") + "\"";
      }

      // Found cell sytle info --> in td
      if (cellStylePattern.indexIn(sFormating) >= 0) {
        sTmpStyle = cellStylePattern.cap();
        sRet += " style=\""
                + sTmpStyle.remove("cellstyle=")
                .remove("\"");
        bCellStyle = true;
      }
      // Text align center
      if (sFormating.contains("<:")
          || sFormating.contains(" : ")
          || sFormating.contains(":>")) {
        if (bCellStyle) {
          sRet += " text-align: center;";
        } else {
          sRet += " style=\"text-align: center;";
          bCellStyle = true;
        }
      }
      // Text align left
      if (sFormating.contains("<(")
          || sFormating.contains("(")
          || sFormating.contains("(>")) {
        if (bCellStyle) {
          sRet += " text-align: left;";
        } else {
          sRet += " style=\"text-align: left;";
          bCellStyle = true;
        }
      }
      // Text align center
      if (sFormating.contains("<)")
          || sFormating.contains(" ) ")
          || sFormating.contains(")>")) {
        if (bCellStyle) {
          sRet += " text-align: right;";
        } else {
          sRet += " style=\"text-align: right;";
          bCellStyle = true;
        }
      }
      // Text vertical align top
      if (sFormating.contains("<^")
          || sFormating.contains(" ^ ")
          || sFormating.contains("^>")) {
        if (bCellStyle) {
          sRet += " text-align: top;";
        } else {
          sRet += " style=\"vertical-align: top;";
          bCellStyle = true;
        }
      }
      // Text vertical align bottom
      if (sFormating.contains("<v")
          || sFormating.contains(" v ")
          || sFormating.contains("v>")) {
        if (bCellStyle) {
          sRet += " text-align: bottom;";
        } else {
          sRet += " style=\"vertical-align: bottom;";
          bCellStyle = true;
        }
      }
      // Closing style section
      if (bCellStyle) {
        sRet += "\"";
      }

      sRet += ">\n";  // Close td
      sRet += sCell + "</td>\n";
    }

    sRet += "</tr>\n";
  }
  sRet += "</tbody>\n</table>\n\n";
  return sRet;
}
