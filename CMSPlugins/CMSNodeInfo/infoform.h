/*****************************************************************************
 * infoform.h
 *
 * Created: 2022/3/7 2022 by blueghost
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
#ifndef INFOFORM_H
#define INFOFORM_H

#include "../../BGCMS/cmsapi.h"
#include "ui_infoform.h"

class InfoForm : public QWidget, private Ui::InfoForm {
    Q_OBJECT

public:
    explicit InfoForm(QWidget* parent = nullptr);

    void loadInfo(const QVariantMap& node);

signals:
    void updateSummary(const QString& summary);
    void updateExtData(const QString& extData);

private:
    QString m_origSummary;
    QString m_origExtData;
};

#endif  // INFOFORM_H
