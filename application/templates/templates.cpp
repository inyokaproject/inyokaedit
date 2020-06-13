/**
 * \file templates.cpp
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
 * Loading and providing templates.
 */

#include "./templates.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSettings>

Templates::Templates(const QString &sCommunity, const QString &sSharePath,
                     const QString &sUserDataDir) {
  QString sPath(sSharePath + "/community/" + sCommunity);
  this->initTemplates(sPath + "/templates");
  this->initHtmlTpl(sPath + "/Preview.tpl");
  m_sListIWLs.clear();
  m_sListIWLUrls.clear();
  Templates::initMappings(sPath + "/linkmap/linkmap.csv", ',',
                          m_sListIWLs, m_sListIWLUrls);
  m_sListIWLs << "user";      // "Build-in" IWL
  m_sListIWLUrls << "user/";  // TODO(): Minor; add community URL?

  m_sListFlags.clear();
  m_sListFlagsImg.clear();
  Templates::initMappings(sPath + "/flagmap/flagmap.csv", ',',
                          m_sListFlags, m_sListFlagsImg);
  m_sListSmilies.clear();
  m_sListSmiliesImg.clear();
  Templates::initMappings(sPath + "/SmileysMap.csv", ',',
                          m_sListSmilies, m_sListSmiliesImg);
  this->initTextformats(sPath + "/Textformats.conf");

  sPath = "/community/" + sCommunity;

  m_sListTestedWith.clear();
  m_sListTestedWithStrings.clear();
  Templates::initMappings(sSharePath + sPath + "/templates/TestedWith.conf",
                          '=', m_sListTestedWith, m_sListTestedWithStrings);
  QFile tmpFile(sUserDataDir + sPath + "/templates/TestedWith.conf");
  if (tmpFile.exists()) {
    Templates::initMappings(tmpFile.fileName(), '=',
                            m_sListTestedWith, m_sListTestedWithStrings);
  }

  m_sListTestedWithTouch.clear();
  m_sListTestedWithTouchStrings.clear();
  Templates::initMappings(sSharePath + sPath + "/templates/TestedWithTouch.conf",
                          '=', m_sListTestedWithTouch,
                          m_sListTestedWithTouchStrings);
  tmpFile.setFileName(sUserDataDir + sPath + "/templates/TestedWithTouch.conf");
  if (tmpFile.exists()) {
    Templates::initMappings(tmpFile.fileName(), '=',
                            m_sListTestedWithTouch,
                            m_sListTestedWithTouchStrings);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initTemplates(const QString &sTplPath) {
  QFile TplFile("");
  QDir TplDir(sTplPath);
  QString tmpLine("");
  QString sTempTplText("");
  QString sTempMacro("");
  bool bFoundMacro;
  bool bFoundTpl;

  // Get template files
  QFileInfoList fiListTplFiles = TplDir.entryInfoList(QDir::NoDotAndDotDot
                                                      | QDir::Files);
  foreach (QFileInfo fi, fiListTplFiles) {
    if ("tpl" == fi.completeSuffix()) {
      // qDebug() << fi.absoluteFilePath();
      TplFile.setFileName(fi.absoluteFilePath());
      if (TplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bFoundMacro = false;
        bFoundTpl = false;
        sTempTplText.clear();
        sTempMacro.clear();
        QTextStream in(&TplFile);
        in.setCodec("UTF-8");

        while (!in.atEnd()) {
          tmpLine = in.readLine().trimmed();
          if (!tmpLine.startsWith("#")) {
            bFoundTpl = true;
            sTempTplText += tmpLine + "\n";
          } else if (tmpLine.startsWith("## Macro=") && !bFoundMacro) {
            bFoundMacro = true;
            tmpLine = tmpLine.remove("## Macro=");
            sTempMacro = tmpLine.trimmed();
          }
        }

        // Found complete template
        if (bFoundMacro && bFoundTpl) {
          m_sListTplNamesINY << fi.baseName();
          m_sListTemplatesINY << sTempTplText;
          m_sListTplMacrosINY << sTempMacro;
        }
        TplFile.close();
      } else {
        QMessageBox::warning(nullptr, "Warning",
                             "Could not open template file: \n" +
                             fi.absoluteFilePath());
        qWarning() << "Could not open template file:"
                   << fi.absoluteFilePath();
      }
    } else if ("macro" == fi.completeSuffix()) {
      TplFile.setFileName(fi.absoluteFilePath());
      if (TplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&TplFile);
        in.setCodec("UTF-8");
        tmpLine = in.readLine().trimmed();
        if (tmpLine.startsWith("## Macro=")) {
          tmpLine = tmpLine.remove("## Macro=");
          m_sListTplMacrosALL << tmpLine.trimmed();
          m_sListTplNamesALL << fi.baseName();
        }
        TplFile.close();
      } else {
        QMessageBox::warning(nullptr, "Warning",
                             "Could not open macro file: \n" +
                             fi.absoluteFilePath());
        qWarning() << "Could not open macro file:"
                   << fi.absoluteFilePath();
      }
    }
  }

  m_sListTplMacrosALL.append(m_sListTplMacrosINY);
  m_sListTplNamesALL.append(m_sListTplNamesINY);

  if (m_sListTplNamesINY.isEmpty()) {
    QMessageBox::warning(nullptr, "Warning",
                         "Could not find any markup template files!");
    qWarning() << "Could not find any template files in:"
               << TplDir.absolutePath();
  }

  qDebug() << "Found templates:" << m_sListTplNamesINY;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initHtmlTpl(const QString &sTplFile) {
  QFile HTMLTplFile(sTplFile);
  if (!HTMLTplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, "Warning",
                         "Could not open preview template file!");
    qWarning() << "Could not open preview template file:"
               << HTMLTplFile.fileName();
    m_sPreviewTemplate = "ERROR";
  } else {
    QTextStream in(& HTMLTplFile);
    in.setCodec("UTF-8");
    m_sPreviewTemplate = in.readAll();

    HTMLTplFile.close();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initMappings(const QString &sFileName,
                             const QChar cSplit,
                             QStringList &sListElements,
                             QStringList &sListMapping) {
  QFile MapFile(sFileName);
  if (!MapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, "Warning", "Could not open mapping file!");
    qWarning() << "Could not open mapping config file:"
               << MapFile.fileName();
    sListElements << "ERROR";
    sListMapping << "ERROR";
  } else {
    QTextStream in(&MapFile);
    in.setCodec("UTF-8");
    QString tmpLine;
    QString sElement;
    QString sMapping;
    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith("#") && !tmpLine.trimmed().isEmpty()) {
        sElement = tmpLine.section(cSplit, 0, 0);  // Split at first occurrence
        sMapping = tmpLine.section(cSplit, 1);     // Second part after match
        if (!sElement.isEmpty() && !sMapping.isEmpty() &&
            !sListElements.contains(sElement.trimmed())) {
          sListElements << sElement.trimmed();
          sListMapping << sMapping.trimmed();
        }
      }
    }
    MapFile.close();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initTextformats(const QString &sFilename) {
  QFile formatsFile(sFilename);
  QStringList sListInput;

  if (!formatsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr,
                         "Warning", "Could not open text formats file!");
    qWarning() << "Could not open text formats config file:"
               << formatsFile.fileName();
    // Initialize possible text formats
    m_sListFormatStart << "ERROR";
    m_sListFormatEnd << "ERROR";
    m_sListFormatHtmlStart << "ERROR";
    m_sListFormatHtmlEnd << "ERROR";
  } else {
    QTextStream in(&formatsFile);
    in.setCodec("UTF-8");
    QString tmpLine;
    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith("#") && !tmpLine.trimmed().isEmpty()) {
        sListInput << tmpLine.trimmed();
      }
    }
    formatsFile.close();

    for (int i = 0; i + 3 < sListInput.size(); i += 4) {
      m_sListFormatStart << sListInput[i];
      m_sListFormatEnd << sListInput[i+1];
      m_sListFormatHtmlStart << sListInput[i+2];
      m_sListFormatHtmlEnd << sListInput[i+3];
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Templates::getPreviewTemplate() const -> QString {
  return m_sPreviewTemplate;
}

auto Templates::getListTplNamesINY() const -> QStringList {
  return m_sListTplNamesINY;
}
auto Templates::getListTemplatesINY() const -> QStringList {
  return m_sListTemplatesINY;
}
auto Templates::getListTplMacrosINY() const -> QStringList {
  return m_sListTplMacrosINY;
}
auto Templates::getListTplNamesALL() const -> QStringList {
  return m_sListTplNamesALL;
}
auto Templates::getListTplMacrosALL() const -> QStringList {
  return m_sListTplMacrosALL;
}

auto Templates::getListFormatStart() const -> QStringList {
  return m_sListFormatStart;
}
auto Templates::getListFormatEnd() const -> QStringList {
  return m_sListFormatEnd;
}
auto Templates::getListFormatHtmlStart() const -> QStringList {
  return m_sListFormatHtmlStart;
}
auto Templates::getListFormatHtmlEnd() const -> QStringList {
  return m_sListFormatHtmlEnd;
}

// ----------------------------------------------------------------------------
// Mappings

auto Templates::getListIWLs() const -> QStringList {
  return m_sListIWLs;
}
auto Templates::getListIWLUrls() const -> QStringList {
  return m_sListIWLUrls;
}

auto Templates::getListFlags() const -> QStringList {
  return m_sListFlags;
}
auto Templates::getListFlagsImg() const -> QStringList {
  return m_sListFlagsImg;
}

auto Templates::getListSmilies() const -> QStringList {
  return m_sListSmilies;
}
auto Templates::getListSmiliesImg() const -> QStringList {
  return m_sListSmiliesImg;
}

auto Templates::getListTestedWith() const -> QStringList {
  return m_sListTestedWith;
}
auto Templates::getListTestedWithStrings() const -> QStringList {
  return m_sListTestedWithStrings;
}

auto Templates::getListTestedWithTouch() const -> QStringList {
  return m_sListTestedWithTouch;
}
auto Templates::getListTestedWithTouchStrings() const -> QStringList {
  return m_sListTestedWithTouchStrings;
}
