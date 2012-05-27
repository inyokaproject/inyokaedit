/**
 * \file CTextEditor.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (qt-info@nokia.com)
 *
 * This file is part of the examples of the Qt Toolkit.
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 *
 * \section DESCRIPTION
 * Extend QTextEdit (editor widget) to popup code completition for Inyoka
 * syntax elements.
 */

#include "CTextEditor.h"
#include "CInyokaEdit.h"

#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>

CTextEditor::CTextEditor( Ui::CInyokaEdit *pGUI, bool bCompleter, QWidget *pParent ) :
    QTextEdit( pParent ),
    m_bCodeCompState( bCompleter )
{
    qDebug() << "Start" << Q_FUNC_INFO;

    m_sListCompleter << "Inhaltsverzeichnis(1)]]" << "Vorlage(Getestet, Ubuntuversion)]]" << "Vorlage(Baustelle, Datum, \"Bearbeiter\")]]"
                     << "Vorlage(Fortgeschritten)]]" << "Vorlage(Pakete, \"foo bar\")]]" << trUtf8("Vorlage(Ausbaufähig, \"Begründung\")]]")
                     << trUtf8("Vorlage(Fehlerhaft, \"Begründung\")]]") << trUtf8("Vorlage(Verlassen, \"Begründung\")]]") << "Vorlage(Archiviert, \"Text\")]]"
                     << "Vorlage(Kopie, Seite, Autor)]]" << trUtf8("Vorlage(Überarbeitung, Datum, Seite, Autor)]]") << "Vorlage(Fremd, Paket, \"Kommentar\")]]"
                     << "Vorlage(Fremd, Quelle, \"Kommentar\")]]" << "Vorlage(Fremd, Software, \"Kommentar\")]]" << trUtf8("Vorlage(Award, \"Preis\", Link, Preiskategorie, \"Preisträger\")]]")
                     << "Vorlage(PPA, PPA-Besitzer, PPA-Name)]]" << "Vorlage(Fremdquelle-auth, URL zum PGP-Key)]]" << trUtf8("Vorlage(Fremdquelle-auth, key PGP-Schlüsselnummer)]]")
                     << "Vorlage(Fremdquelle, URL, Ubuntuversion(en), Komponente(n) )]]" << "Vorlage(Fremdpaket, Projekthoster, Projektname, Ubuntuversion(en))]]"
                     << trUtf8("Vorlage(Fremdpaket, \"Anbieter\", URL zu einer Downloadübersicht, Ubuntuversion(en))]]") << "Vorlage(Fremdpaket, \"Anbieter\", dl, URL zu EINEM Download, Ubuntuversion(en))]]"
                     << "Vorlage(Tasten, TASTE)]]" << trUtf8("Bild(name.png, Größe, Ausrichtung)]]") << "Anker(Name)]]" << "[[Vorlage(Bildunterschrift, BILDLINK, BILDBREITE, \"Beschreibung\", left|right)]]"
                     << trUtf8("Vorlage(Bildersammlung, BILDHÖHE\nBild1.jpg, \"Beschreibung 1\"\nBild2.png, \"Beschreibung 2\"\n)]]");

    m_pCompleter = new QCompleter( m_sListCompleter,
                                   this );

    this->setCompleter(m_pCompleter);
    this->setAcceptRichText(false); // Paste plain text only

    // Cut
    pGUI->cutAct->setShortcuts(QKeySequence::Cut);
    connect( pGUI->cutAct, SIGNAL(triggered()),
             this, SLOT(cut()) );
    connect( this, SIGNAL(copyAvailable(bool)),
             pGUI->cutAct, SLOT(setEnabled(bool)) );
    // Copy
    pGUI->copyAct->setShortcuts(QKeySequence::Copy);
    connect( pGUI->copyAct, SIGNAL(triggered()),
             this, SLOT(copy()) );
    connect( this, SIGNAL(copyAvailable(bool)),
             pGUI->copyAct, SLOT(setEnabled(bool)) );
    // Paste
    pGUI->pasteAct->setShortcuts(QKeySequence::Paste);
    connect( pGUI->pasteAct, SIGNAL(triggered()),
             this, SLOT(paste()) );
    // Undo
    pGUI->undoAct->setShortcuts(QKeySequence::Undo);
    connect( pGUI->undoAct, SIGNAL(triggered()),
             this, SLOT(undo()) );
    connect( this, SIGNAL(undoAvailable(bool)),
             pGUI->undoAct, SLOT(setEnabled(bool)) );
    // Redo
    pGUI->redoAct->setShortcuts(QKeySequence::Redo);
    connect( pGUI->redoAct, SIGNAL(triggered()),
             this, SLOT(redo()) );
    connect( this, SIGNAL(redoAvailable(bool)),
             pGUI->redoAct, SLOT(setEnabled(bool)) );


    // Text changed
    connect( this->document(), SIGNAL(contentsChanged()),
             pParent, SLOT( documentWasModified()) );

    qDebug() << "End" << Q_FUNC_INFO;
}

CTextEditor::~CTextEditor()
{
}

void CTextEditor::setCompleter( QCompleter *completer )
{
    if ( m_pCompleter ) {
        QObject::disconnect( m_pCompleter, 0, this, 0 );
    }

    m_pCompleter = completer;

    if ( !m_pCompleter ) {
        return;
    }

    m_pCompleter->setWidget( this );
    m_pCompleter->setCompletionMode( QCompleter::PopupCompletion );
    m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pCompleter->setWrapAround(false);
    QObject::connect( m_pCompleter, SIGNAL(activated(QString)),
                      this, SLOT(insertCompletion(QString)) );
}

QCompleter *CTextEditor::completer() const
{
    return m_pCompleter;
}

void CTextEditor::insertCompletion( const QString& completion )
{
    if ( m_pCompleter->widget() != this ) {
        return;
    }
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_pCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CTextEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CTextEditor::focusInEvent( QFocusEvent *e )
{
    if ( m_pCompleter ) {
        m_pCompleter->setWidget(this);
    }
    QTextEdit::focusInEvent(e);
}

void CTextEditor::keyPressEvent( QKeyEvent *e )
{

    if ( m_pCompleter && m_pCompleter->popup()->isVisible() ) {
        // The following keys are forwarded by the completer to the widget
        switch ( e->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                e->ignore();
                return; // let the completer do default behavior
            default:
                break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if ( !m_pCompleter || !isShortcut ) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if ( !m_pCompleter || (ctrlOrShift && e->text().isEmpty()) )
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    /*
    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }
    */

    if ( false == m_bCodeCompState ) {
        m_pCompleter->popup()->hide();
        return;
    }
    else if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        m_pCompleter->popup()->hide();
        return;
    }


    if (completionPrefix != m_pCompleter->completionPrefix()) {
        m_pCompleter->setCompletionPrefix(completionPrefix);
        m_pCompleter->popup()->setCurrentIndex(m_pCompleter->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_pCompleter->popup()->sizeHintForColumn(0)
                + m_pCompleter->popup()->verticalScrollBar()->sizeHint().width());
    m_pCompleter->complete(cr); // popup it up!

}
