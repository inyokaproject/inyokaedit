/**
 * \file CTableTemplate.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2014 The InyokaEdit developers
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
 * Shows a modal window with table template.
 */

#include <QDebug>
#include <QTextDocument>

#include "./CTableTemplate.h"
#include "ui_CTableTemplate.h"


void CTableTemplate::initPlugin(QWidget *pParent, CTextEditor *pEditor,
                                const QDir userDataDir) {
    qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined _WIN32
    m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                qApp->applicationName().toLower(),
                                PLUGIN_NAME);
#else
    m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                qApp->applicationName().toLower(),
                                PLUGIN_NAME);
#endif

    m_pEditor = pEditor;
    m_dirPreview = userDataDir;
    m_pTextDocument = new QTextDocument(this);

    // Build UI
    m_pDialog = new QDialog(pParent);
    m_pUi = new Ui::CTableTemplateClass();
    m_pUi->setupUi(m_pDialog);
    m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                              & ~Qt::WindowContextHelpButtonHint);
    m_pDialog->setModal(true);

    // Load table styles
    m_sListTableStyles << "Human" << "KDE" << "Xfce" << "Edubuntu"
                       << "Ubuntu Studio" << "Lubuntu";
    m_sListTableStyles = m_pSettings->value("TableStyles",
                                            m_sListTableStyles).toStringList();
    m_sListTableStylesPrefix << "" << "kde-" << "xfce-" << "edu-"
                             << "studio-" << "lxde-";
    m_sListTableStylesPrefix = m_pSettings->value("TableStylesPrefix",
                                                  m_sListTableStylesPrefix).toStringList();
    m_sRowClassTitle = m_pSettings->value("RowClassTitle", "titel").toString();
    m_sRowClassHead = m_pSettings->value("RowClassHead", "kopf").toString();
    m_sRowClassHighlight = m_pSettings->value("RowClassHighlight", "highlight").toString();

    m_pSettings->setValue("TableStyles", m_sListTableStyles);
    m_pSettings->setValue("TableStylesPrefix", m_sListTableStylesPrefix);
    m_pSettings->setValue("RowClassTitle", m_sRowClassTitle);
    m_pSettings->setValue("RowClassHead", m_sRowClassHead);
    m_pSettings->setValue("RowClassHighlight", m_sRowClassHighlight);

    if (m_sListTableStyles.size() != m_sListTableStylesPrefix.size()) {
        qWarning() << "Different size: TableStyles size =" << m_sListTableStyles.size()
                   << "  TableStylesPrefix size =" << m_sListTableStylesPrefix.size();
    } else {
        m_pUi->tableStyleBox->addItems(m_sListTableStyles);
    }

    connect(m_pUi->previewButton, SIGNAL(pressed()),
            this, SLOT(preview()));
    connect(m_pUi->buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
}

// ----------------------------------------------------------------------------

QTranslator* CTableTemplate::getPluginTranslator(const QString &sLocale) {
    QTranslator* pPluginTranslator = new QTranslator(this);
    QString sLocaleFile = QString(PLUGIN_NAME) + "_" + sLocale;
    if (!pPluginTranslator->load(sLocaleFile,
                                 "/usr/share/" +qAppName().toLower() + "/lang")) {
        // If it fails search in application dircetory
        if (!pPluginTranslator->load(sLocaleFile,
                                     qApp->applicationDirPath() + "/lang")) {
            qWarning() << "Could not load plugin translation:" << sLocaleFile;
            return NULL;
        }
    }
    return pPluginTranslator;
}

QString CTableTemplate::getMenuEntry() const {
    return trUtf8("Table generator");
}

QIcon CTableTemplate::getMenuIcon() const {
    return QIcon(":/tabletemplate.png");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTableTemplate::executePlugin() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi->tableStyleBox->setCurrentIndex(0);
    m_pUi->showHeadBox->setChecked(false);
    m_pUi->showTitleBox->setChecked(false);
    m_pUi->HighlightSecondBox->setChecked(false);
    m_pUi->colsNum->setValue(2);
    m_pUi->rowsNum->setValue(m_pUi->rowsNum->minimum());

    m_sTableString.clear();
    m_pUi->previewBox->setHtml("");
    m_pDialog->show();
    m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTableTemplate::preview() {
    qDebug() << "Calling" << Q_FUNC_INFO;
/*
    this->generateTable();
    m_pTextDocument->setPlainText(m_sTableString);

    QString sRetHtml(m_pParser->genOutput("", m_pTextDocument));
    // Remove for preview useless elements
    sRetHtml.remove(QRegExp("<h1 class=\"pagetitle\">.*</h1>"));
    sRetHtml.remove(QRegExp("<p class=\"meta\">.*</p>"));
    sRetHtml.replace("</style>", "#page table{margin:0px;}</style>");

    m_pUi->previewBox->setHtml(sRetHtml,
                               QUrl::fromLocalFile(m_dirPreview.absolutePath()
                                                   + "/"));
*/
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTableTemplate::generateTable() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    int colsNum = m_pUi->colsNum->value();
    int rowsNum = m_pUi->rowsNum->value();
/*
    m_sTableString = "{{{#!" +  m_pTemplates->getTransTemplate().toLower() + " "
                     + m_pTemplates->getTransTable() + "\n";
*/
    m_sTableString = "{{{#!vorlage Tabelle\n";


    // Create title if set
    if (m_pUi->showTitleBox->isChecked()) {
         m_sTableString += QString("<rowclass=\"%1%2\"-%3> %4\n+++\n")
                 .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
                 .arg(m_sRowClassTitle)
                 .arg(colsNum)
                 .arg(trUtf8("Title"));
    }

    // Create head if set
    if (m_pUi->showHeadBox->isChecked()) {
        m_sTableString += QString("<rowclass=\"%1%2\"> ")
                .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
                .arg(m_sRowClassHead);

        for (int i = 0; i < colsNum; i++) {
            m_sTableString += QString(trUtf8("Head") + " %1 \n").arg(i + 1);
        }

        m_sTableString += "+++\n";
    }

    // Create body
    for (int i = 0; i < rowsNum; i++) {
        if (m_pUi->HighlightSecondBox->isChecked() && 1 == i % 2) {
            m_sTableString += QString("<rowclass=\"%1%2\">")
                    .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
                    .arg(m_sRowClassHighlight);
        }
        for (int j = 0; j < colsNum; j++) {
            m_sTableString += "\n";
        }
        if (i != rowsNum-1) {
            m_sTableString += "+++\n";
        }
    }

    m_sTableString += "}}}\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTableTemplate::accept() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    this->generateTable();
    m_pEditor->insertPlainText(m_sTableString);
    m_pDialog->done(QDialog::Accepted);
}

#if QT_VERSION < 0x050000
    Q_EXPORT_PLUGIN2(tabletemplate, CTableTemplate)
#endif
