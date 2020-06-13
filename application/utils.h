/**
 * \file utils.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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
 * Class definition for utile functions.
 */

#ifndef APPLICATION_UTILS_H_
#define APPLICATION_UTILS_H_

#include <QApplication>
#include <QNetworkAccessManager>

class Utils : public QObject {
  Q_OBJECT

 public:
    explicit Utils(QWidget *pParent, QObject *pParentObj = nullptr);

    static auto getOnlineState() -> bool;
    static void setProxy(const QString &sHostName, const quint16 nPort,
                         const QString &sUser, const QString &sPassword);
    void checkWindowsUpdate();

 public slots:
    void showAbout();

 private slots:
    void replyFinished(QNetworkReply *pReply);

 signals:
    void setWindowsUpdateCheck(const bool);

 private:
    QWidget *m_pParent;
    QNetworkAccessManager *m_NwManager;
};

#endif  // APPLICATION_UTILS_H_
