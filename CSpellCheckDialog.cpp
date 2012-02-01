/**
 * @file CSpellCheckDialog.cpp
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
 * Show spell checker dialog with spelling suggestions.
 *
 * @section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#include "CSpellCheckDialog.h"
#include "ui_CSpellCheckDialog.h"

#include "CSpellChecker.h"

CSpellCheckDialog::CSpellCheckDialog(CSpellChecker *spellChecker, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSpellCheckDialog)
{
    ui->setupUi(this);
    _spellChecker = spellChecker;

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->listWidget, SIGNAL(currentTextChanged(QString)), ui->ledtReplaceWith, SLOT(setText(QString)));

    connect(ui->btnAddToDict, SIGNAL(clicked()), this, SLOT(addToDict()));
    connect(ui->btnReplaceOnce, SIGNAL(clicked()), this, SLOT(replaceOnce()));
    connect(ui->btnReplaceAll, SIGNAL(clicked()), this, SLOT(replaceAll()));
    connect(ui->btnIgnoreOnce, SIGNAL(clicked()), this, SLOT(ignoreOnce()));
    connect(ui->btnIgnoreAll, SIGNAL(clicked()), this, SLOT(ignoreAll()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


CSpellCheckDialog::~CSpellCheckDialog()
{
    delete ui;
}


CSpellCheckDialog::SpellCheckAction CSpellCheckDialog::checkWord(const QString &word)
{
    _unkownWord = word;
    ui->lblUnknownWord->setText(QString("<b>%1</b>").arg(_unkownWord));

    ui->ledtReplaceWith->clear();

    QStringList suggestions = _spellChecker->suggest(word);
    ui->listWidget->clear();
    ui->listWidget->addItems(suggestions);

    if(suggestions.count() > 0)
        ui->listWidget->setCurrentRow(0, QItemSelectionModel::Select);

    _returnCode = AbortCheck;
    QDialog::exec();
    return _returnCode;
}


QString CSpellCheckDialog::replacement() const
{
    return ui->ledtReplaceWith->text();
}


void CSpellCheckDialog::ignoreOnce()
{
    _returnCode = IgnoreOnce;
    accept();
}


void CSpellCheckDialog::ignoreAll()
{
    _spellChecker->ignoreWord(_unkownWord);
    _returnCode = IgnoreAll;
    accept();
}


void CSpellCheckDialog::replaceOnce()
{
    _returnCode = ReplaceOnce;
    accept();
}


void CSpellCheckDialog::replaceAll()
{
    _returnCode = ReplaceAll;
    accept();
}


void CSpellCheckDialog::addToDict()
{
    _spellChecker->addToUserWordlist(_unkownWord);
    _returnCode = AddToDict;
    accept();
}
