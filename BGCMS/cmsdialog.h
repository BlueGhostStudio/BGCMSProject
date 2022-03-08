/*****************************************************************************
 * cmsdialog.h
 *
 * Created: 2022/1/27 2022 by blueghost
 *
 * Copyright 2022 blueghost. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef CMSDIALOG_H
#define CMSDIALOG_H

#include <callgraph.h>

#include <QDialog>
#include <QObject>

class CMSDialog : public QDialog {
    Q_OBJECT
public:
    CMSDialog(CallGraph* pcg, const QString& accept,
              const QString& reject = QString(), QWidget* parent = nullptr);

protected:
    virtual QVariant acceptData() = 0;
};

#endif  // CMSDIALOG_H
