/*****************************************************************************
 * imageviewer.h
 *
 * Created: 2022/2/27 2022 by blueghost
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
#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QProgressBar>

#include "cmsmediaapi.h"
#include "ui_imageviewer.h"

class ImageViewer : public QMainWindow, private Ui::ImageViewer {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget* parent = nullptr);

    void loadImage(CMSMediaApi* mediaApi, const QVariantMap& node);

private:
    QProgressBar* m_loadProgressBar;
};

#endif  // IMAGEVIEWER_H
