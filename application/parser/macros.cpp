/**
 * \file macros.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * Handle build-in macros.
 */

#include "./macros.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextDocument>

Macros::Macros(const QString &sSharePath, const QDir &tmpImgDir)
    : m_sSharePath(sSharePath), m_tmpImgDir(tmpImgDir) {
  QFile fiMacros(QStringLiteral(":/macros.conf"));
  if (!fiMacros.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open macros.conf";
    QMessageBox::warning(nullptr, QStringLiteral("Error"),
                         QStringLiteral("Could not open macros.conf"));
  } else {
    QTextStream in(&fiMacros);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Since Qt 6 UTF-8 is used by default
    in.setCodec("UTF-8");
#endif
    QString tmpLine;
    QStringList tmpList;
    MACRO tmpMacro;

    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith(QLatin1String("Code")) &&
          !tmpLine.trimmed().isEmpty()) {
        tmpList = tmpLine.split(QStringLiteral("="));
        if (2 == tmpList.size()) {
          tmpMacro.name = tmpList[0].trimmed();
          tmpMacro.translations.clear();
          const QStringList tmpList2(tmpList[1].split(QStringLiteral(",")));
          if ("Template" != tmpMacro.name) {
            for (const auto &s : tmpList2) {
              tmpMacro.translations << s.trimmed();
            }
            m_listMacros << tmpMacro;
          } else {
            for (const auto &s : tmpList2) {
              m_sListTplTranslations << s.trimmed();
            }
          }
        }
      }
    }
    fiMacros.close();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::startParsing(QTextDocument *pRawDoc, const QString &sCurrentFile,
                          const QString &sCommunity,
                          QStringList &sListHeadlines) {
  for (const auto &macro : qAsConst(m_listMacros)) {
    for (const auto &s : macro.translations) {
      if ("Anchor" == macro.name) {
        Macros::replaceAnchors(pRawDoc, s);
      } else if ("Attachment" == macro.name) {
        Macros::replaceAttachments(pRawDoc, s);
      } else if ("Date" == macro.name) {
        Macros::replaceDates(pRawDoc, s);
      } else if ("Newline" == macro.name) {
        Macros::replaceNewline(pRawDoc, s);
      } else if ("Picture" == macro.name) {
        this->replacePictures(pRawDoc, s, sCurrentFile, sCommunity);
      } else if ("TableOfContents" == macro.name) {
        Macros::replaceTableOfContents(pRawDoc, s, sListHeadlines);
      } else if ("Span" == macro.name) {
        Macros::replaceSpan(pRawDoc, s);
      } else {
        qWarning() << "Unknown macro:" << macro.name;
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Macros::getTplTranslations() const -> QStringList {
  return m_sListTplTranslations;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replaceAnchors(QTextDocument *pRawDoc, const QString &sTrans) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression regex("\\[{2,2}\\b(" + sTrans +
                           ")\\([A-Za-z_\\s\\-0-9]+\\)\\]{2,2}");
  int nIndex = 0;
  QRegularExpressionMatch match;
  QString sAnchor;

  while ((match = regex.match(sDoc, nIndex)).hasMatch()) {
    sAnchor = match.captured();
    nIndex = match.capturedStart();
    // qDebug() << sAnchor;

    sAnchor.remove("[[" + sTrans + "(");
    sAnchor.remove(QStringLiteral(")]]"));
    sAnchor = sAnchor.trimmed();

    // Replace characters for valid links (ä, ü, ö, spaces)
    sAnchor.replace(QLatin1String(" "), QLatin1String("-"));
    sAnchor.replace(QStringLiteral("Ä"), QLatin1String("Ae"));
    sAnchor.replace(QStringLiteral("Ü"), QLatin1String("Ue"));
    sAnchor.replace(QStringLiteral("Ö"), QLatin1String("Oe"));
    sAnchor.replace(QStringLiteral("ä"), QLatin1String("ae"));
    sAnchor.replace(QStringLiteral("ü"), QLatin1String("ue"));
    sAnchor.replace(QStringLiteral("ö"), QLatin1String("oe"));

    sAnchor = "<a id=\"" + sAnchor + "\" class=\"crosslink anchor\" href=\"#" +
              sAnchor + "\">&#9875;</a>";
    sDoc.replace(nIndex, match.capturedLength(), sAnchor);
    // Go on with RegExp-Search
    nIndex += sAnchor.length();
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replaceAttachments(QTextDocument *pRawDoc, const QString &sTrans) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findMacro(
      "\\[\\[" + sTrans + "\\(.*\\)\\]\\]",
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
  int nIndex = 0;
  QRegularExpressionMatch match;

  while ((match = findMacro.match(sDoc, nIndex)).hasMatch()) {
    QString sMacro = match.captured(0);
    nIndex = match.capturedStart();
    sMacro.remove("[[" + sTrans + "(", Qt::CaseInsensitive);
    sMacro.remove(QStringLiteral(")]]"));
    sMacro.remove('"');

    sMacro =
        "<a href=\"" + sMacro + "\" class=\"crosslink\">" + sMacro + "</a>";

    sDoc.replace(nIndex, match.capturedLength(), sMacro);
    // Go on with new start position
    nIndex += sMacro.length();
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replaceDates(QTextDocument *pRawDoc, const QString &sTrans) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findMacro(
      "\\[\\[" + sTrans + "\\(.*\\)\\]\\]",
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
  int nIndex = 0;
  QRegularExpressionMatch match;
  QDateTime datetime;
  bool bConversionOk;

  while ((match = findMacro.match(sDoc, nIndex)).hasMatch()) {
    QString sMacro = match.captured(0);
    nIndex = match.capturedStart();
    sMacro.remove("[[" + sTrans + "(", Qt::CaseInsensitive);
    sMacro.remove(QStringLiteral(")]]"));

    // First assume ISO 8601 datetime
    datetime = QDateTime::fromString(sMacro, Qt::ISODate);
    bConversionOk = true;
    // Otherwise handle input as unix timestamp
    if (!datetime.isValid()) {
      datetime.setSecsSinceEpoch(sMacro.toUInt(&bConversionOk));
    }

    if (bConversionOk && datetime.isValid()) {
      sMacro = QLocale::system().toString(datetime, QLocale::ShortFormat);
    } else {
      sMacro = QStringLiteral("Invalid date");
    }

    sDoc.replace(nIndex, match.capturedLength(), sMacro);
    // Go on with new start position
    nIndex += sMacro.length();
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replaceNewline(QTextDocument *pRawDoc, const QString &sTrans) {
  QString sDoc(pRawDoc->toPlainText());
  sDoc.replace("[[" + sTrans + "]]", QLatin1String("<br />"));
  sDoc.replace(QLatin1String("\\\\"), QLatin1String("<br />"));
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replacePictures(QTextDocument *pRawDoc, const QString &sTrans,
                             const QString &sCurrentFile,
                             const QString &sCommunity) {
#if defined _WIN32
  QString sExt("file:///");
#else
  QString sExt(QLatin1String(""));
#endif
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findImages(
      "\\[\\[" + sTrans + "\\(.+\\)\\]\\]",
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
  QStringList sListTmpImgInfo;

  QString sImagePath(QLatin1String(""));
  if (!sCurrentFile.isEmpty()) {
    QFileInfo fiArticleFile(sCurrentFile);
    sImagePath = fiArticleFile.absolutePath();
  }

  int nIndex = 0;
  QRegularExpressionMatch match;
  while ((match = findImages.match(sDoc, nIndex)).hasMatch()) {
    nIndex = match.capturedStart();
    QString sTmpImage = match.captured();
    sTmpImage.remove("[[" + sTrans + "(", Qt::CaseInsensitive);
    sTmpImage.remove(QStringLiteral(")]]"));

    QString sImageAlign = QStringLiteral("default");
    double iImgHeight = 0;
    double iImgWidth = 0;
    double tmpH = 0;
    double tmpW = 0;

    sListTmpImgInfo.clear();
    sListTmpImgInfo << sTmpImage.split(QStringLiteral(","));

    QString sImageUrl = sListTmpImgInfo[0].trimmed();
    if (sImageUrl.startsWith(QLatin1String("Wiki/")) ||
        sImageUrl.startsWith(QLatin1String("img/"))) {
      sImageUrl =
          m_sSharePath + "/community/" + sCommunity + "/web/" + sImageUrl;
    } else if (!sImagePath.isEmpty() &&
               QFile(sImagePath + "/" + sImageUrl).exists()) {
      sImageUrl = sImagePath + "/" + sImageUrl;
    } else {
      sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
    }

    for (int i = 1; i < sListTmpImgInfo.length(); i++) {
      // Found integer (width)
      if (0 != sListTmpImgInfo[i].trimmed().toUInt()) {
        tmpW = sListTmpImgInfo[i].trimmed().toUInt();
      } else if (sListTmpImgInfo[i].trimmed().startsWith(QLatin1String("x"))) {
        // Found x+integer (height)
        tmpH =
            sListTmpImgInfo[i].remove(QStringLiteral("x")).trimmed().toUInt();
      } else if (sListTmpImgInfo[i].contains(QLatin1String("x"))) {
        // Found int x int (width x height)
        QString sTmp = sListTmpImgInfo[i];  // Copy needed!
        tmpW = sListTmpImgInfo[i]
                   .remove(sListTmpImgInfo[i].indexOf(QLatin1String("x")),
                           sListTmpImgInfo[i].length())
                   .trimmed()
                   .toUInt();
        tmpH = sTmp.remove(0, sTmp.indexOf(QLatin1String("x")) + 1)
                   .trimmed()
                   .toUInt();
      } else if (sListTmpImgInfo[i].trimmed() == QLatin1String("left") ||
                 sListTmpImgInfo[i].trimmed() == QLatin1String("align=left")) {
        // Found alignment
        sImageAlign = QStringLiteral("left");
      } else if (sListTmpImgInfo[i].trimmed() == QLatin1String("right") ||
                 sListTmpImgInfo[i].trimmed() == QLatin1String("align=right")) {
        sImageAlign = QStringLiteral("right");
      } else if (sListTmpImgInfo[i].trimmed() == QLatin1String("center") ||
                 sListTmpImgInfo[i].trimmed() ==
                     QLatin1String("align=center")) {
        sImageAlign = QStringLiteral("center");
      }
    }

    // No size given
    if (0.0 == tmpH && 0.0 == tmpW) {
      iImgHeight = QImage(sImageUrl).height();
      tmpH = iImgHeight;
      iImgWidth = QImage(sImageUrl).width();
      tmpW = iImgWidth;
    }

    if (tmpH > tmpW) {
      iImgHeight = QImage(sImageUrl).height();
      tmpW = static_cast<double>(QImage(sImageUrl).width()) /
             (iImgHeight / static_cast<double>(tmpH));
    } else if (tmpW > tmpH) {
      iImgWidth = QImage(sImageUrl).width();
      tmpH = static_cast<double>(QImage(sImageUrl).height()) /
             (iImgWidth / static_cast<double>(tmpW));
    }

    // HTML code
    sTmpImage = "<a href=\"" + sExt + sImageUrl + "\" class=\"crosslink\">";
    sTmpImage += "<img src=\"" + sExt + sImageUrl + "\" alt=\"" + sImageUrl +
                 "\" height=\"" + QString::number(tmpH) + "\" width=\"" +
                 QString::number(tmpW) + "\" ";
    sTmpImage += "class=\"image-" + sImageAlign + "\" /></a>";

    sDoc.replace(nIndex, match.capturedLength(), sTmpImage);
    // Go on with RegExp-Search
    nIndex += sTmpImage.length();
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replaceTableOfContents(QTextDocument *pRawDoc,
                                    const QString &sTrans,
                                    QStringList &sListHeadlines) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findMacro(
      "\\[\\[" + sTrans + "\\(.*\\)\\]\\]",
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
  QString sMacro;
  QString sSpaces;
  QString sTmp;
  quint16 nCurrentLevel;

  // Replace characters for valid links (ä, ü, ö, spaces)
  QStringList sListHeadlines_Links;
  sListHeadlines_Links.reserve(sListHeadlines.size());
  for (const auto &s : sListHeadlines) {
    sMacro = s;
    sMacro.replace(QLatin1String(" "), QLatin1String("-"));
    sMacro.replace(QStringLiteral("Ä"), QLatin1String("Ae"));
    sMacro.replace(QStringLiteral("Ü"), QLatin1String("Ue"));
    sMacro.replace(QStringLiteral("Ö"), QLatin1String("Oe"));
    sMacro.replace(QStringLiteral("ä"), QLatin1String("ae"));
    sMacro.replace(QStringLiteral("ü"), QLatin1String("ue"));
    sMacro.replace(QStringLiteral("ö"), QLatin1String("oe"));
    sListHeadlines_Links << sMacro.remove(
        QRegularExpression(QStringLiteral("#{1,5}\\d#{1,5}")));
  }

  int nIndex = 0;
  QRegularExpressionMatch match;
  while ((match = findMacro.match(sDoc, nIndex)).hasMatch()) {
    nIndex = match.capturedStart();
    sMacro = match.captured(0);
    sMacro.remove("[[" + sTrans + "(", Qt::CaseInsensitive);
    sMacro.remove(QStringLiteral(")]]"));

    quint16 nTOCLevel = sMacro.trimmed().toUShort();
    if (0 == nTOCLevel || nTOCLevel > 5) {
      nTOCLevel = 3;  // Default
    }
    // qDebug() << "TOC level:" << nTOCLevel;

    sMacro = "<div class=\"toc\">\n<div class=\"head\">" + sTrans + "</div>\n";
    for (int i = 0; i < sListHeadlines.size(); i++) {
      sTmp = sListHeadlines[i];
      sTmp.remove(QRegularExpression(QStringLiteral("#{1,5}\\d#{1,5}")));
      sListHeadlines[i]
          .remove(sListHeadlines[i].length() - sTmp.length(), sTmp.length())
          .remove(QStringLiteral("#"));

      nCurrentLevel = sListHeadlines[i].toUShort();
      sSpaces.fill(' ', nCurrentLevel);

      if (nCurrentLevel > 0 && nCurrentLevel <= nTOCLevel) {
        sMacro +=
            sSpaces + "1. [#" + sListHeadlines_Links[i] + " " + sTmp + "]\n";
      } else if (0 == nCurrentLevel) {
        qWarning() << "Found strange formatted headline:" << sTmp;
      }
    }
    sMacro += QLatin1String("\n</div>\n");

    sDoc.replace(nIndex, match.capturedLength(), sMacro);
    // Go on with new start position
    nIndex += sMacro.length();
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Macros::replaceSpan(QTextDocument *pRawDoc, const QString &sTrans) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findMacro(
      "\\[\\[" + sTrans + "\\(.*\\)\\]\\]",
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
  QString sMacro;
  QStringList sArgs;
  QString sClass;
  QString sStyle;

  int nIndex = 0;
  QRegularExpressionMatch match;
  while ((match = findMacro.match(sDoc, nIndex)).hasMatch()) {
    nIndex = match.capturedStart();
    sMacro = match.captured(0);
    sMacro.remove("[[" + sTrans + "(", Qt::CaseInsensitive);
    sMacro.remove(QStringLiteral(")]]"));
    sArgs.clear();
    sClass.clear();
    sStyle.clear();

    // Extract arguments
    // Split by ',' but don't split quoted strings with comma
    const QStringList tmpList =
        sMacro.split(QRegularExpression(QStringLiteral("\"")));
    bool bInside = false;
    for (const auto &s : tmpList) {
      if (bInside) {
        // If 's' is inside quotes, get the whole string
        sArgs.append(s);
      } else {
        // If 's' is outside quotes, get the split string
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        sArgs.append(s.split(QRegularExpression(QStringLiteral(",+")),
                             QString::SkipEmptyParts));
#else
        sArgs.append(s.split(QRegularExpression(QStringLiteral(",+")),
                             Qt::SkipEmptyParts));
#endif
      }
      bInside = !bInside;
    }
    sArgs.removeAll(QStringLiteral(" "));

    sMacro.clear();
    if (!sArgs.isEmpty()) {
      sMacro = sArgs[0].trimmed();
    }
    if (sArgs.size() > 1) {
      sClass =
          " class=\"" + sArgs[1].remove(QStringLiteral("\"")).trimmed() + "\"";
    }
    if (sArgs.size() > 2) {
      sStyle =
          " style=\"" + sArgs[2].remove(QStringLiteral("\"")).trimmed() + "\"";
    }
    sMacro = "<span" + sStyle + sClass + ">" + sMacro + "</span>";

    sDoc.replace(nIndex, match.capturedLength(), sMacro);
    // Go on with new start position
    nIndex += sMacro.length();
  }

  pRawDoc->setPlainText(sDoc);
}
