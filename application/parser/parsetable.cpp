// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./parsetable.h"

#include <QRegularExpression>
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
  QRegularExpressionMatch match;

  static QRegularExpression formatPattern(QStringLiteral("\\<{1,1}.+\\>{1,1}"));
  static QRegularExpression tableClassPattern(
      QStringLiteral("tableclass=\\\"[\\w\\s:;%#\\-=]+\\\""));
  static QRegularExpression tableStylePattern(
      QStringLiteral("tablestyle=\\\"[\\w\\s:;%#\\-=]+\\\""));
  static QRegularExpression rowClassPattern(
      QStringLiteral("rowclass=\\\"[\\w.%\\-]+\\\""));
  static QRegularExpression rowStylePattern(
      QStringLiteral("rowstyle=\\\"[\\w\\s:;%#\\-=]+\\\""));
  static QRegularExpression cellClassPattern(
      QStringLiteral("cellclass=\\\"[\\w.%\\-]+\\\""));
  static QRegularExpression cellStylePattern(
      QStringLiteral("cellstyle=\\\"[\\w\\s:;%#\\-=]+\\\""));
  bool bCellStyle;

  static QRegularExpression connectCells(QStringLiteral("-\\d{1,2}"));
  static QRegularExpression connectRows(QStringLiteral("\\|\\d{1,2}"));

  for (int nLine = 0; nLine < sListLines.size(); nLine++) {
    sLine = sListLines[nLine];
    sLine.remove(0, 2);  // Remove || at beginning and end
    sLine.remove(sLine.length() - 2, 2);

    sListCells = sLine.split(QStringLiteral("||"));
    for (int nCell = 0; nCell < sListCells.size(); nCell++) {
      sCell = sListCells[nCell];
      bCellStyle = false;

      // Look for formatting
      if ((match = formatPattern.match(sCell)).hasMatch()) {
        sFormating = match.captured();
        sCell.remove(sFormating);
      } else {
        sFormating.clear();
      }

      if (0 == nCell) {
        if (0 == nLine) {
          QString sTmpClass(QLatin1String(""));
          if ((match = tableClassPattern.match(sFormating)).hasMatch()) {
            sTmpClass = match.captured();
            sTmpClass =
                " class=" + sTmpClass.remove(QStringLiteral("tableclass="));
          }
          sTmpStyle.clear();
          if ((match = tableStylePattern.match(sFormating)).hasMatch()) {
            sTmpStyle = match.captured();
            sTmpStyle =
                " style=" + sTmpStyle.remove(QStringLiteral("tablestyle="));
          }
          sRet = "<table" + sTmpClass + sTmpStyle + ">\n<tbody>\n";
        }

        // New row
        sRet += QLatin1String("<tr");  // Start tr
        // Found row class info --> in tr
        if ((match = rowClassPattern.match(sFormating)).hasMatch()) {
          sTmpStyle = match.captured();
          sRet += " class=" + sTmpStyle.remove(QStringLiteral("rowclass="));
        }
        // Found row style info --> in tr
        if ((match = rowStylePattern.match(sFormating)).hasMatch()) {
          sTmpStyle = match.captured();
          sRet += " style=\"" +
                  sTmpStyle.remove(QStringLiteral("rowstyle="))
                      .remove(QStringLiteral("\"")) +
                  "\"";
        }
        sRet += QLatin1String(">\n");  // Close tr
      }

      // New cell
      sRet += QLatin1String("<td");  // Start td

      // Found cell class info --> in td
      if ((match = cellClassPattern.match(sFormating)).hasMatch()) {
        sTmpStyle = match.captured();
        sRet += " class=" + sTmpStyle.remove(QStringLiteral("cellclass="));
      }

      // Connect cells info (-integer, e.g. -3)
      if ((match = connectCells.match(sFormating)).hasMatch()) {
        sRet +=
            " colspan=\"" + match.captured().remove(QStringLiteral("-")) + "\"";
      }

      // Connect ROWS info (|integer, e.g. |2)
      if ((match = connectRows.match(sFormating)).hasMatch()) {
        sRet +=
            " rowspan=\"" + match.captured().remove(QStringLiteral("|")) + "\"";
      }

      // Found cell style info --> in td
      if ((match = cellStylePattern.match(sFormating)).hasMatch()) {
        sTmpStyle = match.captured();
        sRet += " style=\"" + sTmpStyle.remove(QStringLiteral("cellstyle="))
                                  .remove(QStringLiteral("\""));
        bCellStyle = true;
      }
      // Text align center
      if (sFormating.contains(QLatin1String("<:")) ||
          sFormating.contains(QLatin1String(" : ")) ||
          sFormating.contains(QLatin1String(":>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: center;");
        } else {
          sRet += QLatin1String(" style=\"text-align: center;");
          bCellStyle = true;
        }
      }
      // Text align left
      if (sFormating.contains(QLatin1String("<(")) ||
          sFormating.contains(QLatin1String("(")) ||
          sFormating.contains(QLatin1String("(>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: left;");
        } else {
          sRet += QLatin1String(" style=\"text-align: left;");
          bCellStyle = true;
        }
      }
      // Text align center
      if (sFormating.contains(QLatin1String("<)")) ||
          sFormating.contains(QLatin1String(" ) ")) ||
          sFormating.contains(QLatin1String(")>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: right;");
        } else {
          sRet += QLatin1String(" style=\"text-align: right;");
          bCellStyle = true;
        }
      }
      // Text vertical align top
      if (sFormating.contains(QLatin1String("<^")) ||
          sFormating.contains(QLatin1String(" ^ ")) ||
          sFormating.contains(QLatin1String("^>"))) {
        if (bCellStyle) {
          sRet += QLatin1String(" text-align: top;");
        } else {
          sRet += QLatin1String(" style=\"vertical-align: top;");
          bCellStyle = true;
        }
      }
      // Text vertical align bottom
      if (sFormating.contains(QLatin1String("<v")) ||
          sFormating.contains(QLatin1String(" v ")) ||
          sFormating.contains(QLatin1String("v>"))) {
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
