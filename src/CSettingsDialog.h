/**
 * \file CSettingsDialog.h
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
 * Class definition for providing a graphical interface to the application's settings.
 */


#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QDialog>

namespace Ui{
    class CSettingsDialog;
}

class CSettings;

/**
 * \class CSettingsDialog
 * \brief Graphcal interface to settings
 */
class CSettingsDialog : public QDialog {
  public:
    CSettingsDialog(CSettings *pSettings, QWidget *parent = 0);
    virtual ~CSettingsDialog();
    
  public slots:
    void accept();
    
  private:
    Ui::CSettingsDialog *m_pUi;
    
    CSettings *m_pSettings;
};

#endif // CSETTINGSDIALOG_H
