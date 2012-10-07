/**
 * \file finddialog.h
 *
 * \section DESCRIPTION
 * Copyright (C) 2009  Lorenzo Bettini <http://www.lorenzobettini.it>
 * See COPYING file that comes with this distribution
 */

#ifndef INYOKAEDIT_FINDDIALOG_H_
#define INYOKAEDIT_FINDDIALOG_H_

#include <QDialog>

#include "./findreplacedialog.h"

/**
 * \class FindDialog
 * \brief A find dialog.
 */
class FindDialog : public FindReplaceDialog {
    Q_OBJECT
public:
    explicit FindDialog( QWidget *parent = 0 );
    ~FindDialog();

    /**
      * Writes the state of the form to the passed settings.
      * @param settings
      * @param prefix the prefix to insert in the settings
      */
    virtual void writeSettings(QSettings &settings, const QString &prefix = "FindDialog");

    /**
      * Reads the state of the form from the passed settings.
      * @param settings
      * @param prefix the prefix to look for in the settings
      */
    virtual void readSettings(QSettings &settings, const QString &prefix = "FindDialog");
};

#endif // INYOKAEDIT_FINDDIALOG_H_
