/**
 * \file CProvisionalTplParser.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Provisional template parser. Only used as long as complete inyoka parsing
 * is not supperted.
 */

#include <QDebug>
#include <QFileInfo>
#include <QImage>

#include "./CProvisionalTplParser.h"

CProvisionalTplParser::CProvisionalTplParser(const QStringList &sListHtmlStart,
                                             const QDir &tmpFileOutputDir,
                                             const QDir &tmpImgDir)
    : m_sListHtmlStart(sListHtmlStart),
      m_tmpFileDir(tmpFileOutputDir),
      m_tmpImgDir(tmpImgDir) {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseTpl(const QStringList &sListArgs,
                                        const QString sCurrentFile) {
    m_sCurrentFile = sCurrentFile;

    QStringList sArgs = sListArgs;
    if (sArgs.size() >= 1) {
        if (sArgs[0].toLower() == QString::fromUtf8("Fortgeschritten").toLower()) {
            return this->parseAdvanced();
        } else if (sArgs[0].toLower() == QString::fromUtf8("Archiviert").toLower()) {
            sArgs.removeFirst();
            return this->parseArchived(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Befehl").toLower()) {
            sArgs.removeFirst();
            return this->parseBash(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Builddeps").toLower()) {
            sArgs.removeFirst();
            return this->parseBuilddeps(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Code").toLower()) {
            sArgs.removeFirst();
            return this->parseCode(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Kopie").toLower()) {
            sArgs.removeFirst();
            return this->parseCopy(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Experten").toLower()) {
            sArgs.removeFirst();
            return this->parseExperts(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Fehlerhaft").toLower()) {
            sArgs.removeFirst();
            return this->parseFixme(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Fremdquelle-auth").toLower()) {
            sArgs.removeFirst();
            return this->parseForeignAuth(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Fremdquelle").toLower()) {
            sArgs.removeFirst();
            return this->parseForeignSource(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Fremdpaket").toLower()) {
            sArgs.removeFirst();
            return this->parseForeignPackage(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Fremd").toLower()) {
            sArgs.removeFirst();
            return this->parseForeignWarning(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Icon-Übersicht").toLower()) {
            sArgs.removeFirst();
            return this->parseIconOverview(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("IkhayaAutor").toLower()) {
            sArgs.removeFirst();
            return this->parseIkhayaAuthor(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Ikhaya-Award").toLower()) {
            sArgs.removeFirst();
            return this->parseIkhayaAward(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Ikhayabild").toLower()) {
            sArgs.removeFirst();
            return this->parseIkhayaImage(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Ikhaya-Projektvorstellung").toLower()) {
            sArgs.removeFirst();
            return this->parseIkhayaProjectPresentation();
        } else if (sArgs[0].toLower() == QString::fromUtf8("Bildersammlung").toLower()) {
            sArgs.removeFirst();
            return this->parseImageCollection(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Bildunterschrift").toLower()) {
            sArgs.removeFirst();
            return this->parseImageSubscrition(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Ausbaufähig").toLower()) {
            sArgs.removeFirst();
            return this->parseImprovable(sArgs);
        } else if (sArgs[0].trimmed().toLower() == QString::fromUtf8("Tasten").toLower()) {
            sArgs.removeFirst();
            return this->parseKeys(sArgs);
        } else if (sArgs[0].trimmed().toLower() == QString::fromUtf8("Wissen").toLower()) {
            sArgs.removeFirst();
            return this->parseKnowledge(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Verlassen").toLower()) {
            sArgs.removeFirst();
            return this->parseLeft(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Hinweis").toLower()) {
            sArgs.removeFirst();
            return this->parseNotice(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Uebersicht").toLower()) {
            sArgs.removeFirst();
            return this->parseOverview(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Uebersicht2").toLower()) {
            sArgs.removeFirst();
            return this->parseOverview2(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Pakete").toLower()) {
            sArgs.removeFirst();
            return this->parsePackage(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Paketinstallation").toLower()) {
            sArgs.removeFirst();
            return this->parsePackageInstallation(sArgs);
        } else if (sArgs[0].trimmed().toLower() == QString::fromUtf8("Installbutton").toLower()) {
            sArgs.removeFirst();
            return this->parsePackageInstallButton(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("PPA").toLower()) {
            sArgs.removeFirst();
            return this->parsePPA(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Projekte").toLower()) {
            sArgs.removeFirst();
            return this->parseProjects(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Seitenleiste").toLower()) {
            sArgs.removeFirst();
            return this->parseSidebar(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Tabelle").toLower()) {
            sArgs.removeFirst();
            return this->parseTable(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Getestet").toLower()) {
            sArgs.removeFirst();
            return this->parseTested(sArgs);
        } else if (sArgs[0].trimmed().toLower() == QString::fromUtf8("Baustelle").toLower()
                || sArgs[0].trimmed().toLower() == QString::fromUtf8("InArbeit").toLower()) {
            sArgs.removeFirst();
            return this->parseUnderConstruction(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Warnung").toLower()) {
            sArgs.removeFirst();
            return this->parseWarning(sArgs);
        } else if (sArgs[0].toLower() == QString::fromUtf8("Überarbeitung").toLower()) {
            sArgs.removeFirst();
            return this->parseWorkInProgress(sArgs);
        }
    }
    return "";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseAdvanced() {
    return this->insertBox("box advanced",
                           QString::fromUtf8("Artikel für fortgeschrittene Anwender"),
                           QString::fromUtf8("Dieser Artikel erfordert mehr "
                           "Erfahrung im Umgang mit Linux und ist daher nur "
                           "für fortgeschrittene Benutzer gedacht."));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseArchived(const QStringList &sListArgs) {
    QString sRemark("");

    // Remark available?
    if (sListArgs.size() >= 1) {
        sRemark = sListArgs[0];
    }

    return this->insertBox("box improvable",
                           QString::fromUtf8("Archivierte Anleitung"),
                           QString::fromUtf8("Dieser Artikel wurde archiviert, "
                           "da er - oder Teile daraus - nur noch unter einer "
                           "älteren Ubuntu-Version nutzbar ist. Diese Anleitung "
                           "wird vom Wiki-Team weder auf Richtigkeit überprüft "
                           "noch anderweitig gepflegt. Zusätzlich wurde der "
                           "Artikel für weitere Änderungen gesperrt."),
                           sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseBash(const QStringList &sListArgs) {
    QString sOutput("<div class=\"bash\">\n"
                    "<div class=\"contents\">\n"
                    "<pre>");
    foreach (QString s, sListArgs) {
        sOutput += s.replace("<", "&lt;") + "\n";
    }
    return sOutput + "</pre>\n</div>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseBuilddeps(const QStringList &sListArgs) {
    QString sOutput(QString::fromUtf8("<div class=\"package-list\">\n"
                    "<div class=\"contents\">\n"
                    "<p>"
                    "Befehl zum Installieren der Build-Abhängigkeiten: "
                    "'''[#dummy apt-get]''' [#dummy aptitude]</p>\n"
                    "<div class=\"bash\">"
                    "<div class=\"contents\">\n"
                    "<pre class=\"notranslate\"> sudo apt-get build-dep"));
    foreach (QString s, sListArgs) {
        sOutput += " " + s.trimmed();
    }
    return sOutput + "</pre>\n</div>\n</div>\n</div>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseCode(const QStringList &sListArgs) {
    QString sOutput("<pre>");
    foreach (QString s, sListArgs) {
        sOutput += s + "\n";
    }
    return sOutput + "</pre>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseCopy(const QStringList &sListArgs) {
    QString sLink("");

    if (sListArgs.size() >= 1) {
        sLink = sListArgs[0];
    }

    return insertBox("box warning",
                     QString::fromUtf8("Achtung!"),
                     QString::fromUtf8("Diese Seite wird aktuell überarbeitet. "
                     "Bitte hier keine Änderungen mehr vornehmen, sondern in %1!"))
           .arg("[:Baustelle/" + sLink.replace(" ", "_") + ":]");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseExperts(const QStringList &sListArgs) {
    QString sOutput("");
    foreach (QString s, sListArgs) {
        sOutput += s + " ";
    }
    return insertBox("box experts", QString::fromUtf8("Experten-Info:"), sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseFixme(const QStringList &sListArgs) {
    QString sRemark("");

    // Remark available?
    if (sListArgs.size() >= 1) {
        sRemark = sListArgs[0];
    }

    return this->insertBox("box fixme",
                           QString::fromUtf8("Fehlerhafte Anleitung"),
                           QString::fromUtf8("Diese Anleitung ist fehlerhaft. "
                           "Wenn du weißt, wie du sie ausbessern kannst, nimm "
                           "dir bitte die Zeit und bessere sie aus."),
                           sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseForeignAuth(const QStringList &sListArgs) {
    QStringList sArgs(sListArgs);
    QString sOutput("");

    if (sArgs.size() >= 1) {
        // Key
        if (sArgs[0].startsWith("key")) {
            sArgs[0].remove("key");
            sArgs[0].remove(" ");

            sOutput = QString::fromUtf8("<p>Um die [:Fremdquellen:Fremdquelle] "
                      "zu authentifizieren, kann man den Signierungsschlüssel "
                      "mit folgendem Befehl importieren:");
            sOutput += "</p>\n"
                       "<div class=\"bash\">\n"
                       "<div class=\"contents\">\n"
                       "<pre> sudo apt-key adv --recv-keys --keyserver "
                       "keyserver.ubuntu.com " + sArgs[0]
                       + " </pre>\n"
                       "</div>\n"
                       "</div>\n";
        } else {  // Url
            sOutput = QString::fromUtf8("<p>Um die [:Fremdquellen:Fremdquelle] "
                      "zu authentifizieren, kann man entweder den [%1"
                      " Signierungsschlüssel herunterladen] und in der "
                      "[:Paketquellen_freischalten:Paketverwaltung hinzufügen] "
                      "oder folgenden Befehl ausführen:</p>\n").arg(sArgs[0]);
            sOutput += "<div class=\"bash\">\n"
                       "<div class=\"contents\">\n"
                       "<pre>wget -q " + sArgs[0] + " -O- | sudo "
                       "apt-key add - </pre>\n"
                       "</div>\n"
                       "</div>\n";
        }
    }
    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseForeignPackage(const QStringList &sListArgs) {
    QString sOutput("");

    if (sListArgs.size() >= 2) {
        // Case 1: [[Vorlage(Fremdpaket, Projekthoster, Projektname, Ubuntuversion(en))]]
        if (!(sListArgs[1].startsWith("http"))
                && sListArgs[1] != "dl") {  // LAUNCHPAD
            if (sListArgs[0].toLower() == "launchpad") {
                sOutput = QString::fromUtf8("<p>Beim [:Launchpad:]-Projekt "
                          "[launchpad:%1:] werden [launchpad:%2/+download:"
                          "DEB-Pakete] angeboten. ")
                        .arg(sListArgs[1]).arg(sListArgs[1]);
            } else if (sListArgs[0].toLower() == "sourceforge") {  // SOURCEFORGE
                sOutput = QString::fromUtf8("<p>Beim [wikipedia:SourceForge:]"
                          "-Projekt [sourceforge:%1:] werden "
                          "[sourceforge:%2/files/:DEB-Pakete] angeboten. ")
                        .arg(sListArgs[1]).arg(sListArgs[1]);
            } else if (sListArgs[0].toLower() == "googlecode") {  // GOOGLE CODE
                sOutput = QString::fromUtf8("<p>Beim [http://code.google.com/intl/de/ Google Code]"
                          " {de} -Projekt [googlecode:%1:] werden "
                          "[googlecode:%2/downloads/:DEB-Pakete] angeboten. ")
                        .arg(sListArgs[1]).arg(sListArgs[1]);
            }
        }

        // Case 2: [[Vorlage(Fremdpaket, "Anbieter", dl, URL zu einem Download, Ubuntuversion(en))]]
        else if (sListArgs[1].startsWith("dl")) {
            sOutput = QString::fromUtf8("<p>Von %1 werden folgende DEB-Pakete "
                                        "angeboten:</p>\n * [%2] {dl}\n")
                    .arg(sListArgs[0]).arg(sListArgs[2]);
        }

        // Case 3: [[Vorlage(Fremdpaket, "Anbieter", URL Downloadübersicht, Ubuntuversion(en))]]
        else {
            sOutput = QString::fromUtf8("<p>Von %1 werden [%2 DEB-Pakete] {dl} "
                                        "angeboten. ")
                    .arg(sListArgs[0]).arg(sListArgs[1]);
        }
    }

    // No ubuntu version
    if ((sListArgs.size() == 2
         && !sListArgs[1].startsWith("dl"))
            || (sListArgs.size() == 3
                && sListArgs[1].startsWith("dl"))) {
        sOutput += QString::fromUtf8("Die unterstützten Ubuntuversionen und "
                                     "Architekturen werden aufgelistet. ");
    } else {
        if (sListArgs.size() >= 2) {
            int i = 2;
            if (sListArgs[1].startsWith("dl")) {
                i = 3;
            }
            QString sUbuntuVersions("");
            for (int j = i; j < sListArgs.size(); j++) {
                sUbuntuVersions += sListArgs[j] + " ";
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
    sOutput += insertBox("box warning",
                         QString::fromUtf8("Hinweis!"),
                         QString::fromUtf8("[:Fremdquellen:Fremdpakete] können "
                                           "das System gefährden."));
    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseForeignSource(const QStringList &sListArgs) {
    QStringList sArgs(sListArgs);
    QString sOutput("");
    QStringList sListVersions;

    sOutput = QString::fromUtf8("<p>Um aus der [:Fremdquellen:Fremdquelle] zu "
                                "installieren, muss man unabhängig von der "
                                "Ubuntu-Version die folgende "
                                "[:Paketquellen_freischalten:]:</p>\n");

    sOutput += insertBox("box warning",
                         QString::fromUtf8("Hinweis!"),
                         QString::fromUtf8("Zusätzliche [:Fremdquellen:] können "
                                           "das System gefährden."));

    if (sArgs.size() >= 2) {
        sListVersions = sArgs[1].split(" ", QString::SkipEmptyParts);
        QString sVersions("");
        foreach (QString s, sListVersions) {
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

QString CProvisionalTplParser::parseForeignWarning(const QStringList &sListArgs) {
    QString sRemark("");
    QString sOutput("");

    if (sListArgs.size() >= 1) {
        // Package
        if (sListArgs[0].toLower() == QString::fromUtf8("Paket").toLower()) {
            sOutput = QString::fromUtf8("<p>[:Fremdquellen:Fremdpakete] können "
                                        "das System gefährden.</p>\n");
        } else if (sListArgs[0].toLower() == QString::fromUtf8("Quelle").toLower()) {
            // Source
            sOutput = QString::fromUtf8("<p>Zusätzliche [:Fremdquellen:] können "
                                        "das System gefährden.</p>\n");
        } else if (sListArgs[0].toLower() == QString::fromUtf8("Software").toLower()) {
            // Software
            sOutput = QString::fromUtf8("<p>[:Fremdsoftware:] kann das System "
                                        "gefährden.</p>\n");
        }
        // Remark available
        if (sListArgs.size() >= 2) {
            sRemark = sListArgs[1];
        }
    }

    return insertBox("box warning", QString::fromUtf8("Hinweis!"), sOutput, sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseIconOverview(const QStringList &sListArgs) {
    QString sOutput("<table style=\"width:95%\">\n<tbody>\n");
    QStringList sListTitle;
    QString sStyle("");
    QString sImage("");

    if (sListArgs.size() > 0) {
        sListTitle << sListArgs[0].split("/");
        if (sListTitle.size() > 1) {
            sStyle = sListTitle[1].trimmed() + "-";
        }
    }

    for (int i = 0; i < sListArgs.size(); i++) {
        if (0 == i) {
            sOutput += "<tr class=\"" + sStyle + "titel\">\n"
                       "<td colspan=\"4\">" + sListTitle[0] + "</td>\n</tr>\n"
                       "<tr class=\"" + sStyle + "kopf\">\n"
                       "<td style=\"width: 24%\">Icon</td>\n"
                       "<td style=\"width: 25%\">Dateiname</td>\n"
                       "<td style=\"width: 24%\">Icon</td>\n"
                       "<td style=\"width: 25%\">Dateiname</td>\n</tr>";
        }
        else
        {
            if (1 == i % 2) {
                sOutput += "<tr>\n";
            }
            sOutput += "<td>[[Bild(Wiki/Icons/" + sListArgs[i] + ", x32, center)]]</td>"
                    "<td><strong>" + sListArgs[i] + "</strong></td>\n";
            /*
            sOutput += "<td><a href=\"" + sImage + "\" rel=\"nofollow\" "
                       "class=\"external\"><img src=\"" + sImage + "\" "
                       "alt=\"" + sImage + "\" class=\"image-center\"></a></td>"
                    "<td><strong>" + sListArgs[i] + "</strong></td>\n";
            */

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

QString CProvisionalTplParser::parseIkhayaAuthor(const QStringList &sListArgs) {
    QString sOutput("");

    foreach (QString s, sListArgs) {
        sOutput += s + " ";
    }

    return this->insertBox("box tested_for",
                           QString::fromUtf8("Über den Autor"),
                           sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseIkhayaAward(const QStringList &sListArgs) {
    QString sOutput("");

    foreach (QString s, sListArgs) {
        sOutput += s + " ";
    }

    return this->insertBox("box award",
                           QString::fromUtf8("Award:"),
                           sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseIkhayaImage(const QStringList &sListArgs) {
    QString sImage("");
    QString sText("");
    QString sStyle("width: auto; float: left; margin-right: 1em");

    if (sListArgs.size() > 1) {
        sImage = sListArgs[0].trimmed();
        sText = sListArgs[1].trimmed();
        if (sImage.contains("right") || sText.contains("right")) {
            sImage.remove("right").trimmed();
            sText.remove("right").trimmed();
            sStyle = "width: auto; float: right; margin-left: 1em";
        }
    }

    return "<table style=\"" + sStyle + "\">\n<tbody>\n<tr>\n"
           "<td style=\"text-align: center; background-color: #F2F2F2\">\n"
           "<img src=\"" + sImage + "\" alt=\"" + sImage + "\" class=\"image-default\">"
           "</td>\n</tr><tr>\n<td style=\"text-align: center; background-color: #F9EAAF\">"
           + sText + "</td>\n</tr>\n</tbody></table\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseIkhayaProjectPresentation() {
    return this->insertBox("box notice",
                           QString::fromUtf8("Hinweis"),
                           QString::fromUtf8("Dieser Artikel gehört zur Reihe "
                           "der Projektvorstellungen, die in unregelmäßigen "
                           "Abständen vergleichsweise unbekannte Projekte "
                           "vorstellt, um sie so einem breiteren Publikum näher "
                           "zu bringen. Diese Artikelreihe existiert seit 2008. "
                           "Alle Projektvorstellungen können im Wikiartikel "
                           "[:ubuntuusers/Ikhayateam/Projektvorstellungen:"
                           "Projektvorstellungen] eingesehen werden."));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseImageCollection(const QStringList &sListArgs) {
    QString sOutput("");
    QString sImageUrl("");
    QString sDescription("");
    QString sImageCollHeight("140");
    QString sImageCollAlign("");
    double iImgHeight, iImgWidth;
    bool bContinue(false);

    QString sImagePath("");
    if ("" != m_sCurrentFile) {
        QFileInfo fiArticleFile(m_sCurrentFile);
        sImagePath = fiArticleFile.absolutePath();
    }

    // Get height / align
    for (int i = 0; i < sListArgs.size(); i++) {
        if (0 == i || 1 == i) {
            if ("left" == sListArgs[i] || "right" == sListArgs[i]) {
                sImageCollAlign = sListArgs[i];
            } else if (0 != sListArgs[i].toFloat()) {
                sImageCollHeight = sListArgs[i];
            }
        } else {
            break;
        }
    }


    if ("" == sImageCollAlign) {  // With word wrap
        sOutput = "<div style=\"clear: both\">\n<div "
                "class=\"contents\"> </div>\n</div>";
    } else {  // In continuous text (sImageCollAlign = left | rigth)
        sOutput = "<table style=\"float: " + sImageCollAlign
                + "; clear: both; border: none\">\n<tbody>\n<tr "
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
            } else if (0 != sListArgs[i].toFloat()) {
                continue;
            }
        }

        sImageUrl = sListArgs[i];
        if (sImageUrl.startsWith("Wiki/")) {
            sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;
        } else if ("" != sImagePath &&
                   QFile(sImagePath + "/" + sImageUrl).exists()) {
            sImageUrl = sImagePath + "/" + sImageUrl;
        } else {
            sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
        }

        iImgHeight = QImage(sImageUrl).height();
        iImgWidth = static_cast<double>(QImage(sImageUrl).width()) / (iImgHeight / sImageCollHeight.toDouble());

        if ("" == sImageCollAlign) {  // With word wrap
            if ((i+1) < sListArgs.size()) {
                sDescription = sListArgs[i+1];
                bContinue = true;
            } else {
                sDescription = "";
            }

            sOutput += "<table style=\"float: left; margin: 10px 5px; border: none\">\n<tbody>\n<tr>\n"
                    "<td style=\"text-align: center; background-color: #E2C889; border: none\">"
                    "<a href=\"" + sImageUrl + "\" class=\"crosslink\">"
                    "<img src=\"" + sImageUrl + "\" alt=\"" + sImageUrl + "\" class=\"image-default\" "
                    "width=\"" + QString::number(static_cast<int>(iImgWidth)) + "\" height=\"" + sImageCollHeight + "\"/></a></td>\n</tr>\n"
                    "<tr>\n<td style=\"text-align: center; background-color: #F9EAAF; border: none\">" + sDescription + "</td>\n</tr>\n"
                    "</tbody>\n</table>\n";
        } else {  // In continuous text (sImageCollAlign = left | rigth)
            sOutput += "<td style=\"text-align: center; border-width: 0 10px 0 0; border-color: #FFFFFF \">\n"
                    "<img src=\"" + sImageUrl + "\" alt=\"" + sImageUrl + "\" class=\"image-default\" "
                    "width=\"" + QString::number(static_cast<int>(iImgWidth)) + "\" height=\"" + sImageCollHeight + "\"/></td>\n";

            bContinue = true;
        }
    }

    if ("" == sImageCollAlign) {  // With word wrap
        sOutput += "<div style=\"clear: both\">\n<div class=\"contents\"> "
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
                } else if (0 != sListArgs[i].toFloat()) {
                    continue;
                }
            }

            if ((i+1) < sListArgs.size()) {
                sDescription = sListArgs[i+1];
                bContinue = true;
            } else {
                sDescription = "";
            }

            sOutput += "<td style=\"text-align: center; border-width: "
                    "0 10px 0 0; border-color: #FFFFFF\">\n"
                    + sDescription + "</td>\n";
        }
        sOutput += "</tr></tbody>\n</table>\n";
    }

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseImageSubscrition(const QStringList &sListArgs) {
    QString sOutput("");
    QString sImageUrl("");
    QString sImageWidth("");
    QString sImageDescription("");
    QString sImageAlign("left");
    QString sImageStyle("");
    double iImgHeight, iImgWidth;

    QString sImagePath("");
    if ("" != m_sCurrentFile) {
        QFileInfo fiArticleFile(m_sCurrentFile);
        sImagePath = fiArticleFile.absolutePath();
    }

    sImageUrl = sListArgs[0].trimmed();
    if (sImageUrl.startsWith("Wiki/")) {
        sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;
    } else if ("" != sImagePath &&
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
    if (sImageWidth != "") {
        iImgHeight = static_cast<double>(QImage(sImageUrl).height()) / (iImgWidth / sImageWidth.toDouble());
    } else {
        // Default
        sImageWidth = "140";
        iImgHeight = static_cast<double>(QImage(sImageUrl).height()) / (iImgWidth / 140);
    }

    sOutput = "<table style=\"float: " + sImageAlign
            + "; clear: both; border: none\">\n<tbody>\n";

    // No style info -> default
    if (sImageStyle == "") {
        sOutput += "<tr class=\"titel\">\n";
    } else {
        sOutput += "<tr class=\"" + sImageStyle + "-titel\">\n";
    }

    sOutput += "<td>\n<a href=\"" + sImageUrl + "\" "
               "class=\"crosslink\">\n" + "<img src=\"" + sImageUrl + "\" "
               "alt=\"" + sImageUrl + "\" class=\"image-default\" "
               + "height=\"" + QString::number(static_cast<int>(iImgHeight))
               + "\" width=\"" + sImageWidth + "\"/>\n</a>\n"
               + "</td>\n</tr>\n";

    // No style info -> default
    if (sImageStyle == "") {
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

QString CProvisionalTplParser::parseImprovable(const QStringList &sListArgs) {
    QString sRemark("");

    // Remark available?
    if (sListArgs.size() >= 1) {
        sRemark = sListArgs[0];
    }

    return this->insertBox("box improvable",
                           QString::fromUtf8("Ausbaufähige Anleitung"),
                           QString::fromUtf8("Dieser Anleitung fehlen noch "
                           "einige Informationen. Wenn Du etwas verbessern "
                           "kannst, dann editiere den Beitrag, um die Qualität "
                           "des Wikis noch weiter zu verbessern."),
                           sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseKeys(const QStringList &sListArgs) {
    QString sTmpKey("");
    QString sTmpKeyCode("");
    bool bIsMouseButton;
    QStringList sListTmpKeys;

    if (sListArgs.size() > 0) {
        sTmpKey = sListArgs[0];
    }

    sListTmpKeys = sTmpKey.split("+");
    sTmpKey.clear();

    for (int i = 0; i < sListTmpKeys.size(); i++) {
        bIsMouseButton = false;
        // Remove possible spaces before and after string
        sListTmpKeys[i] = sListTmpKeys[i].trimmed();
        sListTmpKeys[i][0] = sListTmpKeys[i][0].toLower();

        if (sListTmpKeys[i] == "backspace"
                || sListTmpKeys[i] == QString::fromUtf8("löschen")
                || sListTmpKeys[i] == QString::fromUtf8("rückschritt")) {
            sTmpKeyCode = "&#9003;";
        } else if (sListTmpKeys[i] == "ctrl") {
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
        } else if (sListTmpKeys[i] == "eckig_auf") {
            sTmpKeyCode = "[";
        } else if (sListTmpKeys[i] == "eckig_zu") {
            sTmpKeyCode = "]";
        } else if (sListTmpKeys[i] == "bild auf"
                   || sListTmpKeys[i] == "bild-auf"
                   || sListTmpKeys[i] == "bild-rauf") {
            sTmpKeyCode = "Bild &uarr;";
        } else if (sListTmpKeys[i] == "bild ab"
                   || sListTmpKeys[i] == "bild-ab"
                   || sListTmpKeys[i] == "bild-runter") {
            sTmpKeyCode = "Bild &darr;";
        } else if (sListTmpKeys[i] == "print"
                   || sListTmpKeys[i] == "prtsc"
                   || sListTmpKeys[i] == "sysrq") {
            sTmpKeyCode = "Druck";
        } else if (sListTmpKeys[i] == "mac"
                   || sListTmpKeys[i] == "appel"
                   || sListTmpKeys[i] == "apfel"
                   || sListTmpKeys[i] == "cmd") {
            sTmpKeyCode = "&#8984;";
        } else if (sListTmpKeys[i] == "caps"
                   || sListTmpKeys[i] == "feststell"
                   || sListTmpKeys[i] == "feststelltaste"
                   || sListTmpKeys[i] == QString::fromUtf8("groß")) {
            sTmpKeyCode = "&dArr;";
        } else if (sListTmpKeys[i] == "shift"
                   || sListTmpKeys[i] == "umschalt"
                   || sListTmpKeys[i] == "umsch") {
            sTmpKeyCode = "&uArr;";
        } else if (sListTmpKeys[i] == "at") {
            sTmpKeyCode = "&#64;";
        } else if (sListTmpKeys[i] == "cherry"
                   || sListTmpKeys[i] == "keyman") {
            sTmpKeyCode = "Keym&#64;n";
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
                   || sListTmpKeys[i] == "pfeil-hoch") {
            sTmpKeyCode = "&uarr;";
        } else if (sListTmpKeys[i] == "runter"
                   || sListTmpKeys[i] == "ab"
                   || sListTmpKeys[i] == "down"
                   || sListTmpKeys[i] == "pfeil-runter"
                   || sListTmpKeys[i] == "pfeil-ab") {
            sTmpKeyCode = "&darr;";
        } else if (sListTmpKeys[i] == "links"
                   || sListTmpKeys[i] == "left"
                   || sListTmpKeys[i] == "pfeil-links") {
            sTmpKeyCode = "&larr;";
        } else if (sListTmpKeys[i] == "rechts"
                   || sListTmpKeys[i] == "right"
                   || sListTmpKeys[i] == "pfeil-rechts") {
            sTmpKeyCode = "&rarr;";
        } else if (sListTmpKeys[i] == "\",\""
                   || sListTmpKeys[i] == "\',\'") {
            sTmpKeyCode = ",";
        } else if (sListTmpKeys[i] == "minus") {
            sTmpKeyCode = "-";
        } else if (sListTmpKeys[i] == "plus") {
            sTmpKeyCode = "+";
        } else if (sListTmpKeys[i] == "\"`\"") {
            sTmpKeyCode = "`";
        } else if (sListTmpKeys[i] == "sz") {
            sTmpKeyCode = "&szlig;";
        } else if (sListTmpKeys[i] == "gleich") {
            sTmpKeyCode = "=";
        } else if (sListTmpKeys[i] == "num"
                   || sListTmpKeys[i] == "num-taste"
                   || sListTmpKeys[i] == "num-Taste"
                   || sListTmpKeys[i] == "num-lock-taste"
                   || sListTmpKeys[i] == "num-Lock-Taste") {
            sTmpKeyCode = "num &dArr;";
        } else if (sListTmpKeys[i] == "fragezeichen") {
            sTmpKeyCode = "?";
        } else if (sListTmpKeys[i] == "break") {
            sTmpKeyCode = "Pause";
        } else if (sListTmpKeys[i] == "rollen"
                   || sListTmpKeys[i] == "bildlauf") {
            sTmpKeyCode = "&dArr; Rollen";
        } else if (sListTmpKeys[i] == "slash") {
            sTmpKeyCode = "/";
        } else if (sListTmpKeys[i] == "any") {
            sTmpKeyCode = "ANY KEY";
        } else if (sListTmpKeys[i] == "panic") {
            sTmpKeyCode = "PANIC";
        } else if (sListTmpKeys[i] == "koelsch") {
            sTmpKeyCode = "K&ouml;lsch";
        } else {
            if (sListTmpKeys[i] == "lmt"
                    || sListTmpKeys[i] == "lmb") {
                bIsMouseButton = true;
                sTmpKey += "<img src=\"img/wiki/mouse_left.png\" alt="
                        "\"linke Maustaste\" class=\"image-default\" />";
            } else if (sListTmpKeys[i] == "rmt"
                       || sListTmpKeys[i] == "rmb") {
                bIsMouseButton = true;
                sTmpKey += "<img src=\"img/wiki/mouse_right.png\" alt="
                        "\"rechte Maustaste\" class=\"image-default\" />";
            } else if (sListTmpKeys[i] == "mmt"
                       || sListTmpKeys[i] == "mmb") {
                bIsMouseButton = true;
                sTmpKey += "<img src=\"img/wiki/mouse_midd.png\" alt="
                        "\"mittlere Maustaste\" class=\"image-default\" />";
            } else {
                // Everything else: First character to Upper
                // first characters had been changed to lower at beginning
                sListTmpKeys[i][0] = sListTmpKeys[i][0].toUpper();
                sTmpKeyCode = sListTmpKeys[i];
            }
        }

        if (!bIsMouseButton) {
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

QString CProvisionalTplParser::parseKnowledge(const QStringList &sListArgs) {
    QString sOutput("");
    QString sTmp("");
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

    return insertBox("box knowledge",
                     QString::fromUtf8("Zum Verständnis dieses Artikels sind "
                                       "folgende Seiten hilfreich:"),
                     sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseLeft(const QStringList &sListArgs) {
    QString sRemark("");

    // Remark available?
    if (sListArgs.size() >= 1) {
        sRemark = sListArgs[0];
    }

    return this->insertBox("box left",
                           QString::fromUtf8("Verlassene Anleitung"),
                           QString::fromUtf8("Dieser Artikel wurde von seinem "
                           "Ersteller verlassen und wird nicht mehr weiter von "
                           "ihm gepflegt. Wenn Du den Artikel fertigstellen "
                           "oder erweitern kannst, dann bessere ihn bitte aus."),
                           sRemark);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseNotice(const QStringList &sListArgs) {
    QString sOutput("");
    foreach (QString s, sListArgs) {
        sOutput += s + " ";
    }
    return insertBox("box notice", QString::fromUtf8("Hinweis:"), sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseOverview(const QStringList &sListArgs) {
    QList <QStringList> sListList;
    QStringList sListTmp;
    QString sOutput("<table style=\"border: none; margin-top: 0; margin-right: 0\">\n<tbody>");

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
                if (sListList[i].size() >= 1) {
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
                        "background-color: #F9EAAF; \">" + sListList[i][k].trimmed() +
                        "</td>\n</tr>\n";
            }
        }
    }

    return sOutput + "\n<tbody>\n</table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseOverview2(const QStringList &sListArgs) {
    QList <QStringList> sListList;
    QStringList sListTmp;
    QString sOutput("<table style=\"width: 98%; border: none;\">\n<tbody>");
    QString sStyle("");

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
                if (sListList[i].size() >= 1) {
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
                sOutput += "<td style=\"" + sStyle + "background-color: #F9EAAF; "
                           "border-color: #FFFFFF; \">" + sListList[i][k] + "</td>\n";
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

QString CProvisionalTplParser::parsePackage(const QStringList &sListArgs) {
    QString sOutput(QString::fromUtf8("<div class=\"package-list\">\n"
                    "<div class=\"contents\">\n"
                    "<p>Paketliste zum Kopieren: "
                    "'''[#dummy apt-get]''' [#dummy aptitude]</p>\n"
                    "<div class=\"bash\">"
                    "<div class=\"contents\">\n"
                    "<pre class=\"notranslate\"> sudo apt-get install"));
    foreach (QString s, sListArgs) {
        sOutput += " " + s;
    }
    return sOutput + "</pre>\n</div>\n</div>\n</div>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parsePackageInstallation(const QStringList &sListArgs) {
    QStringList sListPackages;
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
    sOutput += QString::fromUtf8("\n[[Vorlage(Installbutton,%1)]]").arg(sPackages);

    // Copy console output
    sOutput += "<div class=\"package-list\">\n"
               "<div class=\"contents\">\n"
               "<p>";
    sOutput += QString::fromUtf8("Paketliste zum Kopieren:");
    sOutput += " '''[#dummy apt-get]''' [#dummy aptitude]</p>\n"
               "<div class=\"bash\">"
               "<div class=\"contents\">\n"
               "<pre class=\"notranslate\"> sudo apt-get install";
    for (int i = 0; i < sListPackages.size(); i++) {
        sOutput += " " + sListPackages[i].trimmed();
    }
    return sOutput + "</pre>\n</div>\n</div>\n</div>\n</div>\n";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parsePackageInstallButton(const QStringList &sListArgs) {
    QString sTmp("");
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
               "<img src=\"img/wiki/button.png\" "
               "alt=\"Wiki-Installbutton\" class=\"image-default\" /></a> "
               "mit [:apturl:]</p>";

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parsePPA(const QStringList &sListArgs) {
    QStringList sArgs(sListArgs);
    QString sOutsideBox("");
    QString sRemark("");
    QString sUser("");
    QString sPPA("");
    QString sOutput("");

    if (sArgs.size() >= 1) {
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
        sOutsideBox = QString::fromUtf8("<p>Adresszeile zum "
                      "[:Paketquellen_freischalten/PPA#PPA-hinzufuegen:Hinzufügen] "
                      "des PPAs:</p>");
        sOutsideBox += "\n * <strong>ppa:" + sUser + "/" + sPPA + "</strong>\n";

        sOutput = QString::fromUtf8("Zusätzliche [:Fremdquellen:] können das "
                                    "System gefährden.");
        sRemark = QString::fromUtf8("Ein PPA unterstützt nicht zwangsläufig "
                  "alle Ubuntu-Versionen. Weitere Informationen sind der "
                  "[[Bild(../img/interwiki/ppa.png)]] [https://launchpad.net/~"
                  "%1/+archive/%2 PPA-Beschreibung] des "
                  "Eigentümers/Teams [lpuser:%3:] zu entnehmen.")
                .arg(sUser).arg(sPPA).arg(sUser);
    }

    return sOutsideBox + insertBox("box warning",
                                   QString::fromUtf8("Hinweis!"),
                                   sOutput, sRemark)
            + QString::fromUtf8("<p>Damit Pakete aus dem PPA genutzt werden "
                                "können, müssen die Paketquellen neu "
                                "[:apt/apt-get#apt-get-update:eingelesen] "
                                "werden.</p>");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseProjects(const QStringList &sListArgs) {
    QList <QStringList> sListList;
    QStringList sListTmp;
    QString sOutput("<table style=\"width: 96%; margin-left: 1em\">\n<tbody>");
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
            if (sListList[i].size() >= 1) {
                sOutput += "<tr class=\"verlauf\">\n<td colspan=\"2\">"
                        + sListList[i][0] + "</td>\n</tr>";
            }
        } else {
            sImage = "";
            sTitle = "";
            sText = "";
            sLinks = "";
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
                    "<td style=\"text-align: center; border-right-color: #f2f2f2; font-weight: bold\">"
                    "[[Bild(" + sImage + ", 32)]][[BR]]" + sTitle + "</td>"
                    "<td style=\"padding-top: 0.8em; border-left-color: #f2f2f2\">"
                    + sText + "\n<div style=\"color: #000000; margin-left: 1em\">\n"
                    "<div class=\"contents\">\n<p>" + sLinks + "</p>\n"
                    "</div>\n</div>""</td></tr>";
        }
    }

    return sOutput + "</tbody></table>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseSidebar(const QStringList &sListArgs) {
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
                if (sListList[i].size() >= 1) {
                    sOutput += "<tr>\n<td style=\"text-align: center; "
                            "background-color: #E2C889; font-size: 1.1em; "
                            "border: none\">" + sListList[i][k].trimmed() +
                            "</td>\n</tr>\n";
                }
            } else if (sListList[i][k].contains("[[BR]]")
                       || k == sListList[i].size() - 1) {
                sListTmp = sListList[i][k].split("[[BR]]", QString::SkipEmptyParts);
                foreach (QString s, sListTmp) {
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

QString CProvisionalTplParser::parseTable(const QStringList &sListArgs) {
    QString sOutput("");
    QStringList sArgs(sListArgs);
    sArgs.prepend("DUMMY");  // "Needed" because of usage i-1 !!!
    QRegExp tablePattern("\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
    QRegExp connectCells("-\\d{1,2}");
    QRegExp connectRows("\\|\\d{1,2}");
    QRegExp rowclassPattern("rowclass=\\\"[\\w.%-]+\\\"");
    QRegExp cellclassPattern("cellclass=\\\"[\\w.%-]+\\\"");

    bool bCellStyleWasSet;
    QRegExp cellStylePattern("cellstyle=\\\"[\\w\\s:;%#-]+\\\"");
    QRegExp rowStylePattern("rowstyle=\\\"[\\w\\s:;%#-]+\\\"");
    QRegExp tableStylePattern("tablestyle=\\\"[\\w\\s:;%#-]+\\\"");

    int iLength;
    QString sTmpCellStyle, sStyleInfo, sTmpTD;

    if (sArgs.length() >= 2) {
        if (tableStylePattern.indexIn(sArgs[1]) >= 0) {
            sTmpCellStyle = tableStylePattern.cap();
            sOutput = "<table style=" + sTmpCellStyle.remove("tablestyle=")
                    + ">\n<tbody>\n";
        } else {
            sOutput = "<table>\n<tbody>\n";
        }
    } else {
        sOutput = "<table>\n<tbody>\n";
    }

    for (int i = 1; i < sArgs.length(); i++) {
        bCellStyleWasSet = false;
        sTmpTD.clear();

        if (sArgs[i] == "+++") {  // New line
            sOutput += "</tr>\n";
        } else {  // New cell
            int myindex = tablePattern.indexIn(sArgs[i]);

            // Check if found style info is in reality a html text format
            bool bTextformat = false;
            foreach (const QString &sTmp, m_sListHtmlStart) {
                if (sArgs[i].trimmed().startsWith(sTmp)) {
                    bTextformat = true;
                }
            }

            // Found style info && pattern which was found is not
            // a <span class=...> element or html text format
            if (myindex >= 0
                    && !sArgs[i].trimmed().startsWith("<span")
                    && !bTextformat) {
                iLength = tablePattern.matchedLength();
                sStyleInfo = tablePattern.cap();

                // Start tr
                if (i == 1 || sArgs[i-1] == "+++"
                        || rowclassPattern.indexIn(sStyleInfo) >= 0
                        || rowStylePattern.indexIn(sStyleInfo) >= 0) {
                    sOutput += "<tr";
                }

                // Found row class info --> in tr
                if (rowclassPattern.indexIn(sStyleInfo) >= 0) {
                    sTmpCellStyle = rowclassPattern.cap();
                    sOutput += " class="
                            + sTmpCellStyle.remove("rowclass=");
                }
                // Found row sytle info --> in tr
                if (rowStylePattern.indexIn(sStyleInfo) >= 0) {
                    sTmpCellStyle = rowStylePattern.cap();
                    sOutput += " style=\""
                            + sTmpCellStyle.remove("rowstyle=")
                            .remove("\"") + "\"";
                }

                // Close tr
                if (i == 1 || sArgs[i-1] == "+++"
                    || rowclassPattern.indexIn(sStyleInfo) >= 0
                    || rowStylePattern.indexIn(sStyleInfo) >= 0) {
                    sOutput += ">\n";
                }

                // Start td
                sOutput += "<td";

                // Found cellclass info
                if (cellclassPattern.indexIn(sStyleInfo) >= 0) {
                    sTmpCellStyle = cellclassPattern.cap();
                    sTmpTD += " class="
                            + sTmpCellStyle.remove("cellclass=");
                }

                // Connect cells info (-integer, e.g. -3)
                if (connectCells.indexIn(sStyleInfo) >= 0) {
                    sTmpTD += " colspan=\""
                            + connectCells.cap().remove("-") + "\"";
                }

                // Connect ROWS info (|integer, e.g. |2)
                if (connectRows.indexIn(sStyleInfo) >= 0) {
                    sTmpTD += " rowspan=\""
                            + connectRows.cap().remove("|") + "\"";
                }

                // Cell style attributs
                if (cellStylePattern.indexIn(sStyleInfo) >= 0) {
                    sTmpTD += " style=\""
                            + cellStylePattern.cap().remove("cellstyle=")
                            .remove("\"");
                    bCellStyleWasSet = true;
                }

                // Text align center
                if (sStyleInfo.contains("<:")
                        || sStyleInfo.contains(" : ")
                        || sStyleInfo.contains(":>")) {
                    if (bCellStyleWasSet) {
                        sTmpTD += " text-align: center;";
                    } else {
                        sTmpTD += " style=\"text-align: center;";
                    }
                    bCellStyleWasSet = true;
                }
                // Text align left
                if (sStyleInfo.contains("<(")
                        || sStyleInfo.contains("(")
                        || sStyleInfo.contains("(>")) {
                    if (bCellStyleWasSet) {
                        sTmpTD += " text-align: left;";
                    } else {
                        sTmpTD += " style=\"text-align: left;";
                    }
                    bCellStyleWasSet = true;
                }
                // Text align center
                if (sStyleInfo.contains("<)")
                        || sStyleInfo.contains(" ) ")
                        || sStyleInfo.contains(")>")) {
                    if (bCellStyleWasSet) {
                        sTmpTD += " text-align: right;";
                    } else {
                        sTmpTD += " style=\"text-align: right;";
                    }
                    bCellStyleWasSet = true;
                }
                // Text vertical align top
                if (sStyleInfo.contains("<^")
                        || sStyleInfo.contains(" ^ ")
                        || sStyleInfo.contains("^>")) {
                    if (bCellStyleWasSet) {
                        sTmpTD += " text-align: top;";
                    } else {
                        sTmpTD += " style=\"vertical-align: top;";
                    }
                    bCellStyleWasSet = true;
                }
                // Text vertical align bottom
                if (sStyleInfo.contains("<v")
                        || sStyleInfo.contains(" v ")
                        || sStyleInfo.contains("v>")) {
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
                    sArgs[i].replace(myindex, iLength, "");
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

QString CProvisionalTplParser::parseTested(const QStringList &sListArgs) {
    QString sOutput("");
    if (sListArgs.size() >= 1) {
        if (sListArgs[0].toLower() == QString("general").toLower()) {
            sOutput = QString::fromUtf8("Dieser Artikel ist größtenteils für "
                                        "alle Ubuntu-Versionen gültig.");
        } else {  // Article tested with ubuntu versions
            for (int i = 0; i < sListArgs.size(); i++) {
                sOutput += "\n * ";
                if (sListArgs[i].toLower() == "saucy") {
                    sOutput += "[:Saucy_Salamander:Ubuntu 13.10] "
                               "\"Saucy Salamander\"";
                } else if (sListArgs[i].toLower() == "raring") {
                    sOutput += "[:Raring_Ringtail:Ubuntu 13.04] "
                               "\"Raring Ringtail\"";
                } else if (sListArgs[i].toLower() == "quantal") {
                    sOutput += "[:Quantal_Quetzal:Ubuntu 12.10] "
                               "\"Quantal Quetzal\"";
                } else if (sListArgs[i].toLower() == "precise") {
                    sOutput += "[:Precise_Pangolin:Ubuntu 12.04] "
                               "\"Precise Pangolin\"";
                } else if (sListArgs[i].toLower() == "oneiric") {
                    sOutput += "[:Oneiric_Ocelot:Ubuntu 11.10] "
                               "\"Oneiric Ocelot\"";
                } else if (sListArgs[i].toLower() == "natty") {
                    sOutput += "[:Natty_Narwhal:Ubuntu 11.04] "
                               "\"Natty Narwhal\"";
                } else if (sListArgs[i].toLower() == "maverick") {
                    sOutput += "[:Maverick_Meerkat:Ubuntu 10.10] "
                               "\"Maverick Meerkat\"";
                } else if (sListArgs[i].toLower() == "lucid") {
                    sOutput += "[:Lucid_Lynx:Ubuntu 10.04] "
                               "\"Lucid Lynx\"";
                } else if (sListArgs[i].toLower() == "hardy") {
                    sOutput += "[:Hardy_Heron:Ubuntu 8.04] "
                               "\"Hardy Heron\"";
                } else {
                    sOutput += sListArgs[i];
                }
            }
            sOutput += "\n";
        }
    } else {
        sOutput = QString::fromUtf8("Dieser Artikel ist mit keiner aktuell "
                                    "unterstützten Ubuntu-Version getestet! "
                                    "Bitte diesen Artikel testen und das "
                                    "getestet-Tag entsprechend anpassen.");
    }

    return this->insertBox("box tested_for",
                           QString::fromUtf8("Dieser Artikel wurde für die "
                                             "folgenden Ubuntu-Versionen "
                                             "getestet:"),
                           sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseUnderConstruction(const QStringList &sListArgs) {
    QStringList sArgs = sListArgs;
    QString sOutput("");
    // Get and check date
    QString sDate;
    if (sArgs.size() >= 1) {
        // Extract date
        QStringList sListDate = sArgs[0].split(".");
        // Wrong date format
        if (3 != sListDate.size()) {
            sDate = "";
        } else {  // Correct number of date elements
            // Wrong date
            if (sListDate[0].toInt() <= 0
                    || sListDate[0].toInt() > 31
                    || sListDate[1].toInt() <= 0
                    || sListDate[1].toInt() > 12
                    || sListDate[2].toInt() <= 0) {
                sDate = "";
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
        if (sDate == "") {
            // Entry with index 0 = first user (no correct date given)
            iCntUser = 0;
        } else {
            iCntUser = 1;  // Entry after date = first user
        }

        // Generate user list
        for (; iCntUser < sArgs.size(); iCntUser++)  {
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

        if (sDate != "") {
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

    return this->insertBox("box workinprogress",
                           QString::fromUtf8("Artikel in Arbeit"),
                           sOutput,
                           QString::fromUtf8("Insbesondere heißt das, dass "
                                             "dieser Artikel noch nicht fertig "
                                             "ist und dass wichtige Teile fehlen "
                                             "oder sogar falsch sein können. "
                                             "Bitte diesen Artikel nicht als "
                                             "Anleitung für Problemlösungen "
                                             "benutzen."));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseWarning(const QStringList &sListArgs) {
    QString sOutput("");
    foreach (QString s, sListArgs) {
        sOutput += s + " ";
    }
    return insertBox("box warning", QString::fromUtf8("Achtung!"), sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CProvisionalTplParser::parseWorkInProgress(const QStringList &sListArgs) {
    QStringList sArgs(sListArgs);
    QString sOutput = QString::fromUtf8("Dieser Artikel wird momentan überarbeitet.");

    // Correct number of elements?
    if (sArgs.size() >= 3) {
        // Replace possible spaces
        for (int i = 0; i < sArgs.size(); i++) {
            sArgs[i].replace(" ", "_");
        }

        sOutput += QString::fromUtf8("\n * Geplante Fertigstellung: %1").arg(sArgs[0]);
        sOutput += QString::fromUtf8("\n * Derzeit gültiger Artikel: [:%1:]").arg(sArgs[1]);

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
                                     ":letzten Änderung] und entscheide, wie du "
                                     "weiter vorgehst.</p>\n").arg(sArgs[1]);
    }

    return insertBox("box workinprogress",
                     QString::fromUtf8("Artikel wird überarbeitet"),
                     sOutput,
                     QString::fromUtf8("Insbesondere heißt das, dass dieser "
                                       "Artikel noch nicht fertig ist und dass "
                                       "wichtige Teile fehlen oder sogar falsch "
                                       "sein können. Bitte diesen Artikel nicht "
                                       "als Anleitung für Problemlösungen "
                                       "benutzen!"));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Insert box
QString CProvisionalTplParser::insertBox(const QString &sClass,
                                         const QString &sHeadline,
                                         const QString &sContents,
                                         const QString &sRemark) {
    QString sReturn("");

    // Generate output
    sReturn = "<div class=\"" + sClass + "\">\n";
    sReturn += "<h3 class=\"" + sClass + "\">" + sHeadline +  "</h3>\n";
    sReturn += "<div class=\"contents\">\n";
    sReturn += "<p>" + sContents + "</p>\n";
    // Remark available
    if (sRemark != "" && sRemark != " ") {
        sReturn += QString::fromUtf8("<hr />\n<p><strong>Anmerkung:</strong> ")
                + sRemark + "</p>\n";
    }
    sReturn += "</div>\n"
               "</div>\n";

    return sReturn;
}
