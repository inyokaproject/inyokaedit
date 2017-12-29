/**
 * \file macros.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Class definition for macros.
 */

#ifndef INYOKAEDIT_MACROS_H_
#define INYOKAEDIT_MACROS_H_

#include <QDir>

#include <QTextDocument>

struct MACRO {
  QString name;
  QStringList translations;
};

class Macros {
 public:
  Macros(const QString &sSharePath, const QDir &tmpImgDir);
  void startParsing(QTextDocument *pRawDoc,
                    const QString &sCurrentFile,
                    const QString &sCommunity,
                    QStringList &sListHeadlines);

 private:
  void replaceAnchors(QTextDocument *pRawDoc, const QString &sTrans);
  void replaceAttachments(QTextDocument *pRawDoc, const QString &sTrans);
  void replaceDates(QTextDocument *pRawDoc, const QString &sTrans);
  void replaceNewline(QTextDocument *pRawDoc, const QString &sTrans);
  void replacePictures(QTextDocument *pRawDoc, const QString &sTrans,
                       const QString &sCurrentFile, const QString &sCommunity);
  void replaceTableOfContents(QTextDocument *pRawDoc, const QString &sTrans,
                              QStringList &sListHeadlines);
  void replaceSpan(QTextDocument *pRawDoc, const QString &sTrans);

  const QString m_sSharePath;
  const QDir m_tmpImgDir;
  QList<MACRO> m_listMacros;
};

#endif  // INYOKAEDIT_MACROS_H_
