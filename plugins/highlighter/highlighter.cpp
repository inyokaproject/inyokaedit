/**
 * \file highlighter.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2018 The InyokaEdit developers
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
 * Syntax highlighting with builtin Qt functions.
 */

#include "./highlighter.h"

#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

#include "ui_highlighter.h"

const QString sSEPARATOR("|");

void Highlighter::initPlugin(QWidget *pParent, TextEditor *pEditor,
                             const QDir userDataDir,
                             const QString sSharePath) {
  Q_UNUSED(pEditor);
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined _WIN32
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
  m_sExt = ".ini";
#else
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
  m_sExt = ".conf";
#endif
  m_sSharePath = sSharePath;

  this->copyDefaultStyles();

  m_sStyleFile = "standard-style";
  // Check for old entry
  if (!m_pSettings->value("Style", "").toString().isEmpty()) {
    m_sStyleFile = m_pSettings->value("Style", "").toString();
  }
  m_pSettings->remove("Style");
  // New plugin entry
  m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  if (!m_pSettings->value("Style", "").toString().isEmpty()) {
    m_sStyleFile = m_pSettings->value("Style", "").toString();
  }
  m_pSettings->setValue("Style", m_sStyleFile);
  m_pSettings->endGroup();

  m_pStyleSet = NULL;
  m_pTemplates = new Templates(
                   m_pSettings->value(
                     "InyokaCommunity", "ubuntuusers_de").toString(),
                   m_sSharePath, userDataDir.absolutePath());
  this->getTranslations();
  this->readStyle(m_sStyleFile);
  this->defineRules();

  this->buildUi(pParent);  // After loading template entries
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Highlighter::getPluginName() const {
  return PLUGIN_NAME;
}

QString Highlighter::getPluginVersion() const {
  return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);
  if ("en" == sLang) {
    return;
  }

  if (!m_translator.load(":/" + QString(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QString(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
    if (!m_translator.load(QString(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" + QString(PLUGIN_NAME).toLower() +
                    "_" + sLang + ".qm";
      return;
    }
  }

  if (qApp->installTranslator(&m_translator)) {
    m_pUi->retranslateUi(m_pDialog);
  } else {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Highlighter::getCaption() const {
  return tr("Syntax highlighter");
}
QIcon Highlighter::getIcon() const {
  return QIcon();
  // return QIcon(":/highlighter.png");
}

bool Highlighter::includeMenu() const {
  return false;
}
bool Highlighter::includeToolbar() const {
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

  QFile stylefile(fi.absolutePath() + "/standard-style" + m_sExt);
  if (!stylefile.exists()) {
    if (!QFile::copy(":/standard-style.conf",
                     fi.absolutePath() + "/standard-style" + m_sExt)) {
      qWarning() << "Couldn't create style file: " << stylefile.fileName();
    }
  }

  stylefile.setFileName(fi.absolutePath() + "/dark-style" + m_sExt);
  if (!stylefile.exists()) {
    if (!QFile::copy(":/dark-style.conf",
                     fi.absolutePath() + "/dark-style" + m_sExt)) {
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
  m_pUi->styleTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

  QStringList sListStyleFiles;
  QFileInfo fiStyleFile(m_pSettings->fileName());
  QFileInfoList fiListFiles(fiStyleFile.absoluteDir().entryInfoList(
                              QDir::NoDotAndDotDot | QDir::Files));
  foreach (QFileInfo fi, fiListFiles) {
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

  connect(m_pUi->styleFilesBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(changedStyle(int)));
  connect(m_pUi->styleTable, SIGNAL(cellDoubleClicked(int, int)),
          this, SLOT(clickedStyleCell(int, int)));

  connect(m_pUi->buttonBox, SIGNAL(accepted()),
          this, SLOT(accept()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::readStyle(const QString &sStyle) {
  bool bOk = false;
  m_bSystemForeground = false;
  m_bSystemBackground = false;
  QString sTmpKey;
  QColor tmpColor;

  if (NULL != m_pStyleSet) {
    delete m_pStyleSet;
  }
  m_pStyleSet = NULL;

#if defined _WIN32
  m_pStyleSet = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(), sStyle);
#else
  m_pStyleSet = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(), sStyle);
#endif

  if (!QFile::exists(m_pStyleSet->fileName())) {
    qWarning() << "Could not find/open highlighting style file:" <<
                  m_pStyleSet->fileName();
  }

  m_colorForeground = QApplication::palette().color(QPalette::Text);
  sTmpKey = m_pStyleSet->value("Foreground", "System")
            .toString();
  if (sTmpKey.toLower() != "system") {
    tmpColor.setRgb(sTmpKey.toInt(&bOk, 16));
    if (bOk) {
      m_colorForeground = tmpColor;
    }
  } else {
    m_bSystemForeground = true;
  }

  m_colorBackground = QApplication::palette().color(QPalette::Base);
  sTmpKey = m_pStyleSet->value("Background", "System")
            .toString();
  if (sTmpKey.toLower() != "system") {
    tmpColor.setRgb(sTmpKey.toInt(&bOk, 16));
    if (bOk) {
      m_colorBackground = tmpColor;
    }
  } else {
    m_bSystemBackground = true;
  }

  m_pStyleSet->beginGroup("Style");
  sTmpKey = m_pStyleSet->value("Heading", "0x008000" + sSEPARATOR + "true")
            .toString();
  this->evalKey(sTmpKey, m_headingsFormat);
  sTmpKey = m_pStyleSet->value("Hyperlink", "0x000080").toString();
  this->evalKey(sTmpKey, m_linksFormat);
  sTmpKey = m_pStyleSet->value("InterWiki", "0x0000ff").toString();
  this->evalKey(sTmpKey, m_interwikiLinksFormat);
  sTmpKey = m_pStyleSet->value("Macro", "0x008080").toString();
  this->evalKey(sTmpKey, m_macrosFormat);
  sTmpKey = m_pStyleSet->value("Parser", "0x800000" + sSEPARATOR + "true")
            .toString();
  this->evalKey(sTmpKey, m_parserFormat);
  sTmpKey = m_pStyleSet->value("Comment", "0xa0a0a4").toString();
  this->evalKey(sTmpKey, m_commentFormat);
  sTmpKey = m_pStyleSet->value("ImgMap", "0x808000").toString();
  this->evalKey(sTmpKey, m_imgMapFormat);
  sTmpKey = m_pStyleSet->value("TableCellFormating", "0x800080").toString();
  this->evalKey(sTmpKey, m_tablecellsFormat);
  sTmpKey = m_pStyleSet->value("TextFormating", "0xff0000").toString();
  this->evalKey(sTmpKey, m_textformatFormat);
  sTmpKey = m_pStyleSet->value("List", "0xff0000").toString();
  this->evalKey(sTmpKey, m_listFormat);
  sTmpKey = m_pStyleSet->value("NewTableLine", "0xff0000").toString();
  this->evalKey(sTmpKey, m_newTableLineFormat);
  sTmpKey = m_pStyleSet->value("Misc", "0xff0000").toString();
  this->evalKey(sTmpKey, m_miscFormat);
  sTmpKey = m_pStyleSet->value(
              "SyntaxError", "---" + sSEPARATOR + "---" + sSEPARATOR + "---" +
              sSEPARATOR + "0xffff00").toString();
  this->evalKey(sTmpKey, m_syntaxErrorFormat);
  m_pStyleSet->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::evalKey(const QString &sKey, QTextCharFormat &charFormat) {
  bool bOk = false;
  QColor tmpColor;
  QBrush tmpBrush;

  // Set defaults
  tmpBrush.setColor(Qt::transparent);
  tmpBrush.setStyle(Qt::SolidPattern);
  charFormat.setBackground(tmpBrush);
  tmpBrush.setColor(Qt::black);
  tmpBrush.setStyle(Qt::SolidPattern);
  charFormat.setForeground(tmpBrush);
  charFormat.setFontWeight(QFont::Normal);
  charFormat.setFontItalic(false);

  QStringList sListTmp = sKey.split(sSEPARATOR);

  // Foreground color
  if (sListTmp.size() > 0) {
    if ("---" != sListTmp[0].trimmed()) {
      tmpColor.setRgb(sListTmp[0].trimmed().toInt(&bOk, 16));
      if (bOk) {
        tmpBrush.setColor(tmpColor);
        charFormat.setForeground(tmpBrush);
      }
    }
    // Font weight
    if (sListTmp.size() > 1) {
      if ("---" != sListTmp[1].trimmed()) {
        if ("true" == sListTmp[1].trimmed().toLower()) {
          charFormat.setFontWeight(QFont::Bold);
        }
      }
      // Italic
      if (sListTmp.size() > 2) {
        if ("---" != sListTmp[2].trimmed()) {
          if ("true" == sListTmp[2].trimmed().toLower()) {
            charFormat.setFontItalic(true);
          }
        }
        // Background
        if (sListTmp.size() > 3) {
          if ("---" != sListTmp[3].trimmed()) {
            tmpColor.setRgb(sListTmp[3].trimmed().toInt(&bOk, 16));
            if (bOk) {
              tmpBrush.setColor(tmpColor);
              charFormat.setBackground(tmpBrush);
            }
          }
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::saveStyle() {
  if (m_bSystemForeground) {
    m_pStyleSet->setValue("Foreground", "System");
  } else {
    m_pStyleSet->setValue("Foreground", "0x" + m_colorForeground.name()
                          .remove("#"));
  }
  if (m_bSystemBackground) {
    m_pStyleSet->setValue("Background", "System");
  } else {
    m_pStyleSet->setValue("Background", "0x" + m_colorBackground.name()
                          .remove("#"));
  }

  m_pStyleSet->beginGroup("Style");
  this->writeFormat("TextFormating", m_textformatFormat);
  this->writeFormat("Heading", m_headingsFormat);
  this->writeFormat("Hyperlink", m_linksFormat);
  this->writeFormat("InterWiki", m_interwikiLinksFormat);
  this->writeFormat("Macro", m_macrosFormat);
  this->writeFormat("Parser", m_parserFormat);
  this->writeFormat("List", m_listFormat);
  this->writeFormat("NewTableLine", m_newTableLineFormat);
  this->writeFormat("TableCellFormating", m_tablecellsFormat);
  this->writeFormat("ImgMap", m_imgMapFormat);
  this->writeFormat("Misc", m_miscFormat);
  this->writeFormat("Comment", m_commentFormat);
  if (m_syntaxErrorFormat.background().isOpaque()) {
    m_pStyleSet->setValue(
          "SyntaxError", "---" + sSEPARATOR + "---" + sSEPARATOR + "---" +
          sSEPARATOR + "0x" +
          m_syntaxErrorFormat.background().color().name().remove("#"));
  }
  m_pStyleSet->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::writeFormat(const QString &sKey,
                              const QTextCharFormat &charFormat) {
  // Foreground color
  QString sValue = "0x" + charFormat.foreground().color().name().remove("#");

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
              "0x" + charFormat.background().color().name().remove("#");
  }

  m_pStyleSet->setValue(sKey, sValue);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::getTranslations() {
  QFile fiMacros(":/macros.conf");
  if (!fiMacros.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open macros.conf";
    QMessageBox::warning(NULL, "Error",
                         "Could not open macros.conf");
  } else {
    QTextStream in(&fiMacros);
    in.setCodec("UTF-8");
    QString tmpLine("");
    QStringList tmpList;
    QStringList tmpList2;

    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.trimmed().isEmpty()) {
        tmpList = tmpLine.split("=");
        if (2 == tmpList.size()) {
          tmpList2 = tmpList[1].split(",");
          foreach (QString s, tmpList2) {
            m_sListMacroKeywords << s.trimmed();

            if ("Template" == tmpList[0].trimmed()) {
              m_sListParserKeywords << s.trimmed().toLower();
            } else if ("Code" == tmpList[0].trimmed()) {
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
    m_pUi->styleTable->item(0, 0)->setText("System");
  } else {
    m_pUi->styleTable->item(0, 0)->setText(
          this->m_colorBackground.name());
  }
  // Foreground
  if (this->m_bSystemForeground) {
    m_pUi->styleTable->item(1, 0)->setText("System");
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
    m_pUi->styleTable->item(nRow, 3)->setText("");
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::clickedStyleCell(int nRow, int nCol) {
  if (0 == nCol || 3 == nCol) {
    QColorDialog colorDialog;
    QColor initialColor(m_pUi->styleTable->item(nRow, nCol)->text());
    QColor newColor = colorDialog.getColor(initialColor);
    if (newColor.isValid()) {
      m_pUi->styleTable->item(nRow, nCol)->setText(newColor.name());
    } else if (newColor.name().isEmpty()) {
      m_pUi->styleTable->item(nRow, nCol)->setText("");
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

  this->evalKey(this->createValues(2), m_textformatFormat);
  this->evalKey(this->createValues(3), m_headingsFormat);
  this->evalKey(this->createValues(4), m_linksFormat);
  this->evalKey(this->createValues(5), m_interwikiLinksFormat);
  this->evalKey(this->createValues(6), m_macrosFormat);
  this->evalKey(this->createValues(7), m_parserFormat);
  this->evalKey(this->createValues(8), m_listFormat);
  this->evalKey(this->createValues(9), m_newTableLineFormat);
  this->evalKey(this->createValues(10), m_tablecellsFormat);
  this->evalKey(this->createValues(11), m_imgMapFormat);
  this->evalKey(this->createValues(12), m_miscFormat);
  this->evalKey(this->createValues(13), m_commentFormat);
  this->evalKey(this->createValues(14), m_syntaxErrorFormat);

  this->saveStyle();
  this->readStyle(m_pUi->styleFilesBox->currentText());

  m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_pSettings->setValue("Style", m_sStyleFile);
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Highlighter::createValues(const quint16 nRow) {
  QString sReturn("");
  QString sTmp("");
  sTmp = m_pUi->styleTable->item(nRow, 0)->text();
  sTmp.remove(0, 1).push_front("0x");
  sTmp.append("|");
  sReturn += sTmp;
  if (m_pUi->styleTable->item(nRow, 1)->checkState() == Qt::Checked) {
    sReturn += "true|";
  } else { sReturn += "false|"; }
  if (m_pUi->styleTable->item(nRow, 2)->checkState() == Qt::Checked) {
    sReturn += "true|";
  } else { sReturn += "false|"; }
  sTmp = m_pUi->styleTable->item(nRow, 3)->text();
  sTmp.remove(0, 1).push_front("0x");
  sReturn += sTmp;
  return sReturn;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::changedStyle(int nIndex) {
  QString sFileName("");

  if (0 == nIndex) {  // Create new style
    bool bOk;
    QFileInfo fiStyle(m_pStyleSet->fileName());

    sFileName = QInputDialog::getText(0, tr("New style"),
                                      tr("Please insert name of "
                                         "new style file:"),
                                      QLineEdit::Normal,
                                      "",
                                      &bOk);
    // Click on "cancel" or string is empty
    if (true != bOk || sFileName.isEmpty()) {
      // Reset selection
      m_pUi->styleFilesBox->setCurrentIndex(
            m_pUi->styleFilesBox->findText(fiStyle.baseName()));
      return;
    } else {
      sFileName = sFileName + "-style";
      QFile fileStyle(fiStyle.absolutePath() + "/" + sFileName + m_sExt);

      if (fileStyle.exists()) {
        // Reset selection
        m_pUi->styleFilesBox->setCurrentIndex(
              m_pUi->styleFilesBox->findText(fiStyle.baseName()));

        QMessageBox::warning(0, tr("Error"), tr("File already exists."));
        qWarning() << "Style file already exists:" << fileStyle.fileName();
        return;
      }
      bOk = fileStyle.copy(fiStyle.absoluteFilePath(),
                           fileStyle.fileName());
      if (true != bOk) {
        // Reset selection
        m_pUi->styleFilesBox->setCurrentIndex(
              m_pUi->styleFilesBox->findText(fiStyle.baseName()));

        QMessageBox::warning(0, tr("Error"), tr("Could not create new style."));
        qWarning() << "Could not create new style file:";
        qWarning() << "Org:" << fiStyle.absoluteFilePath();
        qWarning() << "Copy:" << fileStyle.fileName();
        return;
      }
      m_pUi->styleFilesBox->addItem(sFileName);
      m_pUi->styleFilesBox->setCurrentIndex(
            m_pUi->styleFilesBox->findText(sFileName));
    }
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
  rule.format = m_headingsFormat;
  for (int i = 5; i > 0; i--) {
    rule.pattern = QRegExp("^\\s*={" + QString::number(i) + "}[^=]+={" +
                           QString::number(i) + "}\\s*$");
    m_highlightingRules.append(rule);
  }

  // Links - everything between [...]
  rule.format = m_linksFormat;
  rule.pattern = QRegExp("\\[{1,1}.+\\]{1,1}");
  m_highlightingRules.append(rule);

  // Cell style in tables
  rule.format = m_tablecellsFormat;
  rule.pattern = QRegExp("^\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("\\|\\|\\s*\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
  m_highlightingRules.append(rule);

  // New table line
  rule.format = m_newTableLineFormat;
  rule.pattern = QRegExp("^\\+{3}$");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("\\|\\|");
  m_highlightingRules.append(rule);

  // InterWiki-Links
  sListRegExpPatterns.clear();
  foreach (QString tmpStr, m_pTemplates->getListIWLs()) {
    sListRegExpPatterns << "\\[{1,1}\\b" + tmpStr + "\\b:.+\\]{1,1}";
  }
  foreach (const QString &sPattern, sListRegExpPatterns) {
    rule.format = m_interwikiLinksFormat;
    rule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
    m_highlightingRules.append(rule);
  }

  // Macros ([[Vorlage(...) etc.)
  sListRegExpPatterns.clear();
  foreach (QString tmpStr, m_sListMacroKeywords) {
    sListRegExpPatterns << QRegExp::escape("[[" + tmpStr + "(");
  }
  // Bad workaround for space between keyword and (
  foreach (QString tmpStr, m_sListMacroKeywords) {
    sListRegExpPatterns << QRegExp::escape("[[" + tmpStr + " (");
  }
  sListRegExpPatterns << QRegExp::escape(")]]");
  foreach (const QString &sPattern, sListRegExpPatterns) {
    rule.format = m_macrosFormat;
    rule.pattern = QRegExp(sPattern, Qt::CaseInsensitive);
    m_highlightingRules.append(rule);
  }

  // Parser ({{{#!code etc.)
  sListRegExpPatterns.clear();
  foreach (QString tmpStr, m_sListParserKeywords) {
    sListRegExpPatterns << QRegExp::escape("{{{#!" + tmpStr);
  }
  sListRegExpPatterns << QRegExp::escape("{{{") << QRegExp::escape("}}}");
  foreach (const QString &sPattern, sListRegExpPatterns) {
    rule.format = m_parserFormat;
    rule.pattern = QRegExp(sPattern, Qt::CaseInsensitive);
    m_highlightingRules.append(rule);
  }

  // Define textformat keywords (bold, italic, etc.)
  sListRegExpPatterns.clear();
  sListRegExpPatterns.append(m_pTemplates->getListFormatStart());
  sListRegExpPatterns.append(m_pTemplates->getListFormatEnd());
  sListRegExpPatterns.removeDuplicates();
  foreach (QString sPattern, sListRegExpPatterns) {
    rule.format = m_textformatFormat;
    if (sPattern.startsWith("RegExp=")) {
      sTmpRegExp = sPattern.remove("RegExp=");
    } else {
      sTmpRegExp = QRegExp::escape(sPattern);
    }
    rule.pattern = QRegExp(sTmpRegExp, Qt::CaseSensitive);
    m_highlightingRules.append(rule);
  }

  // Comments (## comment)
  rule.format = m_commentFormat;
  rule.pattern = QRegExp("^##.*$");
  m_highlightingRules.append(rule);

  // Image map elements (flags, smilies, etc.)
  sListRegExpPatterns.clear();
  foreach (QString tmpStr, m_pTemplates->getListFlags()) {
    sListRegExpPatterns << QRegExp::escape(tmpStr);
  }
  foreach (QString tmpStr, m_pTemplates->getListSmilies()) {
    sListRegExpPatterns << QRegExp::escape(tmpStr) + "\\s+";
  }
  foreach (const QString &sPattern, sListRegExpPatterns) {
    rule.format = m_imgMapFormat;
    rule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
    m_highlightingRules.append(rule);
  }

  // List
  rule.format = m_listFormat;
  rule.pattern = QRegExp("^\\s+\\*\\s+");
  m_highlightingRules.append(rule);
  rule.format = m_listFormat;
  rule.pattern = QRegExp("^\\s+1\\.\\s+");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^\\s+a\\.\\s+");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^\\s+A\\.\\s+");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^\\s+i\\.\\s+");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^\\s+I\\.\\s+");
  m_highlightingRules.append(rule);

  // Misc
  sListRegExpPatterns.clear();
  sListRegExpPatterns << QRegExp::escape("[[BR]]") << QRegExp::escape("\\\\");
  foreach (const QString &sPattern, sListRegExpPatterns) {
    rule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
    rule.format = m_miscFormat;
    m_highlightingRules.append(rule);
  }
  rule.format = m_miscFormat;
  rule.pattern = QRegExp("^#tag:");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^# tag:");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^----$");
  m_highlightingRules.append(rule);
  rule.pattern = QRegExp("^>+");
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

bool Highlighter::hasSettings() const {
  return true;
}

void Highlighter::showSettings() {
  m_pDialog->show();
  m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::setCurrentEditor(TextEditor *pEditor) {
  Q_UNUSED(pEditor);
}

void Highlighter::setEditorlist(QList<TextEditor *> listEditors) {
  m_ListHighlighters.clear();

  foreach (TextEditor *pEd, listEditors) {
    if (!m_listEditors.contains(pEd)) {
      m_listEditors << pEd;
    }
  }
  foreach (TextEditor *pEd, m_listEditors) {
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
  foreach (SyntaxHighlighter *hlight, m_ListHighlighters) {
    hlight->setRules(m_highlightingRules);
    hlight->rehighlight();
  }

  QPalette pal;
  pal.setColor(QPalette::Base, m_colorBackground);
  pal.setColor(QPalette::Text, m_colorForeground);
  foreach (TextEditor *editor, m_listEditors) {
    editor->setPalette(pal);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Highlighter::showAbout() {
  QMessageBox aboutbox(NULL);
  aboutbox.setWindowTitle(tr("Info"));
  // aboutbox.setIconPixmap(QPixmap(":/highlighter.png"));
  aboutbox.setText(QString("<p><b>%1</b><br />"
                           "%2</p>"
                           "<p>%3<br />"
                           "%4</p>"
                           "<p><i>%5</i></p>")
                   .arg(this->getCaption())
                   .arg(tr("Version") + ": " + PLUGIN_VERSION)
                   .arg(PLUGIN_COPY)
                   .arg(tr("Licence") + ": " +
                        "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a>")
                   .arg(tr("Inyoka markup syntax highlighter plugin.")));
  aboutbox.exec();
}
