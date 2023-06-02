/*****************************************************************************
 * todoeditform.cpp
 *
 * Created: 2022/4/15 2022 by blueghost
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
#include "todoeditform.h"

TodoEditForm::TodoEditForm(const QModelIndex& index, QWidget* parent)
    : m_index(index), QWidget(parent) {
    setupUi(this);

    PRI->setId(ui_tbPRI0, 0);
    PRI->setId(ui_tbPRI1, 1);
    PRI->setId(ui_tbPRI2, 2);

    ui_tbSave->setIcon(QIcon::fromTheme("window-close"));

    m_stateMenu = new QMenu();
    m_stateMenu->addAction(tr("Normal"), this, [=]() {
        emit updateTodo(m_index, "state", 0);
        ui_tbState->setChecked(false);
    });
    m_stateMenu->addAction(tr("Completed"), this, [=]() {
        emit updateTodo(m_index, "state", 1);
        ui_tbState->setChecked(true);
    });
    m_stateMenu->addAction(tr("Obsolete"), this, [=]() {
        emit updateTodo(m_index, "state", 2);
        ui_tbState->setChecked(true);
    });
    m_stateMenu->addAction(tr("Expired"), this, [=]() {
        emit updateTodo(m_index, "state", 3);
        ui_tbState->setChecked(true);
    });

    ui_tbState->setMenu(m_stateMenu);

    QObject::connect(
        ui_teContent->document(), &QTextDocument::modificationChanged, this,
        [=](bool m) {
            if (m)
                ui_tbSave->setIcon(QIcon::fromTheme("document-save"));
            else
                ui_tbSave->setIcon(QIcon::fromTheme("window-close"));
        });
    QObject::connect(ui_tbState, &QToolButton::clicked, this,
                     [=](bool checked) {
                         emit updateTodo(m_index, "state", checked ? 1 : 0);
                     });
    /*QObject::connect(ui_tbState, &QToolButton::toggled, this, [=]() {
        if (m_loaded) {
            emit updateTodo(m_index, "state", ui_tbState->isChecked() ? 1 : 0);
        }
    });*/
    QObject::connect(ui_leLabel, &QLineEdit::editingFinished, this, [=]() {
        if (m_loaded && ui_leLabel->isModified()) {
            emit updateTodo(m_index, "label", ui_leLabel->text());
        }
    });
    QObject::connect(ui_tbSave, &QToolButton::clicked, this, [=]() {
        if (m_loaded) {
            if (ui_teContent->document()->isModified())
                emit updateTodo(m_index, "content",
                                ui_teContent->toPlainText());

            emit closeEditor();
        }
    });
    QObject::connect(PRI, &QButtonGroup::idToggled, this,
                     [=](int id, bool toggle) {
                         if (m_loaded && toggle) {
                             emit updateTodo(m_index, "PRI", id);
                         }
                     });
    QObject::connect(ui_tbDelete, &QToolButton::clicked, this,
                     [=]() { emit removeTodo(m_index); });
}

void
TodoEditForm::load(const QVariantMap& todo) {
    m_loaded = false;

    qDebug() << "loading....";

    m_todoData = todo;
    ui_tbState->setChecked(m_todoData["state"].toInt() > 0);
    ui_teContent->setPlainText(m_todoData["content"].toString());

    switch (m_todoData["PRI"].toInt()) {
    case 0:
        ui_tbPRI0->setChecked(true);
        break;
    case 1:
        ui_tbPRI1->setChecked(true);
        break;
    default:
        ui_tbPRI2->setChecked(true);
        break;
    }
    ui_leLabel->setText(m_todoData["label"].toString());
    m_loaded = true;

    m_changedData = QPair<QString, QVariant>();
}

bool
TodoEditForm::loaded() const {
    return m_loaded;
}

QVariant
TodoEditForm::todoData() {
    return m_todoData;
}

QPair<QString, QVariant>
TodoEditForm::changedData() {
    QPair<QString, QVariant> data;
    data.swap(m_changedData);

    return data;
}
