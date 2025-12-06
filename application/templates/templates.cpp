// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

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

  m_IwlMap.clear();
  Templates::initMap(sPath + "/linkmap/linkmap.csv", ',', &m_IwlMap);
  m_IwlMap.insert(QStringLiteral("user"), QStringLiteral("user/"));  // Build-in
  m_FlagMap.clear();
  Templates::initMap(sPath + "/flagmap/flagmap.csv", ',', &m_FlagMap);

  Templates::initTxtMap(sPath + "/SmileysMap.csv", ',', &m_SmiliesTxtMap);

  this->initTextformats(sPath + "/Textformats.conf");

  sPath = "/community/" + sCommunity;

  m_TestedWithMap.clear();
  Templates::initMap(sSharePath + sPath + "/templates/TestedWith.conf", '=',
                     &m_TestedWithMap);
  QFile tmpFile(sUserDataDir + sPath + "/templates/TestedWith.conf");
  if (tmpFile.exists()) {
    Templates::initMap(tmpFile.fileName(), '=', &m_TestedWithMap);
  }

  m_TestedWithTouchMap.clear();
  Templates::initMap(sSharePath + sPath + "/templates/TestedWithTouch.conf",
                     '=', &m_TestedWithTouchMap);
  tmpFile.setFileName(sUserDataDir + sPath + "/templates/TestedWithTouch.conf");
  if (tmpFile.exists()) {
    Templates::initMap(tmpFile.fileName(), '=', &m_TestedWithTouchMap);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initTemplates(const QString &sTplPath) {
  QFile TplFile(QLatin1String(""));
  QDir TplDir(sTplPath);
  QString tmpLine;
  QString sTempTplLangText(QLatin1String(""));
  QString sTempMacro(QLatin1String(""));
  bool bFoundMacro;
  bool bFoundTpl;

  // Get template files
  const QFileInfoList fiListTplFiles =
      TplDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
  for (const auto &fi : fiListTplFiles) {
    if ("tpl" == fi.completeSuffix()) {
      // qDebug() << fi.absoluteFilePath();
      TplFile.setFileName(fi.absoluteFilePath());
      if (TplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bFoundMacro = false;
        bFoundTpl = false;
        sTempTplLangText.clear();
        sTempMacro.clear();
        QTextStream in(&TplFile);

        while (!in.atEnd()) {
          tmpLine = in.readLine().trimmed();
          if (!tmpLine.startsWith(QLatin1String("#"))) {
            bFoundTpl = true;
            sTempTplLangText += tmpLine + "\n";
          } else if (tmpLine.startsWith(QLatin1String("## Macro=")) &&
                     !bFoundMacro) {
            bFoundMacro = true;
            tmpLine = tmpLine.remove(QStringLiteral("## Macro="));
            sTempMacro = tmpLine.trimmed();
          }
        }

        // Found complete template
        if (bFoundMacro && bFoundTpl) {
          m_TemplateMap.insert(fi.baseName(), sTempMacro);
          m_InyokaTplLangMap.insert(fi.baseName(), sTempTplLangText);
        }
        TplFile.close();
      } else {
        QMessageBox::warning(
            nullptr, QStringLiteral("Warning"),
            "Could not open template file: \n" + fi.absoluteFilePath());
        qWarning() << "Could not open template file:" << fi.absoluteFilePath();
      }
    } else if ("macro" == fi.completeSuffix()) {
      TplFile.setFileName(fi.absoluteFilePath());
      if (TplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&TplFile);
        tmpLine = in.readLine().trimmed();
        if (tmpLine.startsWith(QLatin1String("## Macro="))) {
          tmpLine = tmpLine.remove(QStringLiteral("## Macro="));
          m_MacroMap.insert(fi.baseName(), tmpLine.trimmed());
        }
        TplFile.close();
      } else {
        QMessageBox::warning(
            nullptr, QStringLiteral("Warning"),
            "Could not open macro file: \n" + fi.absoluteFilePath());
        qWarning() << "Could not open macro file:" << fi.absoluteFilePath();
      }
    }
  }

  if (m_TemplateMap.isEmpty() || m_InyokaTplLangMap.isEmpty()) {
    QMessageBox::warning(
        nullptr, QStringLiteral("Warning"),
        QStringLiteral("Could not find any markup template files!"));
    qWarning() << "Could not find any template files in:"
               << TplDir.absolutePath();
  }

  QStringList sSorted;
  sSorted = m_TemplateMap.keys();
  sSorted.sort();
  qDebug() << "Found templates:" << sSorted;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initHtmlTpl(const QString &sTplFile) {
  QFile HTMLTplFile(sTplFile);
  if (!HTMLTplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(
        nullptr, QStringLiteral("Warning"),
        QStringLiteral("Could not open preview template file!"));
    qWarning() << "Could not open preview template file:"
               << HTMLTplFile.fileName();
    m_sPreviewTemplate = QStringLiteral("ERROR");
  } else {
    QTextStream in(&HTMLTplFile);
    m_sPreviewTemplate = in.readAll();

    HTMLTplFile.close();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initMap(const QString &sFile, const QChar cSplit,
                        QHash<QString, QString> *map) {
  QFile MapFile(sFile);
  if (!MapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, QStringLiteral("Warning"),
                         QStringLiteral("Could not open mapping file!"));
    qWarning() << "Could not open mapping config file:" << MapFile.fileName();
    map->clear();
  } else {
    QTextStream in(&MapFile);
    QString tmpLine;
    QString sKey;
    QString sValue;
    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith(QLatin1String("#")) && !tmpLine.isEmpty()) {
        sKey = tmpLine.section(cSplit, 0, 0)
                   .trimmed();  // Split at first occurrence
        sValue =
            tmpLine.section(cSplit, 1).trimmed();  // Second part after match
        if (!sKey.isEmpty() && !sValue.isEmpty() && !map->contains(sKey)) {
          map->insert(sKey, sValue);
        }
      }
    }
    MapFile.close();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Templates::initTxtMap(const QString &sFile, const QChar cSplit,
                           QPair<QStringList, QStringList> *map) {
  QFile MapFile(sFile);
  if (!MapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, QStringLiteral("Warning"),
                         QStringLiteral("Could not open TXT mapping file!"));
    qWarning() << "Could not open TXT mapping config file:"
               << MapFile.fileName();
  } else {
    QTextStream in(&MapFile);
    QString tmpLine;
    QString sKey;
    QString sValue;
    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith(QLatin1String("#")) && !tmpLine.isEmpty()) {
        sKey = tmpLine.section(cSplit, 0, 0)
                   .trimmed();  // Split at first occurrence
        sValue =
            tmpLine.section(cSplit, 1).trimmed();  // Second part after match
        if (!sKey.isEmpty() && !sValue.isEmpty() &&
            !map->first.contains(sKey)) {
          map->first << sKey;
          map->second << sValue;
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

  if (!formatsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, QStringLiteral("Warning"),
                         QStringLiteral("Could not open text formats file!"));
    qWarning() << "Could not open text formats config file:"
               << formatsFile.fileName();
  } else {
    QStringList sListInput;
    QTextStream in(&formatsFile);
    while (!in.atEnd()) {
      QString tmpLine = in.readLine().trimmed();
      if (!tmpLine.startsWith(QLatin1String("#")) && !tmpLine.isEmpty()) {
        sListInput << tmpLine;
      }
    }
    formatsFile.close();

    for (int i = 0; i + 3 < sListInput.size(); i += 4) {
      m_FormatStartMap.first << sListInput.at(i);
      m_FormatEndMap.first << sListInput.at(i + 1);
      m_FormatStartMap.second << sListInput.at(i + 2);
      m_FormatEndMap.second << sListInput.at(i + 3);

      if (sListInput.at(i + 2).contains(
              QLatin1String("class=\"notranslate\""))) {
        m_FormatStartNoTranslateMap.first << sListInput.at(i);
        m_FormatEndNoTranslateMap.first << sListInput.at(i + 1);
        m_FormatStartNoTranslateMap.second << sListInput.at(i + 2);
        m_FormatEndNoTranslateMap.second << sListInput.at(i + 3);
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Templates::getPreviewTemplate() const -> QString {
  return m_sPreviewTemplate;
}

auto Templates::getTemplateMap() const -> QHash<QString, QString> {
  return m_TemplateMap;
}

auto Templates::getInyokaTplLangMap() const -> QHash<QString, QString> {
  return m_InyokaTplLangMap;
}

auto Templates::getAllBoilerplates() const -> QStringList {
  return QStringList() << m_MacroMap.values() << m_TemplateMap.values();
}

// ----------------------------------------------------------------------------
// Mappings

auto Templates::getIwlMap() const -> QHash<QString, QString> {
  return m_IwlMap;
}

auto Templates::getFlagMap() const -> QHash<QString, QString> {
  return m_FlagMap;
}

auto Templates::getTestedWithMap() const -> QHash<QString, QString> {
  return m_TestedWithMap;
}

auto Templates::getTestedWithTouchMap() const -> QHash<QString, QString> {
  return m_TestedWithTouchMap;
}

// ----------------------------------------------------------------------------
// Txt mappings

auto Templates::getSmiliesTxtMap() const -> QPair<QStringList, QStringList> {
  return m_SmiliesTxtMap;
}

auto Templates::getFormatStartMap() const -> QPair<QStringList, QStringList> {
  return m_FormatStartMap;
}
auto Templates::getFormatEndMap() const -> QPair<QStringList, QStringList> {
  return m_FormatEndMap;
}

auto Templates::getFormatStartNoTranslateMap() const
    -> QPair<QStringList, QStringList> {
  return m_FormatStartNoTranslateMap;
}
auto Templates::getFormatEndNoTranslateMap() const
    -> QPair<QStringList, QStringList> {
  return m_FormatEndNoTranslateMap;
}
