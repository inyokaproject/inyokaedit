/**
 * \file highlighter.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2021 The InyokaEdit developers
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
 * Syntax highlighting with builtin Qt functions.
 */

#include "./highlighter.h"

#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>

#include "../../application/templates/templates.h"
#include "../../application/texteditor.h"

#include "ui_highlighter.h"

const QString Highlighter::sSEPARATOR = QStringLiteral("|");

void Highlighter::initPlugin(QWidget *pParent, TextEditor *pEditor,
                             const QDir &userDataDir,
                             const QString &sSharePath) {
  Q_UNUSED(pEditor)
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
  m_sExt = QStringLiteral(".conf");
#else
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
  m_sExt = QStringLiteral(".ini");
#endif
  m_sSharePath = sSharePath;

  this->copyDefaultStyles();

  if (pParent->window()->palette().window().color().lightnessF() <
      m_pSettings->value(QStringLiteral("DarkThreshold"), 0.5).toDouble()) {
      m_sStyleFile = QStringLiteral("dark-style");
  } else {
    m_sStyleFile = QStringLiteral("standard-style");
  }
  // Check for old entry
  if (!m_pSettings->value(QStringLiteral("Style"), "").toString().isEmpty()) {
    m_sStyleFile = m_pSettings->value(QStringLiteral("Style"), "").toString();
  }
  m_pSettings->remove(QStringLiteral("Style"));
  // New plugin entry
  m_pSettings->beginGroup("Plugin_" + QStringLiteral(PLUGIN_NAME));
  if (!m_pSettings->value(QStringLiteral("Style"), "").toString().isEmpty()) {
    m_sStyleFile = m_pSettings->value(QStringLiteral("Style"), "").toString();
  }
  m_pSettings->setValue(QStringLiteral("Style"), m_sStyleFile);
  m_pSettings->endGroup();

  m_pStyleSet = nullptr;
  m_pTemplates = new Templates(
                   m_pSettings->value(QStringLiteral("Inyoka/Community"),
                                      "ubuntuusers_de").toString(),
                   m_sSharePath, userDataDir.absolutePath());
  this->getTranslations();
  this->readStyle(m_sStyleFile);
  this->defineRules();

  this->buildUi(pParent);  // After loading template entries
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Highlighter::getPluginName() const -> QString {
  return QStringLiteral(PLUGIN_NAME);
}

auto Highlighter::getPluginVersion() const -> QString {
  return QStringLiteral(PLUGIN_VERSION);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);

  if (!m_translator.load(":/" + QStringLiteral(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QStringLiteral(PLUGIN_NAME).toLower() +
                  "_" + sLang + ".qm";
    if (!m_translator.load(QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" +
                    QStringLiteral(PLUGIN_NAME).toLower() +
                    "_" + sLang + ".qm";
      return;
    }
  }

  if (qApp->installTranslator(&m_translator) || "en" == sLang) {
    m_pUi->retranslateUi(m_pDialog);
  } else {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Highlighter::getCaption() const -> QString {
  return tr("Syntax highlighter");
}
auto Highlighter::getIcon() const -> QIcon {
  return QIcon();
  // return QIcon(":/highlighter.png");
}

auto Highlighter::includeMenu() const -> bool {
  return false;
}
auto Highlighter::includeToolbar() const -> bool {
  return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::callPlugin() {
  qDebug() << Q_FUNC_INFO;
  m_pDialog->show();
  m_pDialog->exec();
}

void Highlighter::executePlugin() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::copyDefaultStyles() {
  QFileInfo fi(m_pSettings->fileName());
  QDir confDir(fi.absolutePath());
  if (!confDir.exists()) {
    confDir.mkpath(confDir.absolutePath());
  }

  QFile stylefile(confDir.absolutePath() + "/standard-style" + m_sExt);
  if (!stylefile.exists()) {
    if (!QFile::copy(QStringLiteral(":/standard-style.conf"),
                     confDir.absolutePath() + "/standard-style" + m_sExt)) {
      qWarning() << "Couldn't create style file: " << stylefile.fileName();
    }
  }

  stylefile.setFileName(confDir.absolutePath() + "/dark-style" + m_sExt);
  if (!stylefile.exists()) {
    if (!QFile::copy(QStringLiteral(":/dark-style.conf"),
                     confDir.absolutePath() + "/dark-style" + m_sExt)) {
      qWarning() << "Couldn't create style file: " << stylefile.fileName();
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::buildUi(QWidget *pParent) {
  m_pDialog = new QDialog(pParent);
  m_pUi = new Ui::HighlighterDialog();
  m_pUi->setupUi(m_pDialog);
  m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                            & ~Qt::WindowContextHelpButtonHint);
  m_pDialog->setModal(true);
  // m_pDialog->setWindowIcon(this->getIcon());
  m_pUi->styleTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

  QStringList sListStyleFiles;
  QFileInfo fiStyleFile(m_pSettings->fileName());
  const QFileInfoList fiListFiles(fiStyleFile.absoluteDir().entryInfoList(
                                    QDir::NoDotAndDotDot | QDir::Files));
  for (const auto &fi : fiListFiles) {
    if (fi.fileName().endsWith("-style" + m_sExt)) {
      sListStyleFiles << fi.fileName().remove(m_sExt);
    }
  }
  sListStyleFiles.push_front(tr("Create new style..."));
  m_pUi->styleFilesBox->addItems(sListStyleFiles);
  m_pUi->styleFilesBox->insertSeparator(1);

  m_pUi->styleFilesBox->setCurrentIndex(
        m_pUi->styleFilesBox->findText(m_sStyleFile));
  this->loadHighlighting(m_sStyleFile);

  QStringList sListHeader;
  sListHeader << tr("Color") << tr("Bold")
              << tr("Italic") << tr("Background");
  m_pUi->styleTable->setHorizontalHeaderLabels(sListHeader);
  sListHeader.clear();
  sListHeader << tr("Background") << tr("Text color")
              << tr("Text formating") << tr("Heading")
              << tr("Hyperlink") << tr("InterWiki")
              << tr("Macro") << tr("Parser") << tr("List")
              << tr("Table line") << tr("Table cell format")
              << tr("ImgMap") << tr("Misc") << tr("Comment")
              << tr("Syntax error");
  m_pUi->styleTable->setVerticalHeaderLabels(sListHeader);

  connect(m_pUi->styleFilesBox,
          static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &Highlighter::changedStyle);
  connect(m_pUi->styleTable, &QTableWidget::cellDoubleClicked,
          this, &Highlighter::clickedStyleCell);

  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted,
          this, &Highlighter::accept);
  connect(m_pUi->buttonBox, &QDialogButtonBox::rejected,
          m_pDialog, &QDialog::reject);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::readStyle(const QString &sStyle) {
  bool bOk = false;
  m_bSystemForeground = false;
  m_bSystemBackground = false;
  QString sTmpKey;
  QColor tmpColor;

  delete m_pStyleSet;
  m_pStyleSet = nullptr;

#if defined __linux__
  m_pStyleSet = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(), sStyle);
#else
  m_pStyleSet = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(), sStyle);
#endif

  if (!QFile::exists(m_pStyleSet->fileName())) {
    qWarning() << "Could not find/open highlighting style file:" <<
                  m_pStyleSet->fileName();
  }

  m_colorForeground = QApplication::palette().color(QPalette::Text);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Foreground"), "System")
            .toString();
  if (sTmpKey.toLower() != QLatin1String("system")) {
    tmpColor.setRgb(sTmpKey.toUInt(&bOk, 16));
    if (bOk) {
      m_colorForeground = tmpColor;
    }
  } else {
    m_bSystemForeground = true;
  }

  m_colorBackground = QApplication::palette().color(QPalette::Base);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Background"), "System")
            .toString();
  if (sTmpKey.toLower() != QLatin1String("system")) {
    tmpColor.setRgb(sTmpKey.toUInt(&bOk, 16));
    if (bOk) {
      m_colorBackground = tmpColor;
    }
  } else {
    m_bSystemBackground = true;
  }

  m_pStyleSet->beginGroup(QStringLiteral("Style"));
  sTmpKey = m_pStyleSet->value(QStringLiteral("Heading"),
                               "0x008000" + sSEPARATOR + "true")
            .toString();
  m_headingsFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Hyperlink"),
                               "0x000080").toString();
  m_linksFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("InterWiki"),
                               "0x0000ff").toString();
  m_interwikiLinksFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Macro"),
                               "0x008080").toString();
  m_macrosFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Parser"),
                               "0x800000" + sSEPARATOR + "true")
            .toString();
  m_parserFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Comment"),
                               "0xa0a0a4").toString();
  m_commentFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("ImgMap"),
                               "0x808000").toString();
  m_imgMapFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("TableCellFormating"),
                               "0x800080").toString();
  m_tablecellsFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("TextFormating"),
                               "0xff0000").toString();
  m_textformatFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("List"),
                               "0xff0000").toString();
  m_listFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("NewTableLine"),
                               "0xff0000").toString();
  m_newTableLineFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(QStringLiteral("Misc"),
                               "0xff0000").toString();
  m_miscFormat = Highlighter::evalKey(sTmpKey);
  sTmpKey = m_pStyleSet->value(
              QStringLiteral("SyntaxError"), "---" + sSEPARATOR + "---" +
              sSEPARATOR + "---" + sSEPARATOR + "0xffff00").toString();
  m_syntaxErrorFormat = Highlighter::evalKey(sTmpKey);
  m_pStyleSet->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Highlighter::evalKey(const QString &sKey) -> QTextCharFormat {
  bool bOk = false;
  QColor tmpColor;
  QBrush tmpBrush;
  QTextCharFormat retFormat;

  // Set defaults
  tmpBrush.setColor(Qt::transparent);
  tmpBrush.setStyle(Qt::SolidPattern);
  retFormat.setBackground(tmpBrush);
  tmpBrush.setColor(Qt::black);
  tmpBrush.setStyle(Qt::SolidPattern);
  retFormat.setForeground(tmpBrush);
  retFormat.setFontWeight(QFont::Normal);
  retFormat.setFontItalic(false);

  QStringList sListTmp = sKey.split(sSEPARATOR);

  // Foreground color
  if (!sListTmp.isEmpty()) {
    if ("---" != sListTmp[0].trimmed()) {
      tmpColor.setRgb(sListTmp[0].trimmed().toUInt(&bOk, 16));
      if (bOk) {
        tmpBrush.setColor(tmpColor);
        retFormat.setForeground(tmpBrush);
      }
    }
    // Font weight
    if (sListTmp.size() > 1) {
      if ("---" != sListTmp[1].trimmed()) {
        if ("true" == sListTmp[1].trimmed().toLower()) {
          retFormat.setFontWeight(QFont::Bold);
        }
      }
      // Italic
      if (sListTmp.size() > 2) {
        if ("---" != sListTmp[2].trimmed()) {
          if ("true" == sListTmp[2].trimmed().toLower()) {
            retFormat.setFontItalic(true);
          }
        }
        // Background
        if (sListTmp.size() > 3) {
          if ("---" != sListTmp[3].trimmed()) {
            tmpColor.setRgb(sListTmp[3].trimmed().toUInt(&bOk, 16));
            if (bOk) {
              tmpBrush.setColor(tmpColor);
              retFormat.setBackground(tmpBrush);
            }
          }
        }
      }
    }
  }
  return retFormat;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::saveStyle() {
  if (m_bSystemForeground) {
    m_pStyleSet->setValue(QStringLiteral("Foreground"), "System");
  } else {
    m_pStyleSet->setValue(QStringLiteral("Foreground"),
                          "0x" + m_colorForeground.name()
                          .remove(QStringLiteral("#")));
  }
  if (m_bSystemBackground) {
    m_pStyleSet->setValue(QStringLiteral("Background"), "System");
  } else {
    m_pStyleSet->setValue(QStringLiteral("Background"),
                          "0x" + m_colorBackground.name()
                          .remove(QStringLiteral("#")));
  }

  m_pStyleSet->beginGroup(QStringLiteral("Style"));
  this->writeFormat(QStringLiteral("TextFormating"), m_textformatFormat);
  this->writeFormat(QStringLiteral("Heading"), m_headingsFormat);
  this->writeFormat(QStringLiteral("Hyperlink"), m_linksFormat);
  this->writeFormat(QStringLiteral("InterWiki"), m_interwikiLinksFormat);
  this->writeFormat(QStringLiteral("Macro"), m_macrosFormat);
  this->writeFormat(QStringLiteral("Parser"), m_parserFormat);
  this->writeFormat(QStringLiteral("List"), m_listFormat);
  this->writeFormat(QStringLiteral("NewTableLine"), m_newTableLineFormat);
  this->writeFormat(QStringLiteral("TableCellFormating"), m_tablecellsFormat);
  this->writeFormat(QStringLiteral("ImgMap"), m_imgMapFormat);
  this->writeFormat(QStringLiteral("Misc"), m_miscFormat);
  this->writeFormat(QStringLiteral("Comment"), m_commentFormat);
  if (m_syntaxErrorFormat.background().isOpaque()) {
    m_pStyleSet->setValue(
          QStringLiteral("SyntaxError"), "---" + sSEPARATOR + "---" +
          sSEPARATOR + "---" + sSEPARATOR + "0x" +
          m_syntaxErrorFormat.background().color().name().remove(
            QStringLiteral("#")));
  }
  m_pStyleSet->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::writeFormat(const QString &sKey,
                              const QTextCharFormat &charFormat) {
  // Foreground color
  QString sValue = "0x" + charFormat.foreground().color().name().remove(
                     QStringLiteral("#"));

  // Font weight
  if (QFont::Bold == charFormat.fontWeight()) {
    sValue += sSEPARATOR + "true";
  } else {
    sValue += sSEPARATOR + "false";
  }

  // Italic
  if (charFormat.fontItalic()) {
    sValue += sSEPARATOR + "true";
  } else {
    sValue += sSEPARATOR + "false";
  }

  // Background color
  if (charFormat.background().isOpaque()) {
    sValue += sSEPARATOR +
              "0x" + charFormat.background().color().name().remove(
                QStringLiteral("#"));
  }

  m_pStyleSet->setValue(sKey, sValue);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::getTranslations() {
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

    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.trimmed().isEmpty()) {
        tmpList = tmpLine.split(QStringLiteral("="));
        if (2 == tmpList.size()) {
          const QStringList tmpList2(tmpList[1].split(QStringLiteral(",")));
          for (const auto &s : tmpList2) {
            m_sListMacroKeywords << s.trimmed();

            if ("Template" == tmpList[0].trimmed() ||
                "Code" == tmpList[0].trimmed()) {
              m_sListParserKeywords << s.trimmed().toLower();
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

void Highlighter::loadHighlighting(const QString &sStyleFile) {
  this->readStyle(sStyleFile);

  // Background
  if (this->m_bSystemBackground) {
    m_pUi->styleTable->item(0, 0)->setText(QStringLiteral("System"));
  } else {
    m_pUi->styleTable->item(0, 0)->setText(
          this->m_colorBackground.name());
  }
  // Foreground
  if (this->m_bSystemForeground) {
    m_pUi->styleTable->item(1, 0)->setText(QStringLiteral("System"));
  } else {
    m_pUi->styleTable->item(1, 0)->setText(
          this->m_colorForeground.name());
  }

  this->readValue(2, this->m_textformatFormat);      // Text format
  this->readValue(3, this->m_headingsFormat);        // Heading
  this->readValue(4, this->m_linksFormat);           // Hyperlink
  this->readValue(5, this->m_interwikiLinksFormat);  // InterWiki
  this->readValue(6, this->m_macrosFormat);          // Macro
  this->readValue(7, this->m_parserFormat);          // Parser
  this->readValue(8, this->m_listFormat);            // List
  this->readValue(9, this->m_newTableLineFormat);    // Table line
  this->readValue(10, this->m_tablecellsFormat);     // Table cell
  this->readValue(11, this->m_imgMapFormat);         // Image map
  this->readValue(12, this->m_miscFormat);           // Misc
  this->readValue(13, this->m_commentFormat);        // Comment
  m_pUi->styleTable->item(14, 3)->setText(
        m_syntaxErrorFormat.background().color().name());  // Syntax error
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::readValue(const quint16 nRow,
                            const QTextCharFormat &charFormat) {
  // Foreground
  m_pUi->styleTable->item(nRow, 0)->setText(
        charFormat.foreground().color().name());
  // Bold
  if (charFormat.font().bold()) {
    m_pUi->styleTable->item(nRow, 1)->setCheckState(Qt::Checked);
  } else {
    m_pUi->styleTable->item(nRow, 1)->setCheckState(Qt::Unchecked);
  }
  // Italic
  if (charFormat.font().italic()) {
    m_pUi->styleTable->item(nRow, 2)->setCheckState(Qt::Checked);
  } else {
    m_pUi->styleTable->item(nRow, 2)->setCheckState(Qt::Unchecked);
  }
  // Background
  if (charFormat.background().color() != Qt::transparent) {
    m_pUi->styleTable->item(nRow, 3)->setText(
          charFormat.background().color().name());
  } else {
    m_pUi->styleTable->item(nRow, 3)->setText(QLatin1String(""));
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::clickedStyleCell(int nRow, int nCol) {
  if (0 == nCol || 3 == nCol) {
    QColor initialColor(m_pUi->styleTable->item(nRow, nCol)->text());
    QColor newColor = QColorDialog::getColor(initialColor);
    if (newColor.isValid()) {
      m_pUi->styleTable->item(nRow, nCol)->setText(newColor.name());
    } else if (newColor.name().isEmpty()) {
      m_pUi->styleTable->item(nRow, nCol)->setText(QLatin1String(""));
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::saveHighlighting() {
  m_sStyleFile = m_pUi->styleFilesBox->currentText();
  this->readStyle(m_pUi->styleFilesBox->currentText());

  // Background
  if ("system" == m_pUi->styleTable->item(0, 0)->text().toLower()) {
    m_bSystemBackground = true;
  } else {
    m_bSystemBackground = false;
    m_colorBackground.setNamedColor(m_pUi->styleTable->item(0, 0)->text());
  }
  // Foreground
  if ("system" == m_pUi->styleTable->item(1, 0)->text().toLower()) {
    m_bSystemForeground = true;
  } else {
    m_bSystemForeground = false;
    m_colorForeground.setNamedColor(m_pUi->styleTable->item(1, 0)->text());
  }

  m_textformatFormat = Highlighter::evalKey(this->createValues(2));
  m_headingsFormat = Highlighter::evalKey(this->createValues(3));
  m_linksFormat = Highlighter::evalKey(this->createValues(4));
  m_interwikiLinksFormat = Highlighter::evalKey(this->createValues(5));
  m_macrosFormat = Highlighter::evalKey(this->createValues(6));
  m_parserFormat = Highlighter::evalKey(this->createValues(7));
  m_listFormat = Highlighter::evalKey(this->createValues(8));
  m_newTableLineFormat = Highlighter::evalKey(this->createValues(9));
  m_tablecellsFormat = Highlighter::evalKey(this->createValues(10));
  m_imgMapFormat = Highlighter::evalKey(this->createValues(11));
  m_miscFormat = Highlighter::evalKey(this->createValues(12));
  m_commentFormat = Highlighter::evalKey(this->createValues(13));
  m_syntaxErrorFormat = Highlighter::evalKey(this->createValues(14));

  this->saveStyle();
  this->readStyle(m_pUi->styleFilesBox->currentText());

  m_pSettings->beginGroup("Plugin_" + QStringLiteral(PLUGIN_NAME));
  m_pSettings->setValue(QStringLiteral("Style"), m_sStyleFile);
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Highlighter::createValues(const quint16 nRow) -> QString {
  QString sReturn(QLatin1String(""));
  QString sTmp(QLatin1String(""));
  sTmp = m_pUi->styleTable->item(nRow, 0)->text();
  sTmp.remove(0, 1).push_front(QStringLiteral("0x"));
  sTmp.append(QLatin1String("|"));
  sReturn += sTmp;
  if (m_pUi->styleTable->item(nRow, 1)->checkState() == Qt::Checked) {
    sReturn += QLatin1String("true|");
  } else { sReturn += QLatin1String("false|"); }
  if (m_pUi->styleTable->item(nRow, 2)->checkState() == Qt::Checked) {
    sReturn += QLatin1String("true|");
  } else { sReturn += QLatin1String("false|"); }
  sTmp = m_pUi->styleTable->item(nRow, 3)->text();
  sTmp.remove(0, 1).push_front(QStringLiteral("0x"));
  sReturn += sTmp;
  return sReturn;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::changedStyle(int nIndex) {
  QString sFileName(QLatin1String(""));

  if (0 == nIndex) {  // Create new style
    bool bOk;
    QFileInfo fiStyle(m_pStyleSet->fileName());

    sFileName = QInputDialog::getText(nullptr, tr("New style"),
                                      tr("Please insert name of "
                                         "new style file:"),
                                      QLineEdit::Normal,
                                      QLatin1String(""),
                                      &bOk);
    if (!bOk || sFileName.isEmpty()) {
      // Reset selection
      m_pUi->styleFilesBox->setCurrentIndex(
            m_pUi->styleFilesBox->findText(fiStyle.baseName()));
      return;
    }

    sFileName = sFileName + "-style";
    QFile fileStyle(fiStyle.absolutePath() + "/" + sFileName + m_sExt);

    if (fileStyle.exists()) {
      // Reset selection
      m_pUi->styleFilesBox->setCurrentIndex(
            m_pUi->styleFilesBox->findText(fiStyle.baseName()));

      QMessageBox::warning(nullptr, tr("Error"), tr("File already exists."));
      qWarning() << "Style file already exists:" << fileStyle.fileName();
      return;
    }
    bOk = QFile::copy(fiStyle.absoluteFilePath(),
                      fileStyle.fileName());
    if (!bOk) {
      // Reset selection
      m_pUi->styleFilesBox->setCurrentIndex(
            m_pUi->styleFilesBox->findText(fiStyle.baseName()));

      QMessageBox::warning(nullptr, tr("Error"),
                           tr("Could not create new style."));
      qWarning() << "Could not create new style file:";
      qWarning() << "Org:" << fiStyle.absoluteFilePath();
      qWarning() << "Copy:" << fileStyle.fileName();
      return;
    }
    m_pUi->styleFilesBox->addItem(sFileName);
    m_pUi->styleFilesBox->setCurrentIndex(
          m_pUi->styleFilesBox->findText(sFileName));
  } else {  // Load existing style file
    sFileName = m_pUi->styleFilesBox->currentText();
  }

  loadHighlighting(sFileName);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::defineRules() {
  HighlightingRule rule;
  QStringList sListRegExpPatterns;
  QString sTmpRegExp;
  m_highlightingRules.clear();

  // Headings (= Heading =)
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  rule.format = m_headingsFormat;
  for (int i = 5; i > 0; i--) {
    rule.regexp = QRegularExpression("^\\s*={" + QString::number(i) +
                                     "}[^=]+={" + QString::number(i) +
                                     "}\\s*$");
    m_highlightingRules.append(rule);
  }

  // Links - everything between [...]
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  rule.format = m_linksFormat;
  rule.regexp = QRegularExpression(QStringLiteral("\\[{1,1}.+\\]{1,1}"));
  m_highlightingRules.append(rule);

  // Cell style in tables
  rule.format = m_tablecellsFormat;
  rule.regexp = QRegularExpression(
                  QStringLiteral("^[<]{1,1}.*[>]{1,1}"),
                  QRegularExpression::MultilineOption |
                  QRegularExpression::DotMatchesEverythingOption);
  m_highlightingRules.append(rule);
  rule.regexp = QRegularExpression(
                  QStringLiteral("[|][|] *[<]{1,1}.*[>]{1,1}"),
                  QRegularExpression::DotMatchesEverythingOption);
  m_highlightingRules.append(rule);

  // New table line
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  rule.format = m_newTableLineFormat;
  rule.regexp = QRegularExpression(QStringLiteral("^\\+{3}$"));
  m_highlightingRules.append(rule);
  rule.regexp = QRegularExpression(QStringLiteral("\\|\\|"));
  m_highlightingRules.append(rule);

  // Image map elements (flags, smilies, etc.)
  const QList<QString> flagMap(m_pTemplates->getFlagMap().keys());
  const QStringList smiliesTxtMap(m_pTemplates->getSmiliesTxtMap().first);
  sListRegExpPatterns.clear();
  sListRegExpPatterns.reserve(flagMap.size() + smiliesTxtMap.size());
  for (const auto &tmpStr : flagMap) {
    sListRegExpPatterns << QRegularExpression::escape(tmpStr);
  }
  // sListRegExpPatterns << "\\{([a-z]{2}|[A-Z]{2})\\}";  // Flags
  for (const auto &tmpStr : smiliesTxtMap) {
    sListRegExpPatterns << QRegularExpression::escape(tmpStr);
  }
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  for (const auto &sPattern : qAsConst(sListRegExpPatterns)) {
    rule.format = m_imgMapFormat;
    rule.regexp = QRegularExpression(sPattern);
    m_highlightingRules.append(rule);
  }

  // InterWiki-Links
  const QList<QString> iwlKeys(m_pTemplates->getIwlMap().keys());
  sListRegExpPatterns.clear();
  sListRegExpPatterns.reserve(iwlKeys.size());
  for (const auto &tmpStr : iwlKeys) {
    sListRegExpPatterns << "\\[{1,1}\\b" + tmpStr + "\\b:.+\\]{1,1}";
  }
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  for (const auto &sPattern : qAsConst(sListRegExpPatterns)) {
    rule.format = m_interwikiLinksFormat;
    rule.regexp = QRegularExpression(sPattern);
    m_highlightingRules.append(rule);
  }

  // Macros ([[Vorlage(...) etc.)
  sListRegExpPatterns.clear();
  sListRegExpPatterns.reserve(m_sListMacroKeywords.size() + 1);
  for (const auto &tmpStr : qAsConst(m_sListMacroKeywords)) {
    sListRegExpPatterns << "\\[\\[" + tmpStr + "\\ *\\(";
  }
  sListRegExpPatterns << QStringLiteral("\\)\\]\\]");
  rule.regexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  for (const auto &sPattern : qAsConst(sListRegExpPatterns)) {
    rule.format = m_macrosFormat;
    rule.regexp.setPattern(sPattern);
    m_highlightingRules.append(rule);
  }

  // Parser ({{{#!code etc.)
  sListRegExpPatterns.clear();
  sListRegExpPatterns.reserve(m_sListParserKeywords.size() + 2);
  for (const auto &tmpStr : qAsConst(m_sListParserKeywords)) {
    sListRegExpPatterns << QRegularExpression::escape("{{{#!" + tmpStr);
  }
  sListRegExpPatterns << QRegularExpression::escape(QStringLiteral("{{{")) <<
                         QRegularExpression::escape(QStringLiteral("}}}"));
  rule.regexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  for (const auto &sPattern : qAsConst(sListRegExpPatterns)) {
    rule.format = m_parserFormat;
    rule.regexp.setPattern(sPattern);
    m_highlightingRules.append(rule);
  }

  // Define textformat keywords (bold, italic, etc.)
  sListRegExpPatterns.clear();
  sListRegExpPatterns.reserve(m_pTemplates->getListFormatStart().size() +
                              m_pTemplates->getListFormatEnd().size());
  sListRegExpPatterns.append(m_pTemplates->getListFormatStart());
  sListRegExpPatterns.append(m_pTemplates->getListFormatEnd());
  sListRegExpPatterns.removeDuplicates();
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  for (auto sPattern : qAsConst(sListRegExpPatterns)) {
    rule.format = m_textformatFormat;
    if (sPattern.startsWith(QLatin1String("RegExp="))) {
      sTmpRegExp = sPattern.remove(QStringLiteral("RegExp="));
    } else {
      sTmpRegExp = QRegularExpression::escape(sPattern);
    }
    rule.regexp = QRegularExpression(sTmpRegExp);
    m_highlightingRules.append(rule);
  }

  // Comments (## comment)
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  rule.format = m_commentFormat;
  rule.regexp = QRegularExpression(QStringLiteral("^##.*$"));
  m_highlightingRules.append(rule);

  // List
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  rule.format = m_listFormat;
  rule.regexp = QRegularExpression(QStringLiteral("^\\s+\\*\\s+"));
  m_highlightingRules.append(rule);
  rule.format = m_listFormat;
  rule.regexp = QRegularExpression(QStringLiteral("^\\s+[1aAiI]\\.\\s+"));
  m_highlightingRules.append(rule);

  // Misc
  sListRegExpPatterns.clear();
  sListRegExpPatterns.reserve(2);
  sListRegExpPatterns << QRegularExpression::escape(QStringLiteral("[[BR]]")) <<
                         QRegularExpression::escape(QStringLiteral("\\\\"));
  rule.regexp.setPatternOptions(QRegularExpression::NoPatternOption);
  for (const auto &sPattern : qAsConst(sListRegExpPatterns)) {
    rule.regexp = QRegularExpression(sPattern);
    rule.format = m_miscFormat;
    m_highlightingRules.append(rule);
  }
  rule.format = m_miscFormat;
  rule.regexp = QRegularExpression(QStringLiteral("^# *tag:"));
  m_highlightingRules.append(rule);
  rule.regexp = QRegularExpression(QStringLiteral("^----$"));
  m_highlightingRules.append(rule);
  rule.regexp = QRegularExpression(QStringLiteral("^>+"));
  m_highlightingRules.append(rule);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::accept() {
  this->saveHighlighting();
  this->defineRules();
  this->rehighlightAll();

  m_pDialog->done(QDialog::Accepted);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Highlighter::hasSettings() const -> bool {
  return true;
}

void Highlighter::showSettings() {
  m_pDialog->show();
  m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::setCurrentEditor(TextEditor *pEditor) {
  Q_UNUSED(pEditor)
}

void Highlighter::setEditorlist(const QList<TextEditor *> &listEditors) {
  m_ListHighlighters.clear();

  for (auto *pEd : listEditors) {
    if (!m_listEditors.contains(pEd)) {
      m_listEditors << pEd;
    }
  }
  for (auto *pEd : qAsConst(m_listEditors)) {
    if (!listEditors.contains(pEd)) {
      m_listEditors.removeOne(pEd);
    } else {
      m_ListHighlighters << new SyntaxHighlighter(pEd->document());
    }
  }

  this->rehighlightAll();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::rehighlightAll() {
  for (auto *hlight : qAsConst(m_ListHighlighters)) {
    hlight->setRules(m_highlightingRules);
    hlight->rehighlight();
  }

  QPalette pal;
  pal.setColor(QPalette::Base, m_colorBackground);
  pal.setColor(QPalette::Text, m_colorForeground);
  for (auto *editor : qAsConst(m_listEditors)) {
    editor->setPalette(pal);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  // aboutbox.setIconPixmap(QPixmap(":/highlighter.png"));
  aboutbox.setText(QString::fromLatin1("<p><b>%1</b><br />"
                                       "%2</p>"
                                       "<p>%3<br />"
                                       "%4</p>"
                                       "<p><i>%5</i></p>")
                   .arg(this->getCaption(),
                        tr("Version") + ": " + PLUGIN_VERSION,
                        PLUGIN_COPY,
                        tr("Licence") + ": " +
                        "<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a>",
                        tr("Inyoka markup syntax highlighter plugin.")));
  aboutbox.exec();
}
