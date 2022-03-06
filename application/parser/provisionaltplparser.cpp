/**
 * \file provisionaltplparser.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
 * Provisional template parser. Only used as long as complete inyoka parsing
 * is not supperted.
 */

// clazy:excludeall=qstring-allocations,qt4-qstring-from-array

#include "./provisionaltplparser.h"

#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QRegularExpression>

ProvisionalTplParser::ProvisionalTplParser(
    const QStringList &sListHtmlStart,
    const QString &sSharePath,
    const QDir &tmpImgDir,
    const QHash<QString, QString> &TestedWithMap,
    const QHash<QString, QString> &TestedWithTouchMap,
    const QString &sCommunity)
  : m_sListHtmlStart(sListHtmlStart),
    m_sSharePath(sSharePath),
    m_tmpImgDir(tmpImgDir),
    m_TestedWithMap(TestedWithMap),
    m_TestedWithTouchMap(TestedWithTouchMap),
    m_sCommunity(sCommunity) {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseTpl(const QStringList &sListArgs,
                                    const QString &sCurrentFile) -> QString {
  m_sCurrentFile = sCurrentFile;

  QStringList sArgs = sListArgs;
  if (!sArgs.isEmpty()) {
    if (sArgs[0].toLower() == QString::fromUtf8("Fortgeschritten").toLower()) {
      return ProvisionalTplParser::parseAdvanced();
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Archiviert").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseArchived(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Befehl").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseBash(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Builddeps").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseBuilddeps(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Code").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseCode(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Kopie").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseCopy(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Experten").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseExperts(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Fehlerhaft").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseFixme(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Fremdquelle-auth").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseForeignAuth(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Fremdquelle").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseForeignSource(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Fremdpaket").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseForeignPackage(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Fremd").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseForeignWarning(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Howto").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseHowto(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Icon-Übersicht").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseIconOverview(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("IkhayaAutor").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseIkhayaAuthor(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Ikhaya-Award").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseIkhayaAward(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Ikhayabild").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseIkhayaImage(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8(
                 "Ikhaya-Projektvorstellung").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseIkhayaProjectPresentation();
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Bildersammlung").toLower()) {
      sArgs.removeFirst();
      return this->parseImageCollect(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Bildunterschrift").toLower()) {
      sArgs.removeFirst();
      return this->parseImageSub(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Ausbaufähig").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseImprovable(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Infobox").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseInfobox(sArgs);
    }
    if (sArgs[0].trimmed().toLower() == QString::fromUtf8("Tasten").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseKeys(sArgs);
    }
    if (sArgs[0].trimmed().toLower() == QString::fromUtf8("Wissen").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseKnowledge(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Verlassen").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseLeft(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Hinweis").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseNotice(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("OBS").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseOBS(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8(
                 "Uebersicht").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseOverview(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Uebersicht2").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseOverview2(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Pakete").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parsePackage(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("PipInstallation").toLower()) {
      sArgs.removeFirst();
      return this->parsePipInstall(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8(
                 "Paketinstallation").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parsePkgInstall(sArgs);
    }
    if (sArgs[0].trimmed().toLower() == QString::fromUtf8(
                 "Installbutton").toLower()) {
      sArgs.removeFirst();
      return this->parsePkgInstallBut(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("PPA").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parsePPA(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Projekte").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseProjects(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Seitenleiste").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseSidebar(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("StatusIcon").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseStatusIcon(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Tabelle").toLower()) {
      sArgs.removeFirst();
      return this->parseTable(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Getestet").toLower()) {
      sArgs.removeFirst();
      return this->parseTested(sArgs);
    }
    if (sArgs[0].trimmed().toLower() == QString::fromUtf8("UT").toLower()) {
      sArgs.removeFirst();
      return this->parseTestedUT(sArgs);
    }
    if (sArgs[0].trimmed().toLower() == QString::fromUtf8(
          "Baustelle").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseUnderConst(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Warnung").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseWarning(sArgs);
    }
    if (sArgs[0].toLower() == QString::fromUtf8("Überarbeitung").toLower()) {
      sArgs.removeFirst();
      return ProvisionalTplParser::parseWorkInProgr(sArgs);
    }
  }
  return "";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseAdvanced() -> QString {
  return ProvisionalTplParser::insertBox(
        "box advanced",
        QString::fromUtf8("Artikel für fortgeschrittene Anwender"),
        QString::fromUtf8("Dieser Artikel erfordert mehr Erfahrung im Umgang "
                          "mit Linux und ist daher nur für fortgeschrittene "
                          "Benutzer gedacht."));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseArchived(
    const QStringList &sListArgs) -> QString {
  QString sRemark("");
  if (!sListArgs.isEmpty()) {
    sRemark = sListArgs[0];
  }

  return ProvisionalTplParser::insertBox(
        "box improvable", QString::fromUtf8("Archivierte Anleitung"),
        QString::fromUtf8("Dieser Artikel wurde archiviert. Das bedeutet, dass "
                          "er nicht mehr auf Richtigkeit überprüft oder "
                          "anderweitig gepflegt wird. Der Inhalt wurde für "
                          "keine aktuell unterstützte Ubuntu-Version getestet. "
                          "Wenn du Gründe für eine Wiederherstellung siehst, "
                          "melde dich bitte in der Diskussion zum Artikel. "
                          "Bis dahin bleibt die Seite für weitere Änderungen "
                          "gesperrt."), sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseBash(const QStringList &sListArgs) -> QString {
  QString sOutput("<div class=\"bash\">\n"
                  "<div class=\"contents\">\n"
                  "<pre>");
  for (auto s : sListArgs) {
    sOutput += s.replace("<", "&lt;") + "\n";
  }
  return sOutput + "</pre>\n</div>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseBuilddeps(
    const QStringList &sListArgs) -> QString {
  QString sOutput(
        QString::fromUtf8("<div class=\"package-list\">\n"
                          "<div class=\"contents\">\n"
                          "<p>"
                          "Befehl zum Installieren der Build-Abhängigkeiten: "
                          "'''[#dummy apt-get]''' [#dummy aptitude]</p>\n"
                          "<div class=\"bash\">"
                          "<div class=\"contents\">\n"
                          "<pre class=\"notranslate\"> "
                          "sudo apt-get build-dep"));
  for (const auto &s : sListArgs) {
    sOutput += " " + s.trimmed();
  }
  return sOutput + "</pre>\n</div>\n</div>\n</div>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseCode(const QStringList &sListArgs) -> QString {
  QString sOutput("<pre>");
  for (const auto &s : sListArgs) {
    sOutput += s + "\n";
  }
  return sOutput + "</pre>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseCopy(const QStringList &sListArgs) -> QString {
  QString sLink("");

  if (!sListArgs.isEmpty()) {
    sLink = sListArgs[0];
  }

  return ProvisionalTplParser::insertBox(
        "box warning", QString::fromUtf8("Achtung!"),
        QString::fromUtf8("Dieser Artikel wird aktuell in %1 überarbeitet. "
                          "Daher kann es sein, dass diese Seite hier veraltete "
                          "oder nicht (mehr) zutreffende Informationen enthält."
                          " Vergleiche beide Versionen und wende dich im "
                          "Zweifelsfall mit deinem konkreten Anliegen an das "
                          "Support-Forum. Änderungen am Artikel bitte nur in "
                          "%1!"))
      .arg("[:Baustelle/" + sLink.replace(" ", "_") + ":]");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseExperts(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  for (const auto &s : sListArgs) {
    sOutput += s + " ";
  }
  return ProvisionalTplParser::insertBox("box experts",
                                         QString::fromUtf8("Experten-Info:"),
                                         sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseFixme(const QStringList &sListArgs) -> QString {
  QString sRemark("");
  if (!sListArgs.isEmpty()) {
    sRemark = sListArgs[0];
  }

  return ProvisionalTplParser::insertBox(
        "box fixme", QString::fromUtf8("Fehlerhafte Anleitung"),
        QString::fromUtf8("Diese Anleitung ist fehlerhaft. Wenn du weißt, wie "
                          "du sie ausbessern kannst, nimm dir bitte die Zeit "
                          "und bessere sie aus."), sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseForeignAuth(
    const QStringList &sListArgs) -> QString {
  QStringList sArgs(sListArgs);
  QString sOutput("");

  if (!sArgs.isEmpty()) {
    // Key
    if (sArgs[0].startsWith("key")) {
      sArgs[0].remove("key");
      sArgs[0].remove(" ");

      sOutput = QString::fromUtf8(
                  "<p>Um die [:Fremdquellen:Fremdquelle] zu authentifizieren, "
                  "muss man den Signierungsschlüssel mit folgendem Befehl "
                  "importieren:");
      sOutput += "</p>\n"
                 "<div class=\"bash\">\n"
                 "<div class=\"contents\">\n"
                 "<pre> sudo apt-key adv --recv-keys --keyserver "
                 "keyserver.ubuntu.com " + sArgs[0]
          + " </pre>\n"
            "</div>\n"
            "</div>\n";
    } else {  // Url
      sOutput = QString::fromUtf8(
                  "<p>Um die [:Fremdquellen:Fremdquelle] zu authentifizieren, "
                  "muss man den [%1 Signierungsschlüssel herunterladen] und "
                  "in der [:Paketquellen_freischalten: Paketverwaltung "
                  "hinzufügen]. Alternativ dazu kann man auch im Terminal "
                  "den folgenden Befehl ausführen:"
                  "</p>\n").arg(sArgs[0]);
      sOutput += "<div class=\"bash\">\n"
                 "<div class=\"contents\">\n"
                 "<pre>wget -q -O \"-\" " + sArgs[0] + " | sudo "
                                              "apt-key add - </pre>\n"
                                              "</div>\n"
                                              "</div>\n";
    }
  }
  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseForeignPackage(
    const QStringList &sListArg) -> QString {
  QString sOutput("");

  if (sListArg.size() >= 2) {
    // 1: [[Vorlage(Fremdpaket, Projekthoster, Projektname, Ubuntuversion(en))]]
    if (!(sListArg[1].startsWith("http"))
        && sListArg[1] != "dl") {  // LAUNCHPAD
      if (sListArg[0].toLower() == "launchpad") {
        sOutput = QString::fromUtf8(
                    "<p>Beim [:Launchpad:]-Projekt [launchpad:%1:] werden "
                    "[launchpad:%2/+download:DEB-Pakete] angeboten. ")
                  .arg(sListArg[1], sListArg[1]);
      } else if (sListArg[0].toLower() == "sourceforge") {  // SOURCEFORGE
        sOutput = QString::fromUtf8(
                    "<p>Beim [wikipedia:SourceForge:]-Projekt [sourceforge:%1:]"
                    " werden [sourceforge:%2/files/:DEB-Pakete] angeboten. ")
                  .arg(sListArg[1], sListArg[1]);
      } else if (sListArg[0].toLower() == "googlecode") {  // GOOGLE CODE
        sOutput = QString::fromUtf8(
                    "<p>Beim [http://code.google.com/intl/de/ Google Code]"
                    " {de} -Projekt [googlecode:%1:] werden [googlecode:"
                    "%2/downloads/:DEB-Pakete] angeboten. ")
                  .arg(sListArg[1], sListArg[1]);
      }
    } else if (sListArg[1].startsWith("dl")) {
      // 2: [[Vorlage(Fremdpaket, "Anbieter", dl, URL, Ubuntuversion(en))]]
      sOutput = QString::fromUtf8("<p>Von %1 werden folgende DEB-Pakete "
                                  "angeboten:</p>\n * [%2] {dl}\n")
                .arg(sListArg[0], sListArg[2]);
    } else {
      // 3: [[Vorlage(Fremdpaket, "Anbieter", URL, Ubuntuversion(en))]]
      sOutput = QString::fromUtf8("<p>Von %1 werden [%2 DEB-Pakete] {dl} "
                                  "angeboten. ")
                .arg(sListArg[0], sListArg[1]);
    }
  }

  // No ubuntu version
  if ((sListArg.size() == 2
       && !sListArg[1].startsWith("dl"))
      || (sListArg.size() == 3
          && sListArg[1].startsWith("dl"))) {
    sOutput += QString::fromUtf8("Die unterstützten Ubuntuversionen und "
                                 "Architekturen werden aufgelistet. ");
  } else {
    if (sListArg.size() >= 2) {
      int i = 2;
      if (sListArg[1].startsWith("dl")) {
        i = 3;
      }
      QString sUbuntuVersions("");
      for (int j = i; j < sListArg.size(); j++) {
        sUbuntuVersions += sListArg[j] + " ";
      }
      sOutput += QString::fromUtf8("Die Pakete können für %1 "
                                   "heruntergeladen werden. ")
                 .arg(sUbuntuVersions);
    }
  }

  sOutput += QString::fromUtf8("Nachdem man sie für die korrekte Ubuntuversion "
                               "und Architektur geladen hat, müssen die "
                               "[:Paketinstallation_DEB:DEB-Pakete noch "
                               "installiert werden].</p>\n");

  // Warning box
  sOutput += ProvisionalTplParser::insertBox("box warning",
                                             QString::fromUtf8("Hinweis!"),
                                             QString::fromUtf8(
                                               "[:Fremdquellen:Fremdpakete] "
                                               "können das System gefährden."));
  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseForeignSource(
    const QStringList &sListArgs) -> QString {
  QStringList sArgs(sListArgs);
  QString sOutput("");

  sOutput = QString::fromUtf8("<p>Um aus der [:Fremdquellen:Fremdquelle] zu "
                              "installieren, muss man unabhängig von der "
                              "Ubuntu-Version die folgende "
                              "[:Paketquellen_freischalten:]:</p>\n");

  sOutput += ProvisionalTplParser::insertBox("box warning",
                                             QString::fromUtf8("Hinweis!"),
                                             QString::fromUtf8(
                                               "Zusätzliche [:Fremdquellen:] "
                                               "können das System gefährden."));

  if (sArgs.size() >= 2) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    const QStringList sListVersions(sArgs[1].split(" ",
                                                   QString::SkipEmptyParts));
#else
    const QStringList sListVersions(sArgs[1].split(" ", Qt::SkipEmptyParts));
#endif
    QString sVersions("");
    for (auto s : sListVersions) {
      s = s.toLower();
      s[0] = s[0].toUpper();
      sVersions += "<a href=\"#\">" + s + "</a> | ";
    }
    sVersions.remove(sVersions.size() - 2, 2);

    sOutput += "<div class=\"thirpartyrepo-outer\">\n"
               "<div class=\"contents\">\n"
               "<div class=\"selector\">\n"
               "'''Version:''' " + sVersions + "\n"
                                               "</div>\n";

    if (sArgs.size() == 2) {
      sOutput += "<pre> deb " + sArgs[0] + " " + sListVersions[0] + " </pre>";
    } else {
      if (sArgs[2].startsWith(" ")) {
        sArgs[2].remove(0, 1);
      }
      sOutput += "<pre> deb " + sArgs[0] + " "
          + sArgs[1] + " " + sListVersions[0] + "</pre>";
    }

    sOutput += "</div>\n"
               "</div>\n";
  }
  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseForeignWarning(
    const QStringList &sListArg) -> QString {
  QString sRemark("");
  QString sOutput("");

  if (!sListArg.isEmpty()) {
    // Package
    if (sListArg[0].toLower() == QString::fromUtf8("Paket").toLower()) {
      sOutput = QString::fromUtf8("<p>[:Fremdquellen:Fremdpakete] können "
                                  "das System gefährden.</p>\n");
    } else if (sListArg[0].toLower() == QString::fromUtf8(
                 "Quelle").toLower()) {
      // Source
      sOutput = QString::fromUtf8("<p>Zusätzliche [:Fremdquellen:] können "
                                  "das System gefährden.</p>\n");
    } else if (sListArg[0].toLower() == QString::fromUtf8(
                 "Software").toLower()) {
      // Software
      sOutput = QString::fromUtf8("<p>[:Fremdsoftware:] kann das System "
                                  "gefährden.</p>\n");
    }
    // Remark available
    if (sListArg.size() >= 2) {
      sRemark = sListArg[1];
    }
  }

  return ProvisionalTplParser::insertBox("box warning",
                                         QString::fromUtf8("Hinweis!"),
                                         sOutput, sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseIconOverview(
    const QStringList &sListArgs) -> QString {
  QString sOutput("<table style=\"width:95%\">\n<tbody>\n");
  QStringList sListTitle;
  QString sStyle("");

  if (!sListArgs.isEmpty()) {
    sListTitle << sListArgs[0].split("/");
    if (sListTitle.size() > 1) {
      sStyle = sListTitle[1].trimmed() + "-";
    }
  }

  for (int i = 0; i < sListArgs.size(); i++) {
    if (0 == i) {
      sOutput += "<tr class=\"" + sStyle +
                 "titel\">\n"
                 "<td colspan=\"4\">" +
                 sListTitle[0] + "</td>\n</tr>\n<tr class=\"" +
          sStyle + "kopf\">\n"
                   "<td style=\"width: 24%\">Icon</td>\n"
                   "<td style=\"width: 25%\">Dateiname</td>\n"
                   "<td style=\"width: 24%\">Icon</td>\n"
                   "<td style=\"width: 25%\">Dateiname</td>\n</tr>";
    } else {
      if (1 == i % 2) {
        sOutput += "<tr>\n";
      }
      sOutput += "<td>[[Bild(Wiki/Icons/" + sListArgs[i] +
                 ", x32, center)]]</td>"
                 "<td><strong>" + sListArgs[i] + "</strong></td>\n";

      if (0 == i % 2) {
        sOutput += "</tr>\n";
      }

      if ((1 == i % 2) && (i == sListArgs.size() - 1)) {  // Empty cell
        sOutput += "<td colspan=\"2\"> </td>\n</tr>\n";
      }
    }
  }

  return sOutput + "\n</tbody>\n</table>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseIkhayaAuthor(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  for (const auto &s : sListArgs) {
    sOutput += s + " ";
  }

  return ProvisionalTplParser::insertBox("box tested_for",
                                         QString::fromUtf8("Über den Autor"),
                                         sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseIkhayaAward(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  for (const auto &s : sListArgs) {
    sOutput += s + " ";
  }

  return ProvisionalTplParser::insertBox("box award",
                                         QString::fromUtf8("Award:"),
                                         sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseIkhayaImage(
    const QStringList &sListArgs) -> QString {
  QString sImage("");
  QString sText("");
  QString sStyle("float:left; margin-right:1em;");

  if (sListArgs.size() > 1) {
    sImage = sListArgs[0].trimmed();
    sText = sListArgs[1].trimmed();
    if (sImage.contains("right") || sText.contains("right")) {
      sImage = sImage.remove("right").trimmed();
      sText = sText.remove("right").trimmed();
      sStyle = "clear:right; float:right; margin-left:1em;";
    }
  }

  return "<table style=\"" + sStyle +
      "\">\n<tbody>\n<tr class=\"title\">\n"
      "<td style=\"text-align: center\">\n"
      "<img class=\"image-default\" src=\"" + sImage +
      "\" alt=\"" + sImage +
      "\" >\n</td>\n</tr>\n<tr>\n<td>" + sText +
      "</td>\n</tr>\n</tbody></table\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseIkhayaProjectPresentation() -> QString {
  return ProvisionalTplParser::insertBox(
        "box notice", QString::fromUtf8("Hinweis"),
        QString::fromUtf8("Dieser Artikel gehört zur Reihe der "
                          "Projektvorstellungen, die in unregelmäßigen "
                          "Abständen vergleichsweise unbekannte Projekte "
                          "vorstellt, um sie so einem breiteren Publikum näher "
                          "zu bringen. Diese Artikelreihe existiert seit 2008. "
                          "Alle Projektvorstellungen können im Wikiartikel "
                          "[:ubuntuusers/Ikhayateam/Projektvorstellungen:"
                          "Projektvorstellungen] eingesehen werden."));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseImageCollect(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  QString sImageUrl("");
  QString sDescription("");
  QString sColHeight("140");
  QString sImageCollAlign("");
  double iImgHeight;
  double iImgWidth;
  bool bContinue(false);

  QString sImagePath("");
  if (!m_sCurrentFile.isEmpty()) {
    QFileInfo fiArticleFile(m_sCurrentFile);
    sImagePath = fiArticleFile.absolutePath();
  }

  // Get height / align
  for (int i = 0; i < sListArgs.size(); i++) {
    if (0 == i || 1 == i) {
      if ("left" == sListArgs[i] || "right" == sListArgs[i]) {
        sImageCollAlign = sListArgs[i];
      } else if (0.0 != sListArgs[i].toDouble()) {
        sColHeight = sListArgs[i];
      }
    } else {
      break;
    }
  }

  if ("left" == sImageCollAlign.toLower()) {
    sOutput = "<div style=\"overflow: auto;\">\n";
  }

  if (sImageCollAlign.isEmpty()) {  // With word wrap
    sOutput += "<div style=\"overflow: auto;\">\n<div "
               "class=\"contents\"> </div>\n</div>";
  } else {  // In continuous text (sImageCollAlign = left | rigth)
    sOutput += "<table style=\"float: " + sImageCollAlign
               + "; border: none\">\n<tbody>\n<tr "
                 "style=\"background-color: #E2C889\">\n";
  }

  for (int i = 0; i < sListArgs.length(); i++) {
    if (bContinue) {
      bContinue = false;
      continue;
    }
    if (0 == i || 1 == i) {
      if ("left" == sListArgs[i] || "right" == sListArgs[i]) {
        continue;
      }
      if (0.0 != sListArgs[i].toDouble()) {
        continue;
      }
    }

    sImageUrl = sListArgs[i];
    if (sImageUrl.startsWith("Wiki/")) {
      sImageUrl = m_sSharePath + "/community/" +
                  m_sCommunity + "/web/" + sImageUrl;
    } else if (!sImagePath.isEmpty() &&
               QFile(sImagePath + "/" + sImageUrl).exists()) {
      sImageUrl = sImagePath + "/" + sImageUrl;
    } else {
      sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
    }

    iImgHeight = QImage(sImageUrl).height();
    iImgWidth = static_cast<double>(
                  QImage(sImageUrl).width())/(iImgHeight/sColHeight.toDouble());

    if (sImageCollAlign.isEmpty()) {  // With word wrap
      if ((i+1) < sListArgs.size()) {
        sDescription = sListArgs[i+1];
        bContinue = true;
      } else {
        sDescription.clear();
      }

      sOutput += "<table style=\"float: left; margin: 10px 5px; border: none\">"
                 "\n<tbody>\n<tr>\n<td style=\"text-align: center; background-"
                 "color: #E2C889; border: none\"><a href=\"" + sImageUrl +
                 "\" class=\"crosslink\"><img src=\"" + sImageUrl +
                 "\" alt=\"" + sImageUrl +
                 "\" class=\"image-default\" width=\"" +
                 QString::number(static_cast<int>(iImgWidth)) + "\" height=\"" +
                 sColHeight + "\"/></a></td>\n</tr>\n<tr>\n<td style=\"text-"
                              "align: center; background-color: #F9EAAF; "
                              "border: none\">" + sDescription +
                 "</td>\n</tr>\n</tbody>\n</table>\n";
    } else {  // In continuous text (sImageCollAlign = left | rigth)
      sOutput += "<td style=\"text-align: center; border-width: 0 10px 0 0; "
                 "border-color: #FFFFFF \">\n<img src=\"" + sImageUrl +
                 "\" alt=\"" + sImageUrl +
                 "\" class=\"image-default\" width=\"" +
                 QString::number(static_cast<int>(iImgWidth)) + "\" height=\"" +
                 sColHeight + "\"/></td>\n";
      bContinue = true;
    }
  }

  if (sImageCollAlign.isEmpty()) {  // With word wrap
    sOutput += "<div style=\"overflow: auto;\">\n<div class=\"contents\"> "
               "</div>\n</div>";
  } else {  // In continuous text (sImageCollAlign = left | rigth)
    sOutput += "</tr>\n<tr style=\"background-color: #F9EAAF\">";

    for (int i = 0; i < sListArgs.length(); i++) {
      if (bContinue) {
        bContinue = false;
        continue;
      }
      if (0 == i || 1 == i) {
        if ("left" == sListArgs[i] || "right" == sListArgs[i]) {
          continue;
        }
        if (0.0 != sListArgs[i].toDouble()) {
          continue;
        }
      }

      if ((i+1) < sListArgs.size()) {
        sDescription = sListArgs[i+1];
        bContinue = true;
      } else {
        sDescription.clear();
      }

      sOutput += "<td style=\"text-align: center; border-width: "
                 "0 10px 0 0; border-color: #FFFFFF\">\n"
                 + sDescription + "</td>\n";
    }
    sOutput += "</tr></tbody>\n</table>\n";

    if ("left" == sImageCollAlign.toLower()) {
      sOutput += "</div>\n";
    }
  }

  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseImageSub(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  QString sImageUrl("");
  QString sImageWidth("");
  QString sImageDescription("");
  QString sImageAlign("left");
  QString sImageStyle("");
  double iImgHeight;
  double iImgWidth;

  QString sImagePath("");
  if (!m_sCurrentFile.isEmpty()) {
    QFileInfo fiArticleFile(m_sCurrentFile);
    sImagePath = fiArticleFile.absolutePath();
  }

  sImageUrl = sListArgs[0].trimmed();
  if (sImageUrl.startsWith("Wiki/")) {
    sImageUrl = m_sSharePath + "/community/" +
                m_sCommunity + "/web/" + sImageUrl;
  } else if (!sImagePath.isEmpty() &&
             QFile(sImagePath + "/" + sImageUrl).exists()) {
    sImageUrl = sImagePath + "/" + sImageUrl;
  } else {
    sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
  }

  for (int i = 1; i < sListArgs.length(); i++) {
    // Found integer -> width
    if (sListArgs[i].trimmed().toUInt() != 0) {
      sImageWidth = sListArgs[i].trimmed();
    } else if (sListArgs[i].trimmed() == "left"
               || sListArgs[i].trimmed() == "right") {
      // Alignment
      sImageAlign = sListArgs[i].trimmed();
    } else if (sListArgs[i].trimmed() == "xfce-style"
               || sListArgs[i].trimmed() == "kde-style"
               || sListArgs[i].trimmed() == "edu-style"
               || sListArgs[i].trimmed() == "lxde-style"
               || sListArgs[i].trimmed() == "studio-style") {
      // Style
      sImageStyle = sListArgs[i].trimmed().remove("-style");
    } else {
      // Everything else is description...
      sImageDescription += sListArgs[i];
    }
  }

  iImgWidth = QImage(sImageUrl).width();
  if (!sImageWidth.isEmpty()) {
    iImgHeight = static_cast<double>(
                   QImage(
                     sImageUrl).height()) / (iImgWidth/sImageWidth.toDouble());
  } else {
    // Default
    sImageWidth = "140";
    iImgHeight = static_cast<double>(
                   QImage(sImageUrl).height()) / (iImgWidth / 140);
  }

  sOutput = "<table style=\"float: " + sImageAlign
            + "; clear: both; border: none\">\n<tbody>\n";

  // No style info -> default
  if (sImageStyle.isEmpty()) {
    sOutput += "<tr class=\"titel\">\n";
  } else {
    sOutput += "<tr class=\"" + sImageStyle + "-titel\">\n";
  }

  sOutput += "<td>\n<a href=\"" + sImageUrl +
             "\" class=\"crosslink\">\n" + "<img src=\"" + sImageUrl +
             "\" alt=\"" + sImageUrl + "\" class=\"image-default\" height=\"" +
             QString::number(static_cast<int>(iImgHeight)) + "\" width=\"" +
             sImageWidth + "\"/>\n</a>\n</td>\n</tr>\n";

  // No style info -> default
  if (sImageStyle.isEmpty()) {
    sOutput += "<tr class=\"normal\">\n";
  } else {
    sOutput += "<tr class=\"" + sImageStyle + "-kopf\">\n";
  }

  sOutput += "<td style=\"text-align: center\">" + sImageDescription
             + "</td>\n</tr>" + "</tbody>\n</table>\n";

  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseImprovable(
    const QStringList &sListArgs) -> QString {
  QString sRemark("");
  if (!sListArgs.isEmpty()) {
    sRemark = sListArgs[0];
  }

  return ProvisionalTplParser::insertBox(
        "box improvable", QString::fromUtf8("Ausbaufähige Anleitung"),
        QString::fromUtf8("Dieser Anleitung fehlen noch einige Informationen. "
                          "Wenn Du etwas verbessern kannst, dann editiere den "
                          "Beitrag, um die Qualität des Wikis noch weiter zu "
                          "verbessern."), sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseInfobox(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");

  QStringList sList;
  for (const auto &s : sListArgs) {
    if ("+++" != s) {
      sList << s;
    }
  }

  if (sList.size() >= 3) {
    sOutput = "||<-2 tablestyle=\"float:right; clear:right; margin-top: 1em; "
              "margin-left: 1.5em;\" rowclass=\"verlauf\">" + sList[0] + "||\n";
    sOutput += "||<-2 cellstyle=\"text-align: center; line-height: 2.2em;\">"
               " " + sList[1];
    if (sList[2].endsWith("png") || sList[2].endsWith("jpg")) {
      sOutput += "[[BR]][[Bild(" + sList[2] + ", 260, )]]";
    }
    sOutput += "||\n";

    for (int i = 4; i < sList.size(); i++) {
      if (0 == i % 2) {
        sOutput += "||" + sList[i] + ": ||";
      } else {
        sOutput += sList[i] + "||\n";
      }
    }

    sOutput += "||<-3 rowclass=\"highlight\">||\n"
               "||Installation: ||" + sList[3] + "||";
  }

  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseHowto(const QStringList &sListArgs) -> QString {
  // Generate user list
  QString sUsers("");
  for (int i = 0; i < sListArgs.size(); i++) {
    if (sListArgs.size() > 1) {
      // Comma or "and" between users
      if (i == sListArgs.size() - 1) {
        sUsers += " und ";
      } else if (i > 0 && i < sListArgs.size() - 1) {
        sUsers += ", ";
      }
    }
    sUsers += "[user:" + sListArgs.at(i) + ":]";
  }

  QString sOutput("Diese Howto wurde von " + sUsers + " erstellt. "
                  "Bei Problemen mit der Anleitung melde dies bitte in der "
                  "dazugehörigen Diskussion und wende dich gegebenenfalls "
                  "zusätzlich an den Verfasser des Howtos.");

  return ProvisionalTplParser::insertBox("box notice",
                                         QString::fromUtf8("Hinweis:"),
                                         sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseKeys(const QStringList &sListArgs) -> QString {
  QString sTmpKey("");
  QString sTmpKeyCode("");
  QStringList sListTmpKeys;

  if (!sListArgs.isEmpty()) {
    sTmpKey = sListArgs[0];
  }

  sListTmpKeys = sTmpKey.split("+");
  sTmpKey.clear();

  for (int i = 0; i < sListTmpKeys.size(); i++) {
    bool bImage = false;
    // Remove possible spaces before and after string
    sListTmpKeys[i] = sListTmpKeys[i].trimmed();
    sListTmpKeys[i][0] = sListTmpKeys[i][0].toLower();

    if (sListTmpKeys[i] == "altgr"
        || sListTmpKeys[i] == "alt gr") {
      sTmpKeyCode = "Alt Gr";
    } else if (sListTmpKeys[i] == "backspace"
               || sListTmpKeys[i] == QString::fromUtf8("löschen")
               || sListTmpKeys[i] == QString::fromUtf8("rückschritt")) {
      sTmpKeyCode = "&#9003;";
    } else if (sListTmpKeys[i] == "ctrl"
               || sListTmpKeys[i] == "steuerung") {
      sTmpKeyCode = "Strg";
    } else if (sListTmpKeys[i] == "del"
               || sListTmpKeys[i] == "delete"
               || sListTmpKeys[i] == "entfernen") {
      sTmpKeyCode = "Entf";
    } else if (sListTmpKeys[i] == "return"
               || sListTmpKeys[i] == "enter"
               || sListTmpKeys[i] == "eingabe") {
      sTmpKeyCode = "&#9166;";
    } else if (sListTmpKeys[i] == "escape") {
      sTmpKeyCode = "Esc";
    } else if (sListTmpKeys[i] == "eckig_auf"
               || sListTmpKeys[i] == "eckig auf"
               || sListTmpKeys[i] == "squared open"
               || sListTmpKeys[i] == "squared-open") {
      sTmpKeyCode = "&#91;";
    } else if (sListTmpKeys[i] == "geschweift_auf"
               || sListTmpKeys[i] == "curly open"
               || sListTmpKeys[i] == "curly-open") {
      sTmpKeyCode = "&#123;";
    } else if (sListTmpKeys[i] == "bild auf"
               || sListTmpKeys[i] == "bild-auf"
               || sListTmpKeys[i] == "bild rauf"
               || sListTmpKeys[i] == "bild-rauf") {
      sTmpKeyCode = "Bild &uarr;";
    } else if (sListTmpKeys[i] == "bild ab"
               || sListTmpKeys[i] == "bild-ab"
               || sListTmpKeys[i] == "bild runter"
               || sListTmpKeys[i] == "bild-runter") {
      sTmpKeyCode = "Bild &darr;";
    } else if (sListTmpKeys[i] == "print"
               || sListTmpKeys[i] == "prtsc"
               || sListTmpKeys[i] == "sysrq") {
      sTmpKeyCode = "Druck";
    } else if (sListTmpKeys[i] == "shift"
               || sListTmpKeys[i] == "umschalt"
               || sListTmpKeys[i] == "umsch") {
      sTmpKeyCode = "&uArr;";
    } else if (sListTmpKeys[i] == "shift2"
               || sListTmpKeys[i] == "umschalt2"
               || sListTmpKeys[i] == "umsch2") {
      sTmpKeyCode = "&uArr;&nbsp;&nbsp;&nbsp;";
    } else if (sListTmpKeys[i] == "home"
               || sListTmpKeys[i] == "pos 1") {
      sTmpKeyCode = "Pos1";
    } else if (sListTmpKeys[i] == "mac"
               || sListTmpKeys[i] == "apple"
               || sListTmpKeys[i] == "apfel"
               || sListTmpKeys[i] == "cmd") {
      sTmpKeyCode = "&#8984;";
    } else if (sListTmpKeys[i] == "wahl"
               || sListTmpKeys[i] == "alternate"
               || sListTmpKeys[i] == "optionen") {
      sTmpKeyCode = "&#8997;";
    } else if (sListTmpKeys[i] == "gleich"
               || sListTmpKeys[i] == "equal") {
      sTmpKeyCode = "=";
    } else if (sListTmpKeys[i] == QString::fromUtf8("schrägstrich")
               || sListTmpKeys[i] == "slash") {
      sTmpKeyCode = "/";
    } else if (sListTmpKeys[i] == "caps"
               || sListTmpKeys[i] == "feststell"
               || sListTmpKeys[i] == "feststelltaste"
               || sListTmpKeys[i] == QString::fromUtf8("groß")) {
      sTmpKeyCode = "&dArr;";
    } else if (sListTmpKeys[i] == "doppelpunkt") {
      sTmpKeyCode = ":";
    } else if (sListTmpKeys[i] == "semikolon") {
      sTmpKeyCode = ";";
    } else if (sListTmpKeys[i] == "ausrufezeichen") {
      sTmpKeyCode = "!";
    } else if (sListTmpKeys[i] == "und") {
      sTmpKeyCode = "&#38";
    } else if (sListTmpKeys[i] == QString::fromUtf8("rückstrich")
               || sListTmpKeys[i] == "backslash"
               || sListTmpKeys[i] == "\\") {
      sTmpKeyCode = "&#92";
    } else if (sListTmpKeys[i] == "space"
               || sListTmpKeys[i] == "leer"
               || sListTmpKeys[i] == "leertaste"
               || sListTmpKeys[i] == "leerzeichen"
               || sListTmpKeys[i] == "leerschritt") {
      sTmpKeyCode = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                    "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    } else if (sListTmpKeys[i] == "tab"
               || sListTmpKeys[i] == "tabulator") {
      sTmpKeyCode = "Tab &#8644;";
    } else if (sListTmpKeys[i] == "win") {
      sTmpKeyCode = "Windows";
    } else if (sListTmpKeys[i] == "hoch"
               || sListTmpKeys[i] == "auf"
               || sListTmpKeys[i] == "up"
               || sListTmpKeys[i] == "rauf"
               || sListTmpKeys[i] == "pfeil-hoch"
               || sListTmpKeys[i] == "pfeil hoch"
               || sListTmpKeys[i] == "oben"
               || sListTmpKeys[i] == "pfeil auf") {
      sTmpKeyCode = "&uarr;";
    } else if (sListTmpKeys[i] == "runter"
               || sListTmpKeys[i] == "ab"
               || sListTmpKeys[i] == "down"
               || sListTmpKeys[i] == "pfeil-runter"
               || sListTmpKeys[i] == "pfeil-ab"
               || sListTmpKeys[i] == "pfeil runter"
               || sListTmpKeys[i] == "pfeil ab"
               || sListTmpKeys[i] == "pfeil-unten"
               || sListTmpKeys[i] == "pfeil unten"
               || sListTmpKeys[i] == "unten") {
      sTmpKeyCode = "&darr;";
    } else if (sListTmpKeys[i] == "links"
               || sListTmpKeys[i] == "left"
               || sListTmpKeys[i] == "pfeil-links"
               || sListTmpKeys[i] == "pfeil links") {
      sTmpKeyCode = "&larr;";
    } else if (sListTmpKeys[i] == "rechts"
               || sListTmpKeys[i] == "right"
               || sListTmpKeys[i] == "pfeil-rechts"
               || sListTmpKeys[i] == "pfeil rechts") {
      sTmpKeyCode = "&rarr;";
    } else if (sListTmpKeys[i] == "\",\""
               || sListTmpKeys[i] == "\',\'") {
      sTmpKeyCode = ",";
    } else if (sListTmpKeys[i] == "minus") {
      sTmpKeyCode = "-";
    } else if (sListTmpKeys[i] == "raute"
               || sListTmpKeys[i] == "rautenzeichen"
               || sListTmpKeys[i] == "hash") {
      sTmpKeyCode = "#";
    } else if (sListTmpKeys[i] == "eckig_zu"
               || sListTmpKeys[i] == "eckig zu"
               || sListTmpKeys[i] == "squared close"
               || sListTmpKeys[i] == "squared-close") {
      sTmpKeyCode = "&#93;";
    } else if (sListTmpKeys[i] == "geschweift_zu"
               || sListTmpKeys[i] == "curly close"
               || sListTmpKeys[i] == "curly-close") {
      sTmpKeyCode = "&#125;";
    } else if (sListTmpKeys[i] == "plus") {
      sTmpKeyCode = "+";
    } else if (sListTmpKeys[i] == "gravis"
               || sListTmpKeys[i] == "grave"
               || sListTmpKeys[i] == "'") {
      sTmpKeyCode = "&#96;";
    } else if (sListTmpKeys[i] == "sz") {
      sTmpKeyCode = "&szlig;";
    } else if (sListTmpKeys[i] == "end") {
      sTmpKeyCode = "Ende";
    } else if (sListTmpKeys[i] == "ins"
               || sListTmpKeys[i] == QString::fromUtf8("einfügen")
               || sListTmpKeys[i] == "insert") {
      sTmpKeyCode = "Einfg";
    } else if (sListTmpKeys[i] == "num"
               || sListTmpKeys[i] == "num-taste"
               || sListTmpKeys[i] == "num-lock-taste") {
      sTmpKeyCode = "num &dArr;";
    } else if (sListTmpKeys[i] == "fragezeichen"
               || sListTmpKeys[i] == "question mark"
               || sListTmpKeys[i] == "question-mark") {
      sTmpKeyCode = "?";
    } else if (sListTmpKeys[i] == "break") {
      sTmpKeyCode = "Pause";
    } else if (sListTmpKeys[i] == "rollen"
               || sListTmpKeys[i] == "bildlauf") {
      sTmpKeyCode = "&dArr; Rollen";
    } else if (sListTmpKeys[i] == "akut"
               || sListTmpKeys[i] == "acute") {
      sTmpKeyCode = "&#146;";
    } else if (sListTmpKeys[i] == "anfuehrungsstriche"
               || sListTmpKeys[i] == "doublequote") {
      sTmpKeyCode = "&#34;";
    } else if (sListTmpKeys[i] == "prozent") {
      sTmpKeyCode = "%";
    } else if (sListTmpKeys[i] == "compose") {
      sTmpKeyCode = "&#9092;";
    } else if (sListTmpKeys[i] == "rstrg"
               || sListTmpKeys[i] == "rctrl"
               || sListTmpKeys[i] == "rsteuerung") {
      sTmpKeyCode = "RStrg";
    } else if (sListTmpKeys[i] == "lmt"
               || sListTmpKeys[i] == "lmb") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/mouse_left.png,"
                 "alt=\"linke Maustaste\")]]";
    } else if (sListTmpKeys[i] == "rmt"
               || sListTmpKeys[i] == "rmb") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/mouse_right.png, "
                 "alt=\"rechte Maustaste\")]]";
    } else if (sListTmpKeys[i] == "mmt"
               || sListTmpKeys[i] == "mmb") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/mouse_midd.png, "
                 "alt=\"mittlere Maustaste\")]]";
    } else if (sListTmpKeys[i] == "pfeiltasten") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Icons/pfeiltasten.png, 66, "
                 "alt=\"Pfeiltasten\")]]";
    } else if (sListTmpKeys[i] == "wasd") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Icons/tasten_wasd.png, 66, "
                 "alt=\"W A S D Tasten\")]]";
    } else if (sListTmpKeys[i] == "menue"
               || sListTmpKeys[i] == "menue-taste") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/menu.png, alt=\"Menü\")]]";
    } else if (sListTmpKeys[i] == "power"
               || sListTmpKeys[i] == "an") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/power.png, alt=\"Power\")]]";
    } else if (sListTmpKeys[i] == "tux") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/tux.png, alt=\"Tux\")]]";
    } else if (sListTmpKeys[i] == "ubuntu") {
      bImage = true;
      sTmpKey += "[[Bild(Wiki/Vorlagen/Tasten/ubuntu.png, alt=\"Ubuntu\")]]";
    } else if (sListTmpKeys[i] == "at") {
      sTmpKeyCode = "&#64;";
    } else if (sListTmpKeys[i] == "cherry"
               || sListTmpKeys[i] == "keyman") {
      sTmpKeyCode = "Keym&#64;n";
    } else if (sListTmpKeys[i] == "vol"
               || sListTmpKeys[i] == "lauter") {
      sTmpKeyCode = "Vol+";
    } else if (sListTmpKeys[i] == "vol-"
               || sListTmpKeys[i] == "leiser") {
      sTmpKeyCode = "Vol-";
    } else if (sListTmpKeys[i] == "anykey"
               || sListTmpKeys[i] == "any") {
      sTmpKeyCode = "ANY";
    } else if (sListTmpKeys[i] == "panic") {
      sTmpKeyCode = "PANIC";
    } else if (sListTmpKeys[i] == "koelsch"
               || sListTmpKeys[i] == QString::fromUtf8("kölsch")) {
      sTmpKeyCode = "K&ouml;lsch";
    } else {
      // Everything else: First character to Upper
      // first characters had been changed to lower at beginning
      sListTmpKeys[i][0] = sListTmpKeys[i][0].toUpper();
      sTmpKeyCode = sListTmpKeys[i];
    }

    if (!bImage) {
      sTmpKey += "<span class=\"key\">" + sTmpKeyCode + "</span>";
    }

    // "+" between keys
    if (i != sListTmpKeys.size()-1) {
      sTmpKey += " + ";
    }
  }
  return sTmpKey;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseKnowledge(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  QString sTmp;
  for (int i = 0; i < sListArgs.length(); i++) {
    sOutput += "\n 1. [[Anker(source-" + QString::number(i + 1) + ")]]";
    if (sListArgs[i].contains(",")) {
      sOutput += sListArgs[i].left(sListArgs[i].indexOf(",")).trimmed();
      sTmp = sListArgs[i].right(sListArgs[i].length()
                                - sListArgs[i].indexOf(",")).trimmed();
      sOutput += sTmp;
    } else {
      sOutput += sListArgs[i];
    }
  }

  return ProvisionalTplParser::insertBox(
        "box knowledge",
        QString::fromUtf8("Zum Verständnis dieses Artikels sind "
                          "folgende Seiten hilfreich:"),
        sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseLeft(const QStringList &sListArgs) -> QString {
  QString sRemark("");
  if (!sListArgs.isEmpty()) {
    sRemark = sListArgs[0];
  }

  return ProvisionalTplParser::insertBox(
        "box left", QString::fromUtf8("Verlassene Anleitung"),
        QString::fromUtf8("Dieser Artikel wurde von seinem Ersteller verlassen "
                          "und wird nicht mehr weiter von ihm gepflegt. Wenn "
                          "Du den Artikel fertigstellen oder erweitern kannst, "
                          "dann bessere ihn bitte aus."), sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseNotice(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  for (const auto &s : sListArgs) {
    sOutput += s + " ";
  }
  return ProvisionalTplParser::insertBox("box notice",
                                         QString::fromUtf8("Hinweis:"),
                                         sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseOBS(const QStringList &sListArgs) -> QString {
  QString sOutput("");
  sOutput = QString::fromUtf8("<p>Um das Paket aus einer vom "
                              "[:Open_Build_Service:Open Build Service] "
                              "generierten Paketquelle zu installieren, muss "
                              "man zuerst die [:Paketquellen_freischalten:"
                              "Paketquelle freischalten], indem man folgenden"
                              " Befehl mit [:sudo:root-Rechten] im "
                              "[:Terminal:] ausführt: </p>\n");

  if (!sListArgs.isEmpty()) {
    sOutput += "<div class=\"thirdpartyrepo-outer "
               "thirdpartyrepo-version-14.04\"><div class=\"contents\"><p>"
               "</p><div class=\"bash\"><div class=\"contents\"><pre "
               "class=\"notranslate\">sudo add-apt-repository 'deb http://"
               "download.opensuse.org/repositories/" + sListArgs[0] +
        "/xUbuntu_VERSION/ /'</pre></div></div><p></p></div></div>";
  }

  sOutput += ProvisionalTplParser::insertBox(
               "box warning",
               QString::fromUtf8("Hinweis!"),
               QString::fromUtf8("Zusätzliche [:Fremdquellen:] können"
                                 " das System gefährden."));

  sOutput += QString::fromUtf8("<p>Anschließend sollte die [:Fremdquelle:] "
                               "authentifiziert werden. Dazu lädt man sich "
                               "mit dem folgenden Befehlen den benötigten "
                               "Schlüssel herunter und fügt diesen dem "
                               "Schlüsselbund hinzu:</p>");

  if (!sListArgs.isEmpty()) {
    sOutput += "<div class=\"thirdpartyrepo-outer "
               "thirdpartyrepo-version-14.04\"><div class=\"contents\"><p>"
               "</p><div class=\"bash\"><div class=\"contents\"><pre "
               "class=\"notranslate\">wget http://download.opensuse.org"
               "/repositories/" + sListArgs[0] +
        "/xUbuntu_VERSION/Release.key<br />sudo apt-key add - &lt; Release.key "
        "</pre></div></div><p></p></div></div>";
  }

  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseOverview(
    const QStringList &sListArgs) -> QString {
  QList <QStringList> sListList;
  QStringList sListTmp;
  QString sOutput("<table style=\"border: none; margin-top: 0; "
                  "margin-right: 0\">\n<tbody>");

  // Split arguments by 'empty line'
  for (int j = 0; j < sListArgs.size(); j++) {
    if (sListArgs[j].isEmpty()) {
      sListList << sListTmp;
      sListTmp.clear();
    } else if (sListArgs.size() - 1 == j) {
      sListTmp << sListArgs[j];
      sListList << sListTmp;
      sListTmp.clear();
    } else {
      sListTmp << sListArgs[j];
    }
  }
  // qDebug() << sListList;

  for (int i = 0; i < sListList.size(); i++) {
    for (int k = 0; k < sListList[i].size(); k++) {
      if (0 == k && 0 == i) {
        if (!sListList[i].isEmpty()) {
          sOutput += "<tr>\n<td colspan=\"3\" style=\"text-align: center; "
                     "background-color: #E2C889; font-size: 1.1em; "
                     "border-width: 0 10px 0 0; border-color: #FFFFFF\">"
                     + sListList[i][k].trimmed() +
                     "</td>\n</tr>\n";
        }
      } else if (sListList[i].size() > 1) {
        if (0 == k) {
          sOutput += "<tr>\n";
        }
        sOutput += "<td style=\"text-align: center; background-color: #F9EAAF; "
                   "border-width: 0 10px 10px 0; border-color: #FFFFFF; "
                   "width: 32%\">" + sListList[i][k] + "</td>\n";
        if (2 == k) {
          sOutput += "</tr>\n";
          continue;
        }
      } else {
        sOutput += "<tr>\n<td colspan=\"3\" style=\"text-align: center; "
                   "border-width: 0 10px 10px 0; border-color: #FFFFFF; "
                   "background-color: #F9EAAF; \">" +
                   sListList[i][k].trimmed() + "</td>\n</tr>\n";
      }
    }
  }

  return sOutput + "\n<tbody>\n</table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseOverview2(
    const QStringList &sListArgs) -> QString {
  QList <QStringList> sListList;
  QStringList sListTmp;
  QString sOutput("<table style=\"width: 98%; border: none;\">\n<tbody>");
  QString sStyle;

  // Split arguments by 'empty line'
  for (int j = 0; j < sListArgs.size(); j++) {
    if (sListArgs[j].isEmpty()) {
      sListList << sListTmp;
      sListTmp.clear();
    } else if (sListArgs.size() - 1 == j) {
      sListTmp << sListArgs[j];
      sListList << sListTmp;
      sListTmp.clear();
    } else {
      sListTmp << sListArgs[j];
    }
  }
  // qDebug() << sListList;

  for (int i = 0; i < sListList.size(); i++) {
    for (int k = 0; k < sListList[i].size(); k++) {
      if (0 == k && 0 == i) {
        if (!sListList[i].isEmpty()) {
          sOutput += "<tr>\n<td colspan=\"6\" style=\"text-align: center; "
                     "background-color: #E2C889; font-size: 1.1em; "
                     "border-width: 0 10px 10px 0; border-color: #FFFFFF\">"
                     + sListList[i][k].trimmed() +
                     "</td>\n</tr>\n";
        }
      } else if (sListList[i].size() > 1) {
        if (0 == k) {
          sOutput += "<tr>\n";
        }
        if (0 == k % 2) {
          sStyle = "border-width: 0 0 10px 0; ";
        } else {
          sStyle = "border-width: 0 10px 10px 0; width: 26%; ";
        }
        sOutput += "<td style=\"" + sStyle +
                   "background-color: #F9EAAF; border-color: #FFFFFF; \">" +
                   sListList[i][k] + "</td>\n";
        if (5 == k) {
          sOutput += "</tr>\n";
          continue;
        }
      }
    }
  }

  return sOutput + "\n<tbody>\n</table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parsePackage(
    const QStringList &sListArgs) -> QString {
  QString sOutput(QString::fromUtf8(
                    "<p>Paketliste zum Kopieren:</p>\n"
                    "<div class=\"bash\">\n<pre class=\"notranslate\"> "
                    "sudo apt-get install"));
  for (const auto &s : sListArgs) {
    sOutput += " " + s;
  }
  return sOutput + "</pre>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parsePipInstall(
    const QStringList &sListArgs) -> QString {
  QString sOutput(QString::fromUtf8(
                    "Wer die neueste Version installieren möchte, kann das "
                    "Programm über den Python Paketmanager [:pip:] "
                    "installieren:\n"
                    "<div class=\"bash\">\n"
                    "<div class=\"contents\">\n"
                    "<pre>pip3 install --user"));
  for (const auto &s : sListArgs) {
    sOutput += " " + s;
  }
  return sOutput + "     # Programm wird nur für den aktuellen Nutzer "
                   "installiert</pre>\n</div>\n</div>\n" +
      this->parseForeignWarning(QStringList() << "Software");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parsePkgInstall(
    const QStringList &sListArgs) -> QString {
  QStringList sListPackages;
  sListPackages.reserve(sListArgs.length());
  QStringList sListPackagesTMP;
  QString sOutput("");
  QString sPackages("");

  for (int i = 0; i < sListArgs.length(); i++) {
    sListPackagesTMP.clear();
    sListPackagesTMP = sListArgs[i].split(",");

    // Package with source
    if (sListPackagesTMP.length() >= 2) {
      sOutput += "\n * '''" + sListPackagesTMP[0].trimmed() + "'''"
          + " (''" + sListPackagesTMP[1].trimmed() + "'')";
    } else {  // Only package
      sOutput += "\n * '''" + sListPackagesTMP[0].trimmed() + "'''";
    }

    sListPackages << sListPackagesTMP[0].trimmed();
    sPackages += sListPackagesTMP[0].trimmed();
    if (i < sListArgs.size() - 1) {
      sPackages += ",";
    }
  }

  sOutput += "\n<p>" +
      QString::fromUtf8("Befehl zum Installieren der Pakete:") + "</p>\n";
  sOutput += "<div class=\"bash\">\n"
             "<pre class=\"notranslate\"> sudo apt-get install";
  for (auto &s : sListPackages) {
    sOutput += " " + s.trimmed();
  }
  sOutput += "</pre>\n</div>\n";
  sOutput += "<p>" + QString::fromUtf8("Oder mit [:apturl:] installieren, "
                                       "Link: [apt://");
  sOutput += sPackages;
  return sOutput + "]</p>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parsePkgInstallBut(
    const QStringList &sListArgs) -> QString {
  QString sTmp;
  QString sOutput("");

  sOutput += "<p>\n"
             "<a href=\"apt://";

  // Apt packages install list / button
  for (int i = 0; i < sListArgs.length(); i++) {
    sTmp = sListArgs[i];
    sOutput += sTmp.replace(" ", ",").trimmed();
    if (i < sListArgs.size() - 1) {
      sOutput += ",";
    }
  }

  sOutput += "\" rel=\"nofollow\" class=\"external\">"
             "<img src=\"" + m_sSharePath + "/community/" +
             m_sCommunity + "/web/Wiki/Vorlagen/Installbutton/button.png\" "
             "alt=\"Wiki-Installbutton\" class=\"image-default\" /></a> "
             "mit [:apturl:]</p>";

  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parsePPA(const QStringList &sListArgs) -> QString {
  QStringList sArgs(sListArgs);
  QString sOutsideBox("");
  QString sRemark("");
  QString sUser;
  QString sPPA;
  QString sOutput("");

  if (!sArgs.isEmpty()) {
    sArgs[0].replace(" ", "_");  // Replace possible spaces
    if (sArgs[0].contains('/')) {
      QStringList sList;
      sList = sArgs[0].split("/");
      sUser = sList[0];
      sPPA = sList[1];
    } else if (sArgs.size() >= 2) {
      sArgs[1].replace(" ", "_");
      sUser = sArgs[0];
      sPPA = sArgs[1];
    } else {
      sUser = sArgs[0];
      sPPA = "ppa";
    }

    // Generate output
    sOutsideBox = QString::fromUtf8(
                    "<p>Adresszeile zum [:Paketquellen_freischalten/PPA#PPA-"
                    "hinzufuegen:Hinzufügen] des PPAs:</p>");
    sOutsideBox += "\n * <strong>ppa:" + sUser + "/" + sPPA + "</strong>\n";

    sOutput = QString::fromUtf8("Zusätzliche [:Fremdquellen:] können das "
                                "System gefährden.");
    sRemark = QString::fromUtf8(
          "Ein PPA unterstützt nicht zwangsläufig alle Ubuntu-Versionen. "
          "Weitere Informationen sind der [[Bild(Wiki/Vorlagen/PPA/ppa.png)]]"
          " [https://launchpad.net/~%1/+archive/%2 PPA-Beschreibung] des "
          "Eigentümers/Teams [lpuser:%3:] zu entnehmen.")
        .arg(sUser, sPPA, sUser);
  }

  return sOutsideBox + ProvisionalTplParser::insertBox(
        "box warning", QString::fromUtf8("Hinweis!"), sOutput, sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseProjects(
    const QStringList &sListArgs) -> QString {
  QList <QStringList> sListList;
  QStringList sListTmp;
  QString sOutput("<table>\n<tbody>");
  QString sImage("");
  QString sTitle("");
  QString sText("");
  QString sLinks("");

  // Split arguments by "+++"
  for (int j = 0; j < sListArgs.size(); j++) {
    if ("+++" == sListArgs[j]) {
      sListList << sListTmp;
      sListTmp.clear();
    } else if (sListArgs.size() - 1 == j) {
      sListTmp << sListArgs[j];
      sListList << sListTmp;
      sListTmp.clear();
    } else {
      sListTmp << sListArgs[j];
    }
  }
  // qDebug() << sListList;

  for (int i = 0; i < sListList.size(); i++) {
    if (0 == i) {
      if (!sListList[i].isEmpty()) {
        sOutput += "<tr class=\"verlauf\">\n<td colspan=\"2\">"
                   + sListList[i][0] + "</td>\n</tr>";
      }
    } else {
      sImage.clear();
      sTitle.clear();
      sText.clear();
      sLinks.clear();
      for (int k = 0; k < sListList[i].size(); k++) {
        if (0 == k) {
          sImage = sListList[i][k];
        } else if (1 == k) {
          sTitle = sListList[i][k];
        } else if (2 == k) {
          sText = sListList[i][k];
        } else if (3 == k) {
          sLinks = sListList[i][k];
          sLinks.replace(", ", " | ");
        }
      }
      sOutput += "<tr style=\"background-color: #f2f2f2\">"
                 "<td style=\"text-align: center; border-right-color: #f2f2f2; "
                 "font-weight: bold\">[[Bild(" + sImage + ", 32)]][[BR]]" +
                 sTitle + "</td><td style=\"padding-top: 0.8em; border-left-"
                          "color: #f2f2f2\">" +
                 sText + "\n<div style=\"color: #000000; margin-left: 1em\">\n"
                           "<div class=\"contents\">\n<p>" + sLinks +
                 "</p>\n</div>\n</div>""</td></tr>";
    }
  }

  return sOutput + "</tbody></table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseSidebar(
    const QStringList &sListArgs) -> QString {
  QList <QStringList> sListList;
  QStringList sListTmp;
  QString sOutput("<table style=\"float: right; clear: right; border: none; "
                  "margin-top: 0; margin-left: 5px\">\n<tbody>");

  // Split arguments by 'empty line'
  for (int j = 0; j < sListArgs.size(); j++) {
    if (sListArgs[j].isEmpty()) {
      sListList << sListTmp;
      sListTmp.clear();
    } else if (sListArgs.size() - 1 == j) {
      sListTmp << sListArgs[j];
      sListList << sListTmp;
      sListTmp.clear();
    } else {
      sListTmp << sListArgs[j];
    }
  }

  for (int i = 0; i < sListList.size(); i++) {
    for (int k = 0; k < sListList[i].size(); k++) {
      if (0 == k) {
        if (!sListList[i].isEmpty()) {
          sOutput += "<tr>\n<td style=\"text-align: center; "
                     "background-color: #E2C889; font-size: 1.1em; "
                     "border: none\">" + sListList[i][k].trimmed() +
                     "</td>\n</tr>\n";
        }
      } else if (sListList[i][k].contains("[[BR]]")
                 || k == sListList[i].size() - 1) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        sListTmp = sListList[i][k].split("[[BR]]", QString::SkipEmptyParts);
#else
        sListTmp = sListList[i][k].split("[[BR]]", Qt::SkipEmptyParts);
#endif
        for (const auto &s : qAsConst(sListTmp)) {
          sOutput += "<tr>\n<td style=\"background-color: #F9EAAF; "
                     "border-width: 0 0 10px 0; border-color: #FFFFFF\">"
                     + s + "</td>\n</tr>\n";
        }
      } else {
        sOutput += "<tr>\n<td style=\"background-color: #F9EAAF; "
                   "border: none\">" + sListList[i][k].trimmed() +
                   "</td>\n</tr>\n";
      }
    }
  }

  return sOutput + "\n<tbody>\n</table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseStatusIcon(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  QString sTmp;
  for (int i = 0; i < sListArgs.size(); i++) {
    if (sListArgs.size() - 1 == i) {
      sTmp = sListArgs[i];
      if (sListArgs[i].contains("ja")) {
        sTmp.replace("ja", "[[Bild(Wiki/Vorlagen/StatusIcon/ja.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("nein")) {
        sTmp.replace("nein",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/nein.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("bedingt")) {
        sTmp.replace("bedingt",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/bedingt.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("alpha")) {
        sTmp.replace("alpha",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/alpha.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("buggy")) {
        sTmp.replace("buggy",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/buggy.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("manuell")) {
        sTmp.replace("manuell",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/manuell.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("plugin")) {
        sTmp.replace("plugin",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/plugin.png)]] ");
        sOutput += sTmp + " ";
      } else if (sListArgs[i].contains("unbekannt")) {
        sTmp.replace("unbekannt",
                     "[[Bild(Wiki/Vorlagen/StatusIcon/unbekannt.png)]] ");
        sOutput += sTmp + " ";
      }
    } else {
      sOutput += sListArgs[i] + " ";
    }
  }
  return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseTable(const QStringList &sListArgs) -> QString {
  QString sOutput("");
  QStringList sArgs(sListArgs);
  sArgs.prepend("DUMMY");  // "Needed" because of usage i-1 !!!
  QRegularExpressionMatch match;
  QRegularExpression tablePattern(
        QStringLiteral("\\<{1,1}[\\w\\s=.\\-\":;^|()]+\\>{1,1}"));
  QRegularExpression connectCells(QStringLiteral("-\\d{1,2}"));
  QRegularExpression connectRows(QStringLiteral("\\|\\d{1,2}"));
  QRegularExpression rowclassPattern(
        QStringLiteral("rowclass=\\\"[\\w.%\\-]+\\\""));
  QRegularExpression cellclassPattern(
        QStringLiteral("cellclass=\\\"[\\w.%\\-]+\\\""));
  QRegularExpression tableClassPattern(
        QStringLiteral("tableclass=\\\"[\\w\\s:;%#\\-]+\\\""));

  QRegularExpression cellStylePattern(
        QStringLiteral("cellstyle=\\\"[\\w\\s:;%#\\-]+\\\""));
  QRegularExpression rowStylePattern(
        QStringLiteral("rowstyle=\\\"[\\w\\s:;%#\\-]+\\\""));
  QRegularExpression tableStylePattern(
        QStringLiteral("tablestyle=\\\"[\\w\\s:;%#\\-]+\\\""));

  int nLength;
  QString sTmpCellStyle;
  QString sStyleInfo;
  QString sTmpTD;

  if (sArgs.length() >= 2) {
    QString sTmpClass("");
    if ((match = tableClassPattern.match(sArgs[1])).hasMatch()) {
      sTmpClass = match.captured();
      sTmpClass = " class=" + sTmpClass.remove("tableclass=");
    }
    sTmpCellStyle.clear();
    if ((match = tableStylePattern.match(sArgs[1])).hasMatch()) {
      sTmpCellStyle = match.captured();
      sTmpCellStyle = " style=" + sTmpCellStyle.remove("tablestyle=");
    }
    sOutput = "<table" + sTmpClass + sTmpCellStyle + ">\n<tbody>\n";
  } else {
    sOutput = "<table>\n<tbody>\n";
  }

  for (int i = 1; i < sArgs.length(); i++) {
    sTmpTD.clear();

    if (sArgs[i] == "+++") {  // New line
      sOutput += "</tr>\n";
    } else {  // New cell
      // Check if found style info is in reality a html text format
      bool bTextformat = false;
      for (const auto &sTmp : qAsConst(m_sListHtmlStart)) {
        if (sArgs[i].trimmed().startsWith(sTmp)) {
          bTextformat = true;
        }
      }

      // Found style info && pattern which was found is not
      // a <span class=...> element or html text format
      if ((match = tablePattern.match(sArgs[i])).hasMatch() &&
          !sArgs[i].trimmed().startsWith("<span") &&
          !bTextformat) {
        int nIndex = match.capturedStart();
        bool bCellStyleWasSet = false;
        nLength = match.capturedLength();
        sStyleInfo = match.captured();

        // Start tr
        if (i == 1 || sArgs[i-1] == "+++" ||
            rowclassPattern.match(sStyleInfo).hasMatch() ||
            rowStylePattern.match(sStyleInfo).hasMatch()) {
          sOutput += "<tr";
        }

        // Found row class info --> in tr
        if ((match = rowclassPattern.match(sStyleInfo)).hasMatch()) {
          sTmpCellStyle = match.captured();
          sOutput += " class=" + sTmpCellStyle.remove("rowclass=");
        }
        // Found row sytle info --> in tr
        if ((match = rowStylePattern.match(sStyleInfo)).hasMatch()) {
          sTmpCellStyle = match.captured();
          sOutput += " style=\"" +
              sTmpCellStyle.remove("rowstyle=").remove("\"") + "\"";
        }

        // Close tr
        if (i == 1 || sArgs[i-1] == "+++" ||
            rowclassPattern.match(sStyleInfo).hasMatch() ||
            rowStylePattern.match(sStyleInfo).hasMatch()) {
          sOutput += ">\n";
        }

        // Start td
        sOutput += "<td";

        // Found cellclass info
        if ((match = cellclassPattern.match(sStyleInfo)).hasMatch()) {
          sTmpCellStyle = match.captured();
          sTmpTD += " class=" + sTmpCellStyle.remove("cellclass=");
        }

        // Connect cells info (-integer, e.g. -3)
        if ((match = connectCells.match(sStyleInfo)).hasMatch()) {
          sTmpTD += " colspan=\"" + match.captured().remove("-") + "\"";
        }

        // Connect ROWS info (|integer, e.g. |2)
        if ((match = connectRows.match(sStyleInfo)).hasMatch()) {
          sTmpTD += " rowspan=\"" + match.captured().remove("|") + "\"";
        }

        // Cell style attributs
        if ((match = cellStylePattern.match(sStyleInfo)).hasMatch()) {
          sTmpTD += " style=\"" +
              match.captured().remove("cellstyle=").remove("\"");
          bCellStyleWasSet = true;
        }

        // Text align center
        if (sStyleInfo.contains("<:") ||
            sStyleInfo.contains(" : ") ||
            sStyleInfo.contains(":>")) {
          if (bCellStyleWasSet) {
            sTmpTD += " text-align: center;";
          } else {
            sTmpTD += " style=\"text-align: center;";
          }
          bCellStyleWasSet = true;
        }
        // Text align left
        if (sStyleInfo.contains("<(") ||
            sStyleInfo.contains("(") ||
            sStyleInfo.contains("(>")) {
          if (bCellStyleWasSet) {
            sTmpTD += " text-align: left;";
          } else {
            sTmpTD += " style=\"text-align: left;";
          }
          bCellStyleWasSet = true;
        }
        // Text align center
        if (sStyleInfo.contains("<)") ||
            sStyleInfo.contains(" ) ") ||
            sStyleInfo.contains(")>")) {
          if (bCellStyleWasSet) {
            sTmpTD += " text-align: right;";
          } else {
            sTmpTD += " style=\"text-align: right;";
          }
          bCellStyleWasSet = true;
        }
        // Text vertical align top
        if (sStyleInfo.contains("<^") ||
            sStyleInfo.contains(" ^ ") ||
            sStyleInfo.contains("^>")) {
          if (bCellStyleWasSet) {
            sTmpTD += " text-align: top;";
          } else {
            sTmpTD += " style=\"vertical-align: top;";
          }
          bCellStyleWasSet = true;
        }
        // Text vertical align bottom
        if (sStyleInfo.contains("<v") ||
            sStyleInfo.contains(" v ") ||
            sStyleInfo.contains("v>")) {
          if (bCellStyleWasSet) {
            sTmpTD += " text-align: bottom;";
          } else {
            sTmpTD += " style=\"vertical-align: bottom;";
          }
          bCellStyleWasSet = true;
        }

        // Closing style section
        if (bCellStyleWasSet) {
          sTmpTD += "\"";
        }

        // Remove style info (remove only, if line starts with
        // "<" otherwise element)
        if (sArgs[i].startsWith("<")) {
          sArgs[i].replace(nIndex, nLength, "");
        }

        sOutput += sTmpTD + ">" + sArgs[i] + "</td>\n";
      } else {  // Normal cell without style info
        if (i == 1) {
          sOutput += "<tr>\n";
        }
        if (sArgs[i-1] == "+++") {
          sOutput += "<tr>\n";
        }
        sOutput += "<td>" + sArgs[i] + "</td>\n";
      }
    }
  }
  return sOutput + "</tr></tbody>\n</table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseTested(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  if (!sListArgs.isEmpty()) {
    if (sListArgs[0].toLower() == QString("general").toLower()) {
      sOutput = QString::fromUtf8("Dieser Artikel ist größtenteils für "
                                  "alle Ubuntu-Versionen gültig.");
    } else {  // Article tested with Ubuntu versions
      QStringList sListTmp;
      sListTmp = sListArgs;
      sListTmp.sort();
      for (int i = sListArgs.size()-1; i >= 0; i--) {
        if (m_TestedWithMap.contains(sListTmp[i].toLower())) {
          sOutput += "\n * " + m_TestedWithMap.value(sListTmp[i].toLower());
        }
      }
      if (sOutput.isEmpty()) {
        sOutput = QString::fromUtf8("Dieser Artikel ist mit keiner aktuell "
                                    "unterstützten Ubuntu-Version getestet! "
                                    "Bitte teste diesen Artikel für eine "
                                    "Ubuntu-Version, welche aktuell "
                                    "unterstützt wird. Dazu sind die Hinweise "
                                    "[:Wiki/FAQ_-_häufig_gestellte_Fragen/"
                                    "#Wikiartikel-testen:zum Testen von "
                                    "Artikeln] zu beachten.");
      } else {
        sOutput += QString::fromUtf8("\n<hr>\n~-(Du möchtest den Artikel für "
                                     "eine weitere Ubuntu-Version testen? "
                                     "Mitarbeit im Wiki ist immer willkommen! "
                                     "Dazu sind die Hinweise [:Wiki/FAQ_-_"
                                     "häufig_gestellte_Fragen/#Wikiartikel-"
                                     "testen:zum Testen von Artikeln] zu "
                                     "beachten.)-~");
      }
      sOutput += "\n";
    }
  } else {
    sOutput = QString::fromUtf8("Dieser Artikel ist mit keiner aktuell "
                                "unterstützten Ubuntu-Version getestet! "
                                "Bitte teste diesen Artikel für eine "
                                "Ubuntu-Version, welche aktuell "
                                "unterstützt wird. Dazu sind die Hinweise "
                                "[:Wiki/FAQ_-_häufig_gestellte_Fragen/"
                                "#Wikiartikel-testen:zum Testen von "
                                "Artikeln] zu beachten.");
  }

  return ProvisionalTplParser::insertBox(
        "box tested_for",
        QString::fromUtf8("Dieser Artikel wurde für die folgenden "
                          "Ubuntu-Versionen getestet:"),
        sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseTestedUT(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  if (!sListArgs.isEmpty()) {
    if (sListArgs[0].toLower() == QString("general").toLower()) {
      sOutput = QString::fromUtf8("Dieser Artikel gilt für alle "
                                  "Versionen von Ubuntu Touch.");
    } else {  // Article tested with Ubuntu versions
      for (int i = 0; i < sListArgs.size(); i++) {
        sOutput += "\n * ";
        if (m_TestedWithTouchMap.contains(sListArgs[i].toLower())) {
          sOutput += m_TestedWithTouchMap.value(sListArgs[i]);
        } else {
          sOutput += sListArgs[i];
        }
      }
      sOutput += "\n";
    }
  } else {
    sOutput = QString::fromUtf8("Dieser Artikel ist mit keiner derzeit "
                                "unterstützten Ubuntu-Touch-Version "
                                "getestet! Bitte diesen Artikel testen!");
  }

  return ProvisionalTplParser::insertBox(
        "box ut_box",
        QString::fromUtf8("Dieser Artikel bezieht sich auf Ubuntu Touch, die "
                          "Variante von Ubuntu für mobile Geräte mit "
                          "Touchscreen."),
        sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseUnderConst(
    const QStringList &sListArgs) -> QString {
  QStringList sArgs = sListArgs;
  QString sOutput("");
  // Get and check date
  QString sDate("");
  if (!sArgs.isEmpty()) {
    // Extract date
    QStringList sListDate = sArgs[0].split(".");
    // Wrong date format
    if (3 != sListDate.size()) {
      sDate.clear();
    } else {  // Correct number of date elements
      // Wrong date
      if (sListDate[0].toInt() <= 0
          || sListDate[0].toInt() > 31
          || sListDate[1].toInt() <= 0
          || sListDate[1].toInt() > 12
          || sListDate[2].toInt() <= 0) {
        sDate.clear();
      } else {  // Correct date
        // Add 0 to date if < 10
        for (int i = 0; i < sListDate.size(); i++) {
          if (sListDate[i].toInt() < 10
              && !sListDate[i].startsWith("0")) {
            sListDate[i] = "0" + sListDate[i];
          }
        }
        sDate = sListDate[0] +"."+ sListDate[1] +"."+ sListDate[2];
      }
    }

    QString sUsers("");
    int iCntUser;
    if (sDate.isEmpty()) {
      // Entry with index 0 = first user (no correct date given)
      iCntUser = 0;
    } else {
      iCntUser = 1;  // Entry after date = first user
    }

    // Generate user list
    for (; iCntUser < sArgs.size(); iCntUser++) {
      sArgs[iCntUser].replace(" ", "_");  // Replace possible spaces
      sUsers += "[user:" + sArgs[iCntUser] + ":]";
      // Comma or "and" between users
      if (iCntUser == sArgs.size() - 2) {
        sUsers += " und ";
      } else if (iCntUser != sArgs.size() - 1) {
        sUsers += ", ";
      }
    }
    sOutput = QString::fromUtf8("Dieser Artikel wird momentan von %1 "
                                "erstellt.").arg(sUsers);

    if (!sDate.isEmpty()) {
      sOutput += " "
                 + QString::fromUtf8("Als Fertigstellungsdatum wurde der %1 "
                                     "angegeben.").arg(sDate);
    } else {
      sOutput += " "
                 + QString::fromUtf8("Solltest du dir nicht sicher sein, "
                                     "ob an dieser Anleitung noch gearbeitet "
                                     "wird, kontrolliere das Datum der "
                                     "letzten Änderung und entscheide, wie "
                                     "du weiter vorgehst.");
    }
  } else {  // No parameter given
    sOutput = QString::fromUtf8("Dieser Artikel wird momentan erstellt. "
                                "Solltest du dir nicht sicher sein, ob an "
                                "dieser Anleitung noch gearbeitet wird, "
                                "kontrolliere das Datum der letzten "
                                "Änderung und entscheide, wie du weiter "
                                "vorgehst.");
  }

  return ProvisionalTplParser::insertBox(
        "box workinprogress",
        QString::fromUtf8("Artikel in Arbeit"),
        sOutput,
        QString::fromUtf8("Insbesondere heißt das, dass dieser Artikel noch "
                          "nicht fertig ist und dass wichtige Teile fehlen, "
                          "oder sogar falsch sein können. Bitte diesen Artikel "
                          "nicht als Anleitung für Problemlösungen benutzen."));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseWarning(
    const QStringList &sListArgs) -> QString {
  QString sOutput("");
  for (const auto &s : sListArgs) {
    sOutput += s + " ";
  }
  return ProvisionalTplParser::insertBox("box warning",
                                         QString::fromUtf8("Achtung!"),
                                         sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto ProvisionalTplParser::parseWorkInProgr(
    const QStringList &sListArgs) -> QString {
  QStringList sArgs(sListArgs);
  QString sOutput = QString::fromUtf8(
                      "Dieser Artikel wird momentan überarbeitet.");

  // Correct number of elements?
  if (sArgs.size() >= 3) {
    // Replace possible spaces
    for (int i = 0; i < sArgs.size(); i++) {
      sArgs[i].replace(" ", "_");
    }

    sOutput += QString::fromUtf8(
                 "\n * Geplante Fertigstellung: %1").arg(sArgs[0]);
    sOutput += QString::fromUtf8(
                 "\n * Derzeit gültiger Artikel: [:%1:]").arg(sArgs[1]);

    // Generate user list
    QString sUsers("");
    for (int i = 2; i < sArgs.size(); i++) {
      sUsers += "[user:" + sArgs[i] + ":]";
      // Comma or "and" between users
      if (i == sArgs.size() - 2) {
        sUsers += " und ";
      } else if (i != sArgs.size() - 1) {
        sUsers += ", ";
      }
    }
    sOutput += QString::fromUtf8("\n * Bearbeiter: %1\n").arg(sUsers);
    sOutput += QString::fromUtf8("<p>Solltest du dir nicht sicher sein, ob "
                                 "an dieser Anleitung noch gearbeitet wird, "
                                 "kontrolliere das Datum der [:Baustelle/%1"
                                 "/a/log/:letzten Änderung] und entscheide, "
                                 "wie du weiter vorgehst.</p>\n").arg(sArgs[1]);
  }

  return ProvisionalTplParser::insertBox(
        "box workinprogress",
        QString::fromUtf8("Artikel wird überarbeitet"),
        sOutput,
        QString::fromUtf8("Insbesondere heißt das, dass dieser Artikel noch "
                          "nicht fertig ist und dass wichtige Teile fehlen "
                          "oder sogar falsch sein können. Bitte diesen Artikel "
                          "nicht als Anleitung für Problemlösungen benutzen!"));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Insert box
auto ProvisionalTplParser::insertBox(const QString &sClass,
                                     const QString &sHeadline,
                                     const QString &sContents,
                                     const QString &sRemark) -> QString {
  QString sReturn("");

  // Generate output
  sReturn = "<div class=\"" + sClass + "\">\n";
  sReturn += "<h3 class=\"" + sClass + "\">" + sHeadline + "</h3>\n";
  sReturn += "<div class=\"contents\">\n";
  sReturn += "<p>" + sContents + "</p>\n";
  // Remark available
  if (!sRemark.isEmpty() && sRemark != " ") {
    sReturn += QString::fromUtf8("<hr />\n<p><strong>Anmerkung:</strong> ")
               + sRemark + "</p>\n";
  }
  sReturn += "</div>\n"
             "</div>\n";

  return sReturn;
}
