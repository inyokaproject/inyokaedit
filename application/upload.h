// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_UPLOAD_H_
#define APPLICATION_UPLOAD_H_

#include <QObject>
#include <QUrl>

class QNetworkReply;
class QTextEdit;

class Session;

class Upload : public QObject {
  Q_OBJECT

 public:
  explicit Upload(QWidget *pParent, Session *pSession,
                  const QString &sInyokaUrl, const QString &sConstArea,
                  QObject *pObj = nullptr);

  void setEditor(QTextEdit *pEditor, const QString &sArticlename);

 public slots:
  void clickUploadArticle();

 private:
  void requestRevision(QString sUrl = QLatin1String(""));
  void getRevisionReply(const QString &sNWReply);
  auto redirectUrl(const QUrl &possibleRedirectUrl, const QUrl &oldRedirectUrl)
      -> QUrl;
  void requestUpload();
  void getUploadReply(const QString &sNWReply);
  void replyFinished(QNetworkReply *pReply);

  enum REQUESTSTATE { REQUREVISION, RECREVISION, REQUPLOAD, RECUPLOAD };

  QWidget *m_pParent;
  Session *m_pSession;
  QString m_sInyokaUrl;
  QNetworkReply *m_pReply;

  REQUESTSTATE m_State;
  QString m_sSitename;
  QUrl m_urlRedirectedTo;
  QString m_sRevision;
  QString m_sConstructionArea;
  QTextEdit *m_pEditor;
  QString m_sArticlename;
};

#endif  // APPLICATION_UPLOAD_H_
