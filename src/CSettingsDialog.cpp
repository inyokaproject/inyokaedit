/**
 * \file CSettingsDialog.cpp
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
 * Provide a graphical interface to the application's settings.
 */


#include "CSettingsDialog.h"
#include "ui_CSettingsDialog.h"

#include "./CSettings.h"

CSettingsDialog::CSettingsDialog(CSettings *pSettings, QWidget *parent)
    : QDialog(parent),
      m_pSettings(pSettings)
{
    m_pUi = new Ui::CSettingsDialog();
    m_pUi->setupUi(this);
    
    ///////////////////
    // Load Settings //
    ///////////////////
    
    // General
    m_pUi->codeCompletionCheck->setChecked(m_pSettings->getCodeCompletion());
    m_pUi->previewInEditorCheck->setChecked(m_pSettings->getPreviewInEditor());
    m_pUi->previewAlongsideCheck->setChecked(m_pSettings->getPreviewAlongside());
    m_pUi->inyokaUrlEdit->setText(m_pSettings->getInyokaUrl());
    m_pUi->articleImageDownloadCheck->setChecked(m_pSettings->getAutomaticImageDownload());
    m_pUi->spellCheckerLangEdit->setText(m_pSettings->getSpellCheckerLanguage());
    m_pUi->linkCheckingCheck->setChecked(m_pSettings->getCheckLinks());
    m_pUi->autosaveEdit->setValue(m_pSettings->getAutoSave());
    m_pUi->reloadPreviewKeyEdit->setValue(m_pSettings->getReloadPreviewKey());
    m_pUi->timedPreviewsEdit->setValue(m_pSettings->getTimedPreview());
    m_pUi->scrollbarSyncCheck->setChecked(m_pSettings->getSyncScrollbars());
    
    // Font
    m_pUi->fontFamilyEdit->setText(m_pSettings->m_sFontFamily);
    m_pUi->fontSizeEdit->setValue(m_pSettings->m_nFontsize);
    
    // Recent files
    m_pUi->numberRecentFilesEdit->setValue(m_pSettings->getMaxNumOfRecentFiles());
    
    m_pUi->numberRecentFilesEdit->setMaximum(m_pSettings->getMaxNumOfRecentFiles());
}

CSettingsDialog::~CSettingsDialog(){
    if (m_pUi){
        delete m_pUi;
        m_pUi = NULL;
    }
}

void CSettingsDialog::accept(){
    QString tmpReloadPreviewKey;
    tmpReloadPreviewKey.setNum(m_pUi->reloadPreviewKeyEdit->value());
    tmpReloadPreviewKey.append("0x");
    
    // General
    m_pSettings->m_bCodeCompletion = m_pUi->codeCompletionCheck->isChecked();
    m_pSettings->m_bPreviewInEditor = m_pUi->previewInEditorCheck->isChecked();
    m_pSettings->m_bPreviewAlongside = m_pUi->previewAlongsideCheck->isChecked();
    m_pSettings->m_sInyokaUrl = m_pUi->inyokaUrlEdit->text();
    m_pSettings->m_bAutomaticImageDownload = m_pUi->articleImageDownloadCheck->isChecked();
    m_pSettings->m_sSpellCheckerLanguage = m_pUi->spellCheckerLangEdit->text();
    m_pSettings->m_bCheckLinks = m_pUi->linkCheckingCheck->isChecked();
    m_pSettings->m_nAutosave = m_pUi->autosaveEdit->value();
    m_pSettings->m_sReloadPreviewKey = tmpReloadPreviewKey;
    m_pSettings->m_nTimedPreview = m_pUi->timedPreviewsEdit->value();
    m_pSettings->m_bSyncScrollbars = m_pUi->scrollbarSyncCheck->isChecked();
    
    // Font
    m_pSettings->m_sFontFamily = m_pUi->fontFamilyEdit->text();
    m_pSettings->m_nFontsize = m_pUi->fontSizeEdit->value();
    
    // Recent files
    m_pSettings->m_nMaxLastOpenedFiles = m_pUi->numberRecentFilesEdit->value();
    
    QDialog::accept();
}
