/**
 * \file uu_tabletemplate.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2012-2022 The InyokaEdit developers
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
 * Shows a modal window for ubuntuusers.de table templates.
 */

#include "./uu_tabletemplate.h"

#include <QApplication>
#include <QDebug>
#include <QTextDocument>
#include <QMessageBox>
#include <QSettings>

#include "../../application/parser/parser.h"
#include "../../application/templates/templates.h"
#include "../../application/texteditor.h"

#include "ui_uu_tabletemplate.h"

void Uu_TableTemplate::initPlugin(QWidget *pParent, TextEditor *pEditor,
                                  const QDir &userDataDir,
                                  const QString &sSharePath) {
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  m_pParent = pParent;
  m_pEditor = pEditor;
  m_dirPreview = userDataDir;
  m_pTextDocument = new QTextDocument(this);
  m_sSharePath = sSharePath;
  m_pTemplates = new Templates(
                   m_pSettings->value(QStringLiteral("Inyoka/Community"),
                                      "ubuntuusers_de").toString(),
                   m_sSharePath, m_dirPreview.absolutePath());
  m_pParser = new Parser(m_sSharePath, QDir(QLatin1String("")),
                         QLatin1String(""), false, m_pTemplates,
                         m_pSettings->value(QStringLiteral("Inyoka/Community"),
                                            "ubuntuusers_de").toString(),
                         m_pSettings->value(QStringLiteral("Pygmentize"),
                                            "").toString());

  // Build UI
  m_pDialog = new QDialog(m_pParent);
  m_pUi = new Ui::Uu_TableTemplateClass();
  m_pUi->setupUi(m_pDialog);
  m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                            & ~Qt::WindowContextHelpButtonHint);
  m_pDialog->setModal(true);
  m_pDialog->setWindowIcon(this->getIcon());
  m_pUi->tabWidget->setCurrentIndex(0);  // Load tab "generator" at first start

#ifdef USEQTWEBKIT
  m_pPreviewWebview = new QWebView();
#endif
#ifdef USEQTWEBENGINE
  m_pPreviewWebview = new QWebEngineView();
#endif

#ifndef NOPREVIEW
  m_pUi->generatorTab->layout()->addWidget(m_pPreviewWebview);
  m_pPreviewWebview->setContextMenuPolicy(Qt::NoContextMenu);
  m_pPreviewWebview->setAcceptDrops(false);
  m_pPreviewWebview->setUrl(QUrl(QStringLiteral("about:blank")));
#else
  m_pUi->previewButton->setVisible(false);
#endif

  // Remove old obsolete conf entry
  m_pSettings->beginGroup(QStringLiteral("Plugin_tabletemplate"));
  m_pSettings->remove(QLatin1String(""));
  m_pSettings->endGroup();

  // Load table styles
  m_pSettings->beginGroup("Plugin_" + QStringLiteral(PLUGIN_NAME));
  m_sListTableClasses << QStringLiteral("zebra") <<
                         QStringLiteral("zebra_start2") <<
                         QStringLiteral("zebra_start3");
  m_sListTableClasses = m_pSettings->value(QStringLiteral("TableClasses"),
                                           m_sListTableClasses).toStringList();
  m_sListTableStyles << QStringLiteral("Human") << QStringLiteral("KDE") <<
                        QStringLiteral("Xfce") << QStringLiteral("Edubuntu") <<
                        QStringLiteral("Ubuntu Studio") <<
                        QStringLiteral("Lubuntu");
  m_sListTableStyles = m_pSettings->value(QStringLiteral("TableStyles"),
                                          m_sListTableStyles).toStringList();
  m_sListTableStylesPrefix << QLatin1String("") << QStringLiteral("kde-") <<
                              QStringLiteral("xfce-") <<
                              QStringLiteral("edu-") <<
                              QStringLiteral("studio-") <<
                              QStringLiteral("lxde-");
  m_sListTableStylesPrefix = m_pSettings->value(
                               QStringLiteral("TableStylesPrefix"),
                               m_sListTableStylesPrefix).toStringList();
  m_sRowClassTitle = m_pSettings->value(QStringLiteral("RowClassTitle"),
                                        "titel").toString();
  m_sRowClassHead = m_pSettings->value(QStringLiteral("RowClassHead"),
                                       "kopf").toString();
  m_sRowClassHighlight = m_pSettings->value(
                           QStringLiteral("RowClassHighlight"),
                           "highlight").toString();

  m_pSettings->setValue(QStringLiteral("TableClasses"), m_sListTableClasses);
  m_pSettings->setValue(QStringLiteral("TableStyles"), m_sListTableStyles);
  m_pSettings->setValue(QStringLiteral("TableStylesPrefix"),
                        m_sListTableStylesPrefix);
  m_pSettings->setValue(QStringLiteral("RowClassTitle"), m_sRowClassTitle);
  m_pSettings->setValue(QStringLiteral("RowClassHead"), m_sRowClassHead);
  m_pSettings->setValue(QStringLiteral("RowClassHighlight"),
                        m_sRowClassHighlight);
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
  connect(m_pUi->BaseToNewButton, &QPushButton::pressed,
          this, &Uu_TableTemplate::convertToNewTemplate);
  connect(m_pUi->NewToBaseButton, &QPushButton::pressed,
          this, &Uu_TableTemplate::convertToBaseTemplate);
#ifndef NOPREVIEW
  connect(m_pUi->previewButton, &QPushButton::pressed,
          this, &Uu_TableTemplate::preview);
#endif
  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted,
          this, &Uu_TableTemplate::accept);
  connect(m_pUi->buttonBox, &QDialogButtonBox::rejected,
          m_pDialog, &QDialog::reject);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Uu_TableTemplate::getPluginName() const -> QString {
  return QStringLiteral(PLUGIN_NAME);
}

auto Uu_TableTemplate::getPluginVersion() const -> QString {
  return QStringLiteral(PLUGIN_VERSION);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::installTranslator(const QString &sLang) {
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
                    QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
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

auto Uu_TableTemplate::getCaption() const -> QString {
  return tr("Ubuntuusers.de table generator");
}
auto Uu_TableTemplate::getIcon() const -> QIcon {
  if (m_pParent->window()->palette().window().color().lightnessF() < 0.5) {
    return QIcon(QLatin1String(":/tabletemplate_dark.png"));
  }
  return QIcon(QLatin1String(":/tabletemplate.png"));
}

auto Uu_TableTemplate::includeMenu() const -> bool {
  return true;
}
auto Uu_TableTemplate::includeToolbar() const -> bool {
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
#ifndef NOPREVIEW
  m_pPreviewWebview->setHtml(QLatin1String(""));
#endif
  m_pUi->baseTextEdit->clear();
  m_pUi->newTextEdit->clear();
  if (m_pEditor->textCursor().selectedText().startsWith(
        QLatin1String("{{{#!"))) {
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

#ifndef NOPREVIEW
void Uu_TableTemplate::preview() {
  m_pTextDocument->setPlainText(this->generateTable());

  QString sRetHtml(m_pParser->genOutput(QLatin1String(""), m_pTextDocument));
  // Remove for preview useless elements
  sRetHtml.remove(
        QRegularExpression(QStringLiteral("<h1 class=\"pagetitle\">.*</h1>"),
                           QRegularExpression::DotMatchesEverythingOption));
  sRetHtml.remove(
        QRegularExpression(QStringLiteral("<p class=\"meta\">.*</p>"),
                           QRegularExpression::DotMatchesEverythingOption));
  sRetHtml.replace(QLatin1String("</style>"),
                   QLatin1String("#page table{margin:0px;}</style>"));

  m_pPreviewWebview->setHtml(sRetHtml,
                             QUrl::fromLocalFile(m_dirPreview.absolutePath()
                                                 + "/"));
}
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Uu_TableTemplate::generateTable() -> QString {
  QString sTab(QStringLiteral("{{{#!vorlage Tabelle\n"));
  QString sTableClass(QLatin1String(""));
  int colsNum = m_pUi->colsNum->value();
  int rowsNum = m_pUi->rowsNum->value();

  if (m_pUi->HighlightSecondBox->isChecked() &&
      // Use only, if default style is used; otherwise highlight every second.
      0 == m_pUi->tableStyleBox->currentIndex()) {
    if (m_pUi->showTitleBox->isChecked() && m_pUi->showHeadBox->isChecked()) {
      sTableClass = "tableclass=\"" + m_sListTableClasses[2] + "\" ";
    } else if ((m_pUi->showTitleBox->isChecked() &&
               !m_pUi->showHeadBox->isChecked()) ||
               (!m_pUi->showTitleBox->isChecked() &&
                m_pUi->showHeadBox->isChecked())) {
      sTableClass = "tableclass=\"" + m_sListTableClasses[1] + "\" ";
    } else {
      sTab += "<tableclass=\"" + m_sListTableClasses[0] + "\">";
    }
  }

  // Create title if set
  if (m_pUi->showTitleBox->isChecked()) {
    sTab += QString("<" + sTableClass + "rowclass=\"%1%2\"-%3> %4\n+++\n")
            .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()],
        m_sRowClassTitle, QString::number(colsNum), tr("Title"));
  }

  // Create head if set
  if (m_pUi->showHeadBox->isChecked()) {
    if (m_pUi->showTitleBox->isChecked()) {
      sTab += QStringLiteral("<rowclass=\"%1%2\"> ")
              .arg(
                m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()],
          m_sRowClassHead);
    } else {
      sTab += QString("<" + sTableClass + "rowclass=\"%1%2\"> ")
              .arg(
                m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()],
          m_sRowClassHead);
    }

    for (int i = 0; i < colsNum; i++) {
      sTab += QString(tr("Head") + " %1 \n").arg(i + 1);
    }

    sTab += QLatin1String("+++\n");
  }

  // Create body
  for (int i = 0; i < rowsNum; i++) {
    if (m_pUi->HighlightSecondBox->isChecked() &&
        // Use only, if non-default style is used; otherwiese use "zebra" class.
        0 != m_pUi->tableStyleBox->currentIndex() &&
        1 == i % 2) {
      sTab += QStringLiteral("<rowclass=\"%1%2\">").arg(
                m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()],
          m_sRowClassHighlight);
    }
    for (int j = 0; j < colsNum; j++) {
      sTab += QLatin1String("\n");
    }
    if (i != rowsNum-1) {
      sTab += QLatin1String("+++\n");
    }
  }

  sTab += QLatin1String("}}}\n");
  return sTab;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::convertToBaseTemplate() {
  m_bBaseToNew = false;
  QString sTableCode(QLatin1String(""));
  QString sInput(m_pUi->newTextEdit->toPlainText());
  QStringList sListInput;
  QStringList sListRow;

  sInput.remove(QStringLiteral("{{{#!vorlage Tabelle"));
  sInput = sInput.trimmed();
  if (sInput.endsWith(QLatin1String("}}}"))) {
    sInput.remove(sInput.length() - 3, 3);
  }

  sListInput << sInput.split(QStringLiteral("+++"));
  for (int i = 0; i < sListInput.size(); i++) {
    sListInput[i] = sListInput[i].trimmed() + " ";

    sListRow.clear();
    sListRow << sListInput[i].split(QStringLiteral("\n"));
    for (int j = 0; j < sListRow.size(); j++) {
      sTableCode += "|| " + sListRow[j].trimmed() + " ";
    }

    if (i < sListInput.size()) {
      sTableCode += QLatin1String("||\n");
    }
  }

  m_pUi->baseTextEdit->setPlainText(sTableCode);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::convertToNewTemplate() {
  m_bBaseToNew = true;
  QString sTableCode(QStringLiteral("{{{#!vorlage Tabelle"));
  QStringList sListInput;
  QStringList sListRow;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  sListInput << m_pUi->baseTextEdit->toPlainText().split(
                  QRegularExpression(QStringLiteral("\\|\\|\\s*\\n")),
                  QString::SkipEmptyParts);
#else
  sListInput << m_pUi->baseTextEdit->toPlainText().split(
                  QRegularExpression(QStringLiteral("\\|\\|\\s*\\n")),
                  Qt::SkipEmptyParts);
#endif

  for (int i = 0; i < sListInput.size(); i++) {
    sListInput[i] = sListInput[i].trimmed();
    if (sListInput[i].startsWith(QLatin1String("||"))) {
      sListInput[i].remove(0, 2);
    }
    if (sListInput[i].endsWith(QLatin1String("||"))) {
      sListInput[i].remove(sListInput[i].length() - 2, 2);
    }

    sListRow.clear();
    sListRow << sListInput[i].split(QStringLiteral("||"));
    for (int j = 0; j < sListRow.size(); j++) {
      sTableCode += "\n" + sListRow[j].trimmed();
    }

    if (i < sListInput.size() - 1) {
      sTableCode += QLatin1String("\n+++");
    }
  }
  sTableCode += QLatin1String("\n}}}\n");

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

auto Uu_TableTemplate::hasSettings() const -> bool {
  return false;
}

void Uu_TableTemplate::showSettings() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void Uu_TableTemplate::setEditorlist(const QList<TextEditor *> &listEditors) {
  Q_UNUSED(listEditors)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_TableTemplate::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  aboutbox.setIconPixmap(QPixmap(QStringLiteral(":/tabletemplate.png")));
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
                        tr("Plugin for generating ubuntuusers.de styled "
                           "Inyoka tables.")));
  aboutbox.exec();
}
