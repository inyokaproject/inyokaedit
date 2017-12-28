/**
 * \file templates.cpp
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
 * Loading and providing templates.
 */

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSettings>

#include "./templates.h"

Templates::Templates(const QString &sCommunity, const QString &sSharePath,
                     const QString &sUserDataDir) {
  QString sPath(sSharePath + "/community/" + sCommunity);
  this->initTemplates(sPath + "/templates");
  this->initHtmlTpl(sPath + "/Preview.tpl");
  m_sListIWLs.clear();
  m_sListIWLUrls.clear();
  this->initMappings(sPath + "/InterWikiMap.conf", m_sListIWLs, m_sListIWLUrls);
  m_sListFlags.clear();
  m_sListFlagsImg.clear();
  this->initMappings(sPath + "/FlagsMap.conf", m_sListFlags, m_sListFlagsImg);
  m_sListSmilies.clear();
  m_sListSmiliesImg.clear();
  this->initMappings(sPath + "/SmiliesMap.conf", m_sListSmilies, m_sListSmiliesImg);
  this->initTextformats(sPath + "/Textformats.conf");
  this->initTranslations(sPath + "/community.conf");

  sPath = "/community/" + sCommunity;

  m_sListTestedWith.clear();
  m_sListTestedWithStrings.clear();
  this->initMappings(sSharePath + sPath + "/templates/TestedWith.conf",
                     m_sListTestedWith, m_sListTestedWithStrings);
  QFile tmpFile(sUserDataDir + sPath + "/templates/TestedWith.conf");
  if (tmpFile.exists()) {
    this->initMappings(tmpFile.fileName(),
                       m_sListTestedWith, m_sListTestedWithStrings);
  }

  m_sListTestedWithTouch.clear();
  m_sListTestedWithTouchStrings.clear();
  this->initMappings(sSharePath + sPath + "/templates/TestedWithTouch.conf",
                     m_sListTestedWithTouch, m_sListTestedWithTouchStrings);
  tmpFile.setFileName(sUserDataDir + sPath + "/templates/TestedWithTouch.conf");
  if (tmpFile.exists()) {
    this->initMappings(tmpFile.fileName(),
                       m_sListTestedWithTouch, m_sListTestedWithTouchStrings);
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
          } else if (tmpLine.startsWith("## Macro=")
                     && false == bFoundMacro) {
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
        QMessageBox::warning(0, "Warning",
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
        QMessageBox::warning(0, "Warning",
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
    QMessageBox::warning(0, "Warning",
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
    QMessageBox::warning(0, "Warning",
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
                             QStringList &sListElements,
                             QStringList &sListMapping) {
  QFile MapFile(sFileName);
  if (!MapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(0, "Warning", "Could not open mapping file!");
    qWarning() << "Could not open mapping config file:"
               << MapFile.fileName();
    sListElements << "ERROR";
  } else {
    QTextStream in(&MapFile);
    in.setCodec("UTF-8");
    QString tmpLine;
    QString sElement;
    QString sMapping;
    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith("#") && !tmpLine.trimmed().isEmpty()) {
        sElement = tmpLine.section('=', 0, 0);  // Split only first '='
        sMapping = tmpLine.section('=', 1);     // Second part after first '='
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
    QMessageBox::warning(0, "Warning", "Could not open text formats file!");
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

void Templates::initTranslations(const QString &sFilename) {
  QFile translFile(sFilename);
  if (!translFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(0, "Error",
                          "Could not open template translation file!");
    qCritical() << "Could not open template translation file:"
                << translFile.fileName();
    exit(-3);
  }

  QSettings configTransl(translFile.fileName(), QSettings::IniFormat);
  configTransl.setIniCodec("UTF-8");

  configTransl.beginGroup("Translations");
  m_sTransAnchor = configTransl.value("Anchor", "ERROR").toString();
  if ("ERROR" == m_sTransAnchor) {
    qCritical() << "Anchor translation not found.";
  }
  m_sTransAttachment = configTransl.value("Attachment", "ERROR").toString();
  if ("ERROR" == m_sTransAttachment) {
    qCritical() << "Attachment translation not found.";
  }
  m_sTransCodeBlock = configTransl.value("CodeBlock", "ERROR").toString();
  if ("ERROR" == m_sTransCodeBlock) {
    qCritical() << "Code block translation not found.";
  }
  m_sTransDate = configTransl.value("Date", "ERROR").toString();
  if ("ERROR" == m_sTransDate) {
    qCritical() << "Date translation not found.";
  }
  m_sTransImage = configTransl.value("Image", "ERROR").toString();
  if ("ERROR" == m_sTransImage) {
    qCritical() << "Image translation not found.";
  }
  m_sTransRevText = configTransl.value("RevText", "ERROR").toString();
  if ("ERROR" == m_sTransRevText) {
    qCritical() << "Revision text translation not found.";
  }
  m_sTransTOC = configTransl.value("TableOfContents", "ERROR").toString();
  if ("ERROR" == m_sTransTOC) {
    qCritical() << "TOC translation not found.";
  }
  m_sTransTagText = configTransl.value("TagText", "ERROR").toString();
  if ("ERROR" == m_sTransTagText) {
    qCritical() << "Tag text translation not found.";
  }
  m_sTransTemplate = configTransl.value("Template", "ERROR").toString();
  if ("ERROR" == m_sTransTemplate) {
    qCritical() << "Template translation not found.";
  }

  // Translation needed for table template plugin
  m_sTransTable = configTransl.value("Table", "ERROR").toString();
  if ("ERROR" == m_sTransTable) {
    qCritical() << "Table translation not found.";
  }
  // Translation needed for knowledge box selector plugin
  m_sTransKnowledge = configTransl.value("Knowledge", "ERROR").toString();
  if ("ERROR" == m_sTransKnowledge) {
    qCritical() << "Knowledge box translation not found.";
  }
  configTransl.endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Templates::getPreviewTemplate() const {
  return m_sPreviewTemplate;
}

QStringList Templates::getListTplNamesINY() const {
  return m_sListTplNamesINY;
}
QStringList Templates::getListTemplatesINY() const {
  return m_sListTemplatesINY;
}
QStringList Templates::getListTplMacrosINY() const {
  return m_sListTplMacrosINY;
}
QStringList Templates::getListTplNamesALL() const {
  return m_sListTplNamesALL;
}
QStringList Templates::getListTplMacrosALL() const {
  return m_sListTplMacrosALL;
}

QString Templates::getTransAnchor() const {
  return m_sTransAnchor;
}
QString Templates::getTransAttachment() const {
  return m_sTransAttachment;
}
QString Templates::getTransCodeBlock() const {
  return m_sTransCodeBlock;
}
QString Templates::getTransDate() const {
  return m_sTransDate;
}
QString Templates::getTransImage() const {
  return m_sTransImage;
}
QString Templates::getTransKnowledge() const {
  return m_sTransKnowledge;
}
QString Templates::getTransRev() const {
  return m_sTransRevText;
}
QString Templates::getTransTable() const {
  return m_sTransTable;
}
QString Templates::getTransTag() const {
  return m_sTransTagText;
}
QString Templates::getTransTemplate() const {
  return m_sTransTemplate;
}
QString Templates::getTransTOC() const {
  return m_sTransTOC;
}

QStringList Templates::getListFormatStart() const {
  return m_sListFormatStart;
}
QStringList Templates::getListFormatEnd() const {
  return m_sListFormatEnd;
}
QStringList Templates::getListFormatHtmlStart() const {
  return m_sListFormatHtmlStart;
}
QStringList Templates::getListFormatHtmlEnd() const {
  return m_sListFormatHtmlEnd;
}

// ----------------------------------------------------------------------------
// Mappings

QStringList Templates::getListIWLs() const {
  return m_sListIWLs;
}
QStringList Templates::getListIWLUrls() const {
  return m_sListIWLUrls;
}

QStringList Templates::getListFlags() const {
  return m_sListFlags;
}
QStringList Templates::getListFlagsImg() const {
  return m_sListFlagsImg;
}

QStringList Templates::getListSmilies() const {
  return m_sListSmilies;
}
QStringList Templates::getListSmiliesImg() const {
  return m_sListSmiliesImg;
}

QStringList Templates::getListTestedWith() const {
  return m_sListTestedWith;
}
QStringList Templates::getListTestedWithStrings() const {
  return m_sListTestedWithStrings;
}

QStringList Templates::getListTestedWithTouch() const {
  return m_sListTestedWithTouch;
}
QStringList Templates::getListTestedWithTouchStrings() const {
  return m_sListTestedWithTouchStrings;
}
