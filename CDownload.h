/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CDownload.h
* Purpose:    Class definition download functions
***************************************************************************/

#ifndef CDOWNLOAD_H
#define CDOWNLOAD_H

#include "CInyokaEdit.h"

class CInyokaEdit;

class CDownload : public QObject
{
    Q_OBJECT

public:
    CDownload(QWidget *pParent, CInyokaEdit *pInyokaEdit, const QString &sAppName, const QString &sAppDir, const QDir StylesDir, const QString &sInyokaUrl, const bool bAutomaticImageDownload);

    bool LoadInyokaStyles();

public slots:
    void DownloadArticle();

signals:
    void SendArticleText(const QString &);

private:
    void DownloadImages(const QString &sArticlename);

    QWidget *m_pParent;
    CInyokaEdit *m_pInyokaEdit;
    QString m_sAppName;
    QString m_sAppDir;
    QDir m_StylesDir;
    QString m_sInyokaUrl;
    bool m_bAutomaticImageDownload;

    QString m_sWinBashFolder;
    QString m_sWget;
};

#endif // CDOWNLOAD_H
