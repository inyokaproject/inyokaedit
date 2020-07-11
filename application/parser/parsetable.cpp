/**
 * \file parsetable.cpp
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Parse tables.
 */

#include "./parsetable.h"

#include <QRegExp>
#include <QStringList>
#include <QTextBlock>
#include <QTextDocument>

ParseTable::ParseTable() = default;

void ParseTable::startParsing(QTextDocument *pRawDoc) {
  QString sDoc(QLatin1String(""));
  QString sLine(QLatin1String(""));
  QStringList sListLines;
  bool bTable = false;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    // New cell or still in table with unfinished line
    if (block.text().trimmed().startsWith(QLatin1String("||")) || bTable) {
      bTable = true;
      sLine += block.text();

      // Line completed
      if (block.text().trimmed().endsWith(QLatin1String("||"))) {
        sListLines << sLine.trimmed();
        sLine.clear();

        // Table finished
        if (!(block.next().text().trimmed().startsWith(QLatin1String("||")))) {
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

auto ParseTable::createTable(const QStringList &sListLines) -> QString {
  QString sRet(QLatin1String(""));
  QStringList sListCells;
  QString sLine;
  QString sCell;
  QString sFormating(QLatin1String(""));
  QString sTmpStyle(QLatin1String(""));

  QRegExp formatPattern("\\<{1,1}.+\\>{1,1}");
  QRegExp tableClassPattern("tableclass=\\\"[\\w\\s:;%#-=]+\\\"");
  QRegExp tableStylePattern("tablestyle=\\\"[\\w\\s:;%#-=]+\\\"");
  QRegExp rowClassPattern("rowclass=\\\"[\\w.%-]+\\\"");
  QRegExp rowStylePattern("rowstyle=\\\"[\\w\\s:;%#-=]+\\\"");
  QRegExp cellClassPattern("cellclass=\\\"[\\w.%-]+\\\"");
  QRegExp cellStylePattern("cellstyle=\\\"[\\w\\s:;%#-=]+\\\"");
  bool bCellStyle;

  QRegExp connectCells("-\\d{1,2}");
  QRegExp connectRows("\\|\\d{1,2}");

  for (int nLine = 0; nLine < sListLines.size(); nLine++) {
    sLine = sListLines[nLine];
    sLine.remove(0, 2);  // Remove || at beginning and end
    sLine.remove(sLine.length() - 2, 2);

    sListCells = sLine.split(QStringLiteral("||"));
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
          QString sTmpClass(QLatin1String(""));
          if (tableClassPattern.indexIn(sFormating) >= 0) {
            sTmpClass = tableClassPattern.cap();
            sTmpClass = " class=" + sTmpClass.remove(
                          QStringLiteral("tableclass="));
          }
          sTmpStyle.clear();
          if (tableStylePattern.indexIn(sFormating) >= 0) {
            sTmpStyle = tableStylePattern.cap();
            sTmpStyle = " style=" + sTmpStyle.remove(
                          QStringLiteral("tablestyle="));
          }
          sRet = "<table" + sTmpClass + sTmpStyle + ">\n<tbody>\n";
        }

        // New row
        sRet += QLatin1String("<tr");  // Start tr
        // Found row class info --> in tr
        if (rowClassPattern.indexIn(sFormating) >= 0) {
          sTmpStyle = rowClassPattern.cap();
          sRet += " class="
                  + sTmpStyle.remove(QStringLiteral("rowclass="));
        }
        // Found row sytle info --> in tr
        if (rowStylePattern.indexIn(sFormating) >= 0) {
          sTmpStyle = rowStylePattern.cap();
          sRet += " style=\""
                  + sTmpStyle.remove(QStringLiteral("rowstyle="))
                  .remove(QStringLiteral("\"")) + "\"";
        }
        sRet += QLatin1String(">\n");  // Close tr
      }

      // New cell
      sRet += QLatin1String("<td");  // Start td

      // Found cell class info --> in td
      if (cellClassPattern.indexIn(sFormating) >= 0) {
        sTmpStyle = cellClassPattern.cap();
        sRet += " class="
                + sTmpStyle.remove(QStringLiteral("cellclass="));
      }

      // Connect cells info (-integer, e.g. -3)
      if (connectCells.indexIn(sFormating) >= 0) {
        sRet += " colspan=\""
                + connectCells.cap().remove(QStringLiteral("-")) + "\"";
      }

      // Connect ROWS info (|integer, e.g. |2)
      if (connectRows.indexIn(sFormating) >= 0) {
        sRet += " rowspan=\""
                + connectRows.cap().remove(QStringLiteral("|")) + "\"";
      }

      // Found cell sytle info --> in td
      if (cellStylePattern.indexIn(sFormating) >= 0) {
        sTmpStyle = cellStylePattern.cap();
        sRet += " style=\""
                + sTmpStyle.remove(QStringLiteral("cellstyle="))
                .remove(QStringLiteral("\""));
        bCellStyle = true;
      }
      // Text align center
      if (sFormating.contains(QLatin1String("<:"))
          || sFormating.contains(QLatin1String(" : "))
          || sFormating.contains(QLatin1String(":>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: center;");
        } else {
          sRet += QLatin1String(" style=\"text-align: center;");
          bCellStyle = true;
        }
      }
      // Text align left
      if (sFormating.contains(QLatin1String("<("))
          || sFormating.contains(QLatin1String("("))
          || sFormating.contains(QLatin1String("(>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: left;");
        } else {
          sRet += QLatin1String(" style=\"text-align: left;");
          bCellStyle = true;
        }
      }
      // Text align center
      if (sFormating.contains(QLatin1String("<)"))
          || sFormating.contains(QLatin1String(" ) "))
          || sFormating.contains(QLatin1String(")>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: right;");
        } else {
          sRet += QLatin1String(" style=\"text-align: right;");
          bCellStyle = true;
        }
      }
      // Text vertical align top
      if (sFormating.contains(QLatin1String("<^"))
          || sFormating.contains(QLatin1String(" ^ "))
          || sFormating.contains(QLatin1String("^>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: top;");
        } else {
          sRet += QLatin1String(" style=\"vertical-align: top;");
          bCellStyle = true;
        }
      }
      // Text vertical align bottom
      if (sFormating.contains(QLatin1String("<v"))
          || sFormating.contains(QLatin1String(" v "))
          || sFormating.contains(QLatin1String("v>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: bottom;");
        } else {
          sRet += QLatin1String(" style=\"vertical-align: bottom;");
          bCellStyle = true;
        }
      }
      // Closing style section
      if (bCellStyle) {
        sRet += QLatin1String("\"");
      }

      sRet += QLatin1String(">\n");  // Close td
      sRet += sCell + "</td>\n";
    }

    sRet += QLatin1String("</tr>\n");
  }
  sRet += QLatin1String("</tbody>\n</table>\n\n");
  return sRet;
}
