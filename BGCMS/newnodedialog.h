/*****************************************************************************
 * newnodedialog.h
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
#ifndef NEWNODEDIALOG_H
#define NEWNODEDIALOG_H

#include "cmsdialog.h"
#include "ui_newnodedialog.h"

class NewNodeDialog : public CMSDialog, private Ui::NewNodeDialog {
    Q_OBJECT

public:
    explicit NewNodeDialog(CallGraph* pcg, const QString& accept,
                           const QString& reject = QString(),
                           QWidget* parent = nullptr);

protected:
    QVariant acceptData() override;
};

#endif  // NEWNODEDIALOG_H
