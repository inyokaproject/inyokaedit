/**
 * \file findform.h
 *
 * \section DESCRIPTION
 * Copyright (C) 2009  Lorenzo Bettini <http://www.lorenzobettini.it>
 * See COPYING file that comes with this distribution
 */

#ifndef FINDFORM_H
#define FINDFORM_H

#include <QWidget>
#include <QTextCursor>

#include "findreplaceform.h"

/**
 * \class FindForm
 * \brief The form for the find dialog.
 */
class FindForm : public FindReplaceForm {
    Q_OBJECT
public:
    FindForm(QWidget *parent = 0);
    ~FindForm();

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

protected:
    void changeEvent(QEvent *e);

};

#endif // FINDFORM_H
