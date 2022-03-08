/*****************************************************************************
 * newnodedlg.h
 *
 * Created: 2022/2/10 2022 by blueghost
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
#ifndef NEWNODEDLG_H
#define NEWNODEDLG_H

#include "../../BGCMS/cmsdialog.h"
#include "ui_newnodedlg.h"

class newNodeDlg : public CMSDialog, private Ui::newNodeDlg {
    Q_OBJECT

public:
    explicit newNodeDlg(CallGraph* pcg, const QString& contentType,
                        const QString& accept,
                        const QString& reject = QString(),
                        QWidget* parent = nullptr);

protected:
    QVariant acceptData() override;

private:
    const QString m_contentType;
};

#endif  // NEWNODEDLG_H
