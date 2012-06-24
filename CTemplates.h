/**
 * \file CTemplates.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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
 * Class definition.
 */

#ifndef CTEMPLATES_H
#define CTEMPLATES_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QSettings>

class QString;
class QStringList;
class QFile;
class QDir;

class CTemplates : public QObject
{
    Q_OBJECT

public:
    CTemplates( const QString sAppName,
                const QString sAppPath,
                const QString sTplLang );
    // Destructor
    ~CTemplates();

    QString getPreviewTemplate() const;
    QStringList getListTplNames() const;
    QStringList getListTemplates() const;
    QStringList getListTplMacros() const;
    QStringList getFlaglist() const;
    QString getTransTemplate() const;
    QString getTransTOC() const;
    QString getTransImage() const;
    QString getTransCodeBlock() const;
    QString getTransAttachment() const;
    QString getTransAnchor() const;
    QString getTransDate() const;
    QString getTransOverview() const;
    QString getTransTable() const;
    QString getTransRev() const;
    QString getTransTag() const;

    QStringList getListFormatStart() const;
    QStringList getListFormatEnd() const;
    QStringList getListFormatHtmlStart() const;
    QStringList getListFormatHtmlEnd() const;
    QStringList getListFlags() const;

private:
    void initTemplates();
    void initHtmlTpl( const QString sFileName );
    void initFlags( const QString sFileName );
    void initTextformats( const QString sFileName );
    void initTranslations( const QString sFileName );

    QString m_sAppName;
    QString m_sAppPath;
    QString m_sTplLang;

    QString m_sPreviewTemplate;
    QStringList m_sListTplNames;
    QStringList m_sListTemplates;
    QStringList m_sListTplMacros;

    // List for text formats (start keyword, end keyword, start HTML code, end HTML code)
    QStringList m_sListFormatStart, m_sListFormatEnd, m_sListFormatHtmlStart, m_sListFormatHtmlEnd;
    // List for flags
    QStringList m_sListFlags;

    // Translations
    QString m_sTransTemplate;
    QString m_sTransTOC;
    QString m_sTransImage;
    QString m_sTransCodeBlock;
    QString m_sTransAttachment;
    QString m_sTransAnchor;
    QString m_sTransDate;
    QString m_sTransOverview;
    QString m_sTransRevText;
    QString m_sTransTagText;
    QString m_sTransTable;
};

#endif // CTEMPLATES_H
