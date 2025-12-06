// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_UTILS_H_
#define APPLICATION_UTILS_H_

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class Utils : public QObject {
  Q_OBJECT

 public:
  explicit Utils(QWidget *pParent, QObject *pParentObj = nullptr);

  static auto getOnlineState() -> bool;
  static void setProxy(const QString &sHostName, const quint16 nPort,
                       const QString &sUser, const QString &sPassword);
  void checkWindowsUpdate();

 private slots:
  void replyFinished(QNetworkReply *pReply);

 signals:
  void setWindowsUpdateCheck(const bool);

 private:
  QWidget *m_pParent;
  QNetworkAccessManager *m_NwManager;
};

#endif  // APPLICATION_UTILS_H_
