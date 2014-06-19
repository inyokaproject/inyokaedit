/**
 * \file CTemplates.cpp
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
 * Loading and providing templates.
 */

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSettings>

#include "./CTemplates.h"

extern bool bDEBUG;

CTemplates::CTemplates(const QString &sTplLang)
    : m_sTplLang(sTplLang),
      m_sAppName(qApp->applicationName()),
      m_sAppPath(qApp->applicationDirPath()) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    this->initTemplates();
    this->initHtmlTpl("Preview.tpl");
    this->initImgMap("Flags.conf", m_sListFlags, m_sListFlagsImg);
    this->initImgMap("Smilies.conf", m_sListSmilies, m_sListSmiliesImg);
    this->initTextformats("Textformats.conf");
    this->initTranslations("Translations.conf");

    m_pInterWikiLinks = new CXmlParser(m_sAppName, m_sAppPath,
                                       "iWikiLinks/iWikiLinks.xml");
    m_pDropdownTemplates = new CXmlParser(m_sAppName, m_sAppPath,
                                          "templates/" + m_sTplLang
                                          + "/Templates_Dropdown.xml");
}

// Destructor
CTemplates::~CTemplates() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTemplates::initTemplates() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QFile TplFile("");
    QDir TplDir("");
    QString tmpLine("");
    QString sTempTplText("");
    QString sTempMacro("");
    bool bFoundMacro;
    bool bFoundTpl;

    // Path from normal installation
    if (TplDir.exists("/usr/share/" + m_sAppName.toLower()
                      + "/templates/" + m_sTplLang) && !bDEBUG) {
        TplDir.setPath("/usr/share/" +m_sAppName.toLower()
                       + "/templates/" + m_sTplLang);
    } else {  // No installation: Use app path
        TplDir.setPath(m_sAppPath + "/templates/" + m_sTplLang);
    }

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

    qDebug() << "Loaded templates:" << m_sListTplNamesINY;

    m_pMarkupTemplates = new CXmlParser(m_sAppName, m_sAppPath,
                                        "templates/" + m_sTplLang
                                        + "/Templates.xml");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTemplates::initHtmlTpl(const QString &sTplFile) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QFile HTMLTplFile(sTplFile);

    // Path from normal installation
    if (QFile::exists("/usr/share/" + m_sAppName.toLower() + "/templates/"
                      +  HTMLTplFile.fileName()) && !bDEBUG) {
        HTMLTplFile.setFileName("/usr/share/" + m_sAppName.toLower()
                                + "/templates/" + HTMLTplFile.fileName());
    } else {  // No installation: Use app path
        HTMLTplFile.setFileName(m_sAppPath + "/templates/"
                                + HTMLTplFile.fileName());
    }

    if (!HTMLTplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(0, "Warning", "Could not open preview template file!");
        qWarning() << "Could not open preview template file:"
                   << HTMLTplFile.fileName();
        m_sPreviewTemplate = "ERROR";
    } else {
        QTextStream in(& HTMLTplFile);
        m_sPreviewTemplate = in.readAll();

        HTMLTplFile.close();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTemplates::initImgMap(const QString &sFilename,
                            QStringList &sListElements,
                            QStringList &sListImgSource) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QFile ImgMapFile(sFilename);
    QStringList sListTmpLine;
    sListElements.clear();
    sListImgSource.clear();

    // Path from normal installation
    if (QFile::exists("/usr/share/" + m_sAppName.toLower() + "/templates/"
                      + ImgMapFile.fileName()) && !bDEBUG) {
        ImgMapFile.setFileName("/usr/share/" + m_sAppName.toLower()
                              + "/templates/" + ImgMapFile.fileName());
    } else {  // No installation: Use app path
        ImgMapFile.setFileName(m_sAppPath + "/templates/"
                              + ImgMapFile.fileName());
    }

    if (!ImgMapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(0, "Warning", "Could not open image map file!");
        qWarning() << "Could not open flag config file:"
                   << ImgMapFile.fileName();
        sListElements << "ERROR";
    } else {
        QTextStream in(&ImgMapFile);
        QString tmpLine;
        while (!in.atEnd()) {
            tmpLine = in.readLine().trimmed();
            if (!tmpLine.startsWith("#") && !tmpLine.trimmed().isEmpty()) {
                sListTmpLine = tmpLine.split("=");
                if (2 == sListTmpLine.size()) {
                    sListElements << sListTmpLine[0].trimmed();
                    sListImgSource << sListTmpLine[1].trimmed();
                }
            }
        }
        ImgMapFile.close();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTemplates::initTextformats(const QString &sFilename) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QFile formatsFile(sFilename);
    QStringList sListInput;

    // Path from normal installation
    if (QFile::exists("/usr/share/" + m_sAppName.toLower() + "/templates/"
                      + formatsFile.fileName()) && !bDEBUG) {
        formatsFile.setFileName("/usr/share/" + m_sAppName.toLower()
                                + "/templates/" + formatsFile.fileName());
    } else {  // No installation: Use app path
        formatsFile.setFileName(m_sAppPath + "/templates/"
                                + formatsFile.fileName());
    }

    if (!formatsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(0, "Warning", "Could not open text formats config file!");
        qWarning() << "Could not open text formats config file:"
                   << formatsFile.fileName();
        // Initialize possible text formats
        m_sListFormatStart << "ERROR";
        m_sListFormatEnd << "ERROR";
        m_sListFormatHtmlStart << "ERROR";
        m_sListFormatHtmlEnd << "ERROR";
    } else {
        QTextStream in(&formatsFile);
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

void CTemplates::initTranslations(const QString &sFilename) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QFile translFile;

    // Path from normal installation
    if (QFile::exists("/usr/share/" + m_sAppName.toLower() + "/templates/"
                      + m_sTplLang + "/" + sFilename) && !bDEBUG) {
        translFile.setFileName("/usr/share/" + m_sAppName.toLower()
                               + "/templates/" + m_sTplLang + "/" + sFilename);
    } else {  // No installation: Use app path
        translFile.setFileName(m_sAppPath + "/templates/" + m_sTplLang + "/"
                               + sFilename);
    }

    if (!translFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(0, "Error", "Could not open template translation file!");
        qCritical() << "Could not open template translation file:"
                    << translFile.fileName();
        exit(-1);
    }

    QSettings configTransl(translFile.fileName(), QSettings::IniFormat);
    configTransl.setIniCodec("UTF-8");

    m_sTransTemplate = configTransl.value("Template", "ERROR").toString();
    if ("ERROR" == m_sTransTemplate) {
        qCritical() << "Template translation not found.";
    }
    m_sTransTOC = configTransl.value("TableOfContents", "ERROR").toString();
    if ("ERROR" == m_sTransTOC) {
        qCritical() << "TOC translation not found.";
    }
    m_sTransImage = configTransl.value("Image", "ERROR").toString();
    if ("ERROR" == m_sTransImage) {
        qCritical() << "Image translation not found.";
    }
    m_sTransCodeBlock = configTransl.value("CodeBlock", "ERROR").toString();
    if ("ERROR" == m_sTransCodeBlock) {
        qCritical() << "Code block translation not found.";
    }
    m_sTransAttachment = configTransl.value("Attachment", "ERROR").toString();
    if ("ERROR" == m_sTransAttachment) {
        qCritical() << "Attachment translation not found.";
    }
    m_sTransAnchor = configTransl.value("Anchor", "ERROR").toString();
    if ("ERROR" == m_sTransAnchor) {
        qCritical() << "Anchor translation not found.";
    }
    m_sTransDate = configTransl.value("Date", "ERROR").toString();
    if ("ERROR" == m_sTransDate) {
        qCritical() << "Date translation not found.";
    }
    m_sTransRevText = configTransl.value("RevText", "ERROR").toString();
    if ("ERROR" == m_sTransRevText) {
        qCritical() << "Revision text translation not found.";
    }
    m_sTransTagText = configTransl.value("TagText", "ERROR").toString();
    if ("ERROR" == m_sTransTagText) {
        qCritical() << "Tag text translation not found.";
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
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CXmlParser* CTemplates::getTPLs() const {
    return m_pMarkupTemplates;
}

CXmlParser* CTemplates::getIWLs() const {
    return m_pInterWikiLinks;
}

CXmlParser* CTemplates::getDropTPLs() const {
    return m_pDropdownTemplates;
}

QString CTemplates::getPreviewTemplate() const {
    return m_sPreviewTemplate;
}

QStringList CTemplates::getListTplNamesINY() const {
    return m_sListTplNamesINY;
}
QStringList CTemplates::getListTemplatesINY() const {
    return m_sListTemplatesINY;
}
QStringList CTemplates::getListTplMacrosINY() const {
    return m_sListTplMacrosINY;
}

QStringList CTemplates::getListTplNamesALL() const {
    return m_sListTplNamesALL;
}
QStringList CTemplates::getListTplMacrosALL() const {
    return m_sListTplMacrosALL;
}

QString CTemplates::getTransTemplate() const {
    return m_sTransTemplate;
}

QString CTemplates::getTransTOC() const {
    return m_sTransTOC;
}

QString CTemplates::getTransImage() const {
    return m_sTransImage;
}

QString CTemplates::getTransCodeBlock() const {
    return m_sTransCodeBlock;
}

QString CTemplates::getTransAttachment() const {
    return m_sTransAttachment;
}

QString CTemplates::getTransAnchor() const {
    return m_sTransAnchor;
}

QString CTemplates::getTransDate() const {
    return m_sTransDate;
}

QString CTemplates::getTransTable() const {
    return m_sTransTable;
}

QString CTemplates::getTransRev() const {
    return m_sTransRevText;
}

QString CTemplates::getTransTag() const {
    return m_sTransTagText;
}

QString CTemplates::getTransKnowledge() const {
    return m_sTransKnowledge;
}

QStringList CTemplates::getListFormatStart() const {
    return m_sListFormatStart;
}

QStringList CTemplates::getListFormatEnd() const {
    return m_sListFormatEnd;
}

QStringList CTemplates::getListFormatHtmlStart() const {
    return m_sListFormatHtmlStart;
}

QStringList CTemplates::getListFormatHtmlEnd() const {
    return m_sListFormatHtmlEnd;
}

// ----------------------------------------------------------------------------
// Image maps

QStringList CTemplates::getListFlags() const {
    return m_sListFlags;
}
QStringList CTemplates::getListFlagsImg() const {
    return m_sListFlagsImg;
}

QStringList CTemplates::getListSmilies() const {
    return m_sListSmilies;
}
QStringList CTemplates::getListSmiliesImg() const {
    return m_sListSmiliesImg;
}
