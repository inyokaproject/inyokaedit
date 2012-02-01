/**
 * @file CDownload.h
 * @author See AUTHORS
 *
 * @section LICENSE
 *
 * Copyright (C) 2011-2012 by the respective authors (see AUTHORS)
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
 * @section DESCRIPTION
 * Class definition for download functions.
 */

#ifndef CDOWNLOAD_H
#define CDOWNLOAD_H

#include "CInyokaEdit.h"

class CInyokaEdit;

class CDownload : public QObject
{
    Q_OBJECT

public:
    CDownload(QWidget *pParent, const QString &sAppName, const QString &sAppDir, const QDir StylesDir);

    bool loadInyokaStyles();
    void downloadArticle(const QDir ImgDir, const QString &sInyokaUrl, const bool bAutomaticImageDownload);

signals:
    void sendArticleText(const QString &);

private:
    void downloadImages(const QString &sArticlename, const QDir ImgDir, const QString &sInyokaUrl, const bool bAutomaticImageDownload);

    QWidget *m_pParent;
    QString m_sAppName;
    QString m_sAppDir;
    QDir m_StylesDir;

    QString m_sWinBashFolder;
    QString m_sWget;
};

#endif // CDOWNLOAD_H
