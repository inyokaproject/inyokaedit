/**
 * \file uu_tabletemplate.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2012-2018 The InyokaEdit developers
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
 * Shows a modal window for ubuntuusers.de table templates.
 */

#include <QDebug>
#include <QTextDocument>

#include "./uu_tabletemplate.h"
#include "ui_uu_tabletemplate.h"

void Uu_TableTemplate::initPlugin(QWidget *pParent, TextEditor *pEditor,
                                  const QDir userDataDir,
                                  const QString sSharePath) {
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined _WIN32
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  m_pEditor = pEditor;
  m_dirPreview = userDataDir;
  m_pTextDocument = new QTextDocument(this);
  m_pTemplates = new Templates(
                   m_pSettings->value(
                     "InyokaCommunity", "ubuntuusers_de").toString(),
                   sSharePath, m_dirPreview.absolutePath());
  m_pParser = new Parser(sSharePath, QDir(""), "", false, m_pTemplates,
                         m_pSettings->value("InyokaCommunity",
                                            "ubuntuusers_de").toString());

  // Build UI
  m_pDialog = new QDialog(pParent);
  m_pUi = new Ui::Uu_TableTemplateClass();
  m_pUi->setupUi(m_pDialog);
  m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                            & ~Qt::WindowContextHelpButtonHint);
  m_pDialog->setModal(true);
  m_pUi->tabWidget->setCurrentIndex(0);  // Load tab "generator" at first start

#ifdef USEQTWEBKIT
  m_pPreviewWebview = new QWebView();
#else
  m_pPreviewWebview = new QWebEngineView();
#endif
  m_pUi->generatorTab->layout()->addWidget(m_pPreviewWebview);
  m_pPreviewWebview->setContextMenuPolicy(Qt::NoContextMenu);
  m_pPreviewWebview->setAcceptDrops(false);
  m_pPreviewWebview->setUrl(QUrl("about:blank"));

  // Remove old obsoleteconf entry
  m_pSettings->beginGroup("Plugin_tabletemplate");
  m_pSettings->remove("");
  m_pSettings->endGroup();

  // Load table styles
  m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_sListTableStyles << "Human" << "KDE" << "Xfce" << "Edubuntu"
                     << "Ubuntu Studio" << "Lubuntu";
  m_sListTableStyles = m_pSettings->value("TableStyles",
                                          m_sListTableStyles).toStringList();
  m_sListTableStylesPrefix << "" << "kde-" << "xfce-" << "edu-"
                           << "studio-" << "lxde-";
  m_sListTableStylesPrefix = m_pSettings->value(
                               "TableStylesPrefix",
                               m_sListTableStylesPrefix).toStringList();
  m_sRowClassTitle = m_pSettings->value("RowClassTitle", "titel").toString();
  m_sRowClassHead = m_pSettings->value("RowClassHead", "kopf").toString();
  m_sRowClassHighlight = m_pSettings->value(
                           "RowClassHighlight", "highlight").toString();

  m_pSettings->setValue("TableStyles", m_sListTableStyles);
  m_pSettings->setValue("TableStylesPrefix", m_sListTableStylesPrefix);
  m_pSettings->setValue("RowClassTitle", m_sRowClassTitle);
  m_pSettings->setValue("RowClassHead", m_sRowClassHead);
  m_pSettings->setValue("RowClassHighlight", m_sRowClassHighlight);
  m_pSettings->endGroup();

  if (m_sListTableStyles.size() != m_sListTableStylesPrefix.size()) {
    qWarning() << "Different size: TableStyles size ="
               << m_sListTableStyles.size()
               << "  TableStylesPrefix size ="
               << m_sListTableStylesPrefix.size();
  } else {
    m_pUi->tableStyleBox->addItems(m_sListTableStyles);
  }

  m_bBaseToNew = false;
  connect(m_pUi->BaseToNewButton, SIGNAL(pressed()),
          this, SLOT(convertToNewTemplate()));
  connect(m_pUi->NewToBaseButton, SIGNAL(pressed()),
          this, SLOT(convertToBaseTemplate()));
  connect(m_pUi->previewButton, SIGNAL(pressed()),
          this, SLOT(preview()));
  connect(m_pUi->buttonBox, SIGNAL(accepted()),
          this, SLOT(accept()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Uu_TableTemplate::getPluginName() const {
  return PLUGIN_NAME;
}

QString Uu_TableTemplate::getPluginVersion() const {
  return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QTranslator* Uu_TableTemplate::getPluginTranslator(const QString &sSharePath,
                                                   const QString &sLocale) {
  QTranslator* pPluginTranslator = new QTranslator(this);
  QString sLocaleFile = QString(PLUGIN_NAME) + "_" + sLocale;
  if (!pPluginTranslator->load(sLocaleFile, sSharePath + "/lang")) {
    qWarning() << "Could not load plugin translation:" << sLocaleFile;
    return NULL;
  }
  return pPluginTranslator;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Uu_TableTemplate::getCaption() const {
  return trUtf8("Ubuntuusers.de table generator");
}
QIcon Uu_TableTemplate::getIcon() const {
  return QIcon(":/tabletemplate.png");
}

bool Uu_TableTemplate::includeMenu() const {
  return true;
}
bool Uu_TableTemplate::includeToolbar() const {
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::callPlugin() {
  qDebug() << Q_FUNC_INFO;
  m_pUi->tableStyleBox->setCurrentIndex(0);
  m_pUi->showHeadBox->setChecked(false);
  m_pUi->showTitleBox->setChecked(false);
  m_pUi->HighlightSecondBox->setChecked(false);
  m_pUi->colsNum->setValue(2);
  m_pUi->rowsNum->setValue(m_pUi->rowsNum->minimum());
  m_pPreviewWebview->setHtml("");
  m_pUi->baseTextEdit->clear();
  m_pUi->newTextEdit->clear();
  if (m_pEditor->textCursor().selectedText().startsWith("{{{#!")) {
    m_pUi->newTextEdit->insertPlainText(
          m_pEditor->textCursor().selectedText());
  } else {
    m_pUi->baseTextEdit->insertPlainText(
          m_pEditor->textCursor().selectedText());
  }
  m_pDialog->show();
  m_pDialog->exec();
}

void Uu_TableTemplate::executePlugin() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::preview() {
  m_pTextDocument->setPlainText(this->generateTable());

  QString sRetHtml(m_pParser->genOutput("", m_pTextDocument));
  // Remove for preview useless elements
  sRetHtml.remove(QRegExp("<h1 class=\"pagetitle\">.*</h1>"));
  sRetHtml.remove(QRegExp("<p class=\"meta\">.*</p>"));
  sRetHtml.replace("</style>", "#page table{margin:0px;}</style>");

  m_pPreviewWebview->setHtml(sRetHtml,
                             QUrl::fromLocalFile(m_dirPreview.absolutePath()
                                                 + "/"));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Uu_TableTemplate::generateTable() {
  QString sTab("{{{#!vorlage Tabelle\n");
  int colsNum = m_pUi->colsNum->value();
  int rowsNum = m_pUi->rowsNum->value();

  // Create title if set
  if (m_pUi->showTitleBox->isChecked()) {
    sTab += QString("<rowclass=\"%1%2\"-%3> %4\n+++\n")
            .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
        .arg(m_sRowClassTitle)
        .arg(colsNum)
        .arg(trUtf8("Title"));
  }

  // Create head if set
  if (m_pUi->showHeadBox->isChecked()) {
    sTab += QString("<rowclass=\"%1%2\"> ")
            .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
        .arg(m_sRowClassHead);

    for (int i = 0; i < colsNum; i++) {
      sTab += QString(trUtf8("Head") + " %1 \n").arg(i + 1);
    }

    sTab += "+++\n";
  }

  // Create body
  for (int i = 0; i < rowsNum; i++) {
    if (m_pUi->HighlightSecondBox->isChecked() && 1 == i % 2) {
      sTab += QString("<rowclass=\"%1%2\">")
              .arg(
                m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
          .arg(m_sRowClassHighlight);
    }
    for (int j = 0; j < colsNum; j++) {
      sTab += "\n";
    }
    if (i != rowsNum-1) {
      sTab += "+++\n";
    }
  }

  sTab += "}}}\n";
  return sTab;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::convertToBaseTemplate() {
  m_bBaseToNew = false;
  QString sTableCode("");
  QString sInput(m_pUi->newTextEdit->toPlainText());
  QStringList sListInput;
  QStringList sListRow;

  sInput.remove("{{{#!vorlage Tabelle");
  sInput = sInput.trimmed();
  if (sInput.endsWith("}}}")) {
    sInput.remove(sInput.length() - 3, 3);
  }

  sListInput << sInput.split("+++");
  for (int i = 0; i < sListInput.size(); i++) {
    sListInput[i] = sListInput[i].trimmed() + " ";

    sListRow.clear();
    sListRow << sListInput[i].split("\n");
    for (int j = 0; j < sListRow.size(); j++) {
      sTableCode += "|| " + sListRow[j].trimmed() + " ";
    }

    if (i < sListInput.size()) {
      sTableCode += "||\n";
    }
  }

  m_pUi->baseTextEdit->setPlainText(sTableCode);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::convertToNewTemplate() {
  m_bBaseToNew = true;
  QString sTableCode("{{{#!vorlage Tabelle");
  QStringList sListInput;
  QStringList sListRow;

  sListInput << m_pUi->baseTextEdit->toPlainText().split(
                  QRegExp("\\|\\|\\s*\\n"), QString::SkipEmptyParts);

  for (int i = 0; i < sListInput.size(); i++) {
    sListInput[i] = sListInput[i].trimmed();
    if (sListInput[i].startsWith("||")) {
      sListInput[i].remove(0, 2);
    }
    if (sListInput[i].endsWith("||")) {
      sListInput[i].remove(sListInput[i].length() - 2, 2);
    }

    sListRow.clear();
    sListRow << sListInput[i].split("||");
    for (int j = 0; j < sListRow.size(); j++) {
      sTableCode += "\n" + sListRow[j].trimmed();
    }

    if (i < sListInput.size() - 1) {
      sTableCode += "\n+++";
    }
  }
  sTableCode += "\n}}}\n";

  m_pUi->newTextEdit->setPlainText(sTableCode);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::accept() {
  if (0 == m_pUi->tabWidget->currentIndex()) {
    m_pEditor->insertPlainText(this->generateTable());
  } else if (1 == m_pUi->tabWidget->currentIndex() && m_bBaseToNew) {
    m_pEditor->insertPlainText(m_pUi->newTextEdit->toPlainText());
  } else if (1 == m_pUi->tabWidget->currentIndex() && !m_bBaseToNew) {
    m_pEditor->insertPlainText(m_pUi->baseTextEdit->toPlainText());
  }
  m_pDialog->done(QDialog::Accepted);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool Uu_TableTemplate::hasSettings() const {
  return false;
}

void Uu_TableTemplate::showSettings() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void Uu_TableTemplate::setEditorlist(QList<TextEditor *> listEditors) {
  Q_UNUSED(listEditors);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::showAbout() {
  QMessageBox aboutbox(NULL);
  aboutbox.setWindowTitle(trUtf8("Info"));
  aboutbox.setIconPixmap(QPixmap(":/tabletemplate.png"));
  aboutbox.setText(QString("<p><b>%1</b><br />"
                           "%2</p>"
                           "<p>%3<br />"
                           "%4</p>"
                           "<p><i>%5</i></p>")
                   .arg(this->getCaption())
                   .arg(trUtf8("Version") + ": " + PLUGIN_VERSION)
                   .arg(PLUGIN_COPY)
                   .arg(trUtf8("Licence") + ": " +
                        "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a>")
                   .arg(trUtf8("Plugin for generating ubuntuusers.de styled "
                               "Inyoka tables.")));
  aboutbox.exec();
}
