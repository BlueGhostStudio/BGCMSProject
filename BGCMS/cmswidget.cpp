#include "cmswidget.h"

#include <QClipboard>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtDebug>

#include "categorydialog.h"
#include "cmssubwin.h"
#include "collectiondialog.h"
#include "global.h"
#include "resourcedialog.h"

CMSWidget::CMSWidget(QWidget* parent) : QWidget(parent) {
    setupUi(this);

    ui_toolbarLayout->insertWidget(0, &ui_cmsToolBar);
    ui_cmsToolBar.addAction(action_Back);
    ui_cmsToolBar.addSeparator();
    ui_cmsToolBar.addAction(actionNew_Entry);
    ui_cmsToolBar.addAction(actionProperty);
    ui_cmsToolBar.addAction(actionDelete_Entry);

    ui_filterMenu.addAction(actionFilterArticle);
    ui_filterMenu.addAction(actionFilterComponent);
    ui_filterMenu.addAction(actionFilterStyle);
    ui_filterMenu.addAction(actionFilterMedia);
    ui_filterMenu.setTitle(tr("Filter"));

    ui_moveToMenu.setTitle(tr("Move to..."));
    QObject::connect(&ui_moveToMenu, &QMenu::triggered, this,
                     &CMSWidget::moveToCategory);

    ui_toolbarLayout->insertWidget(1, &ui_filterToolBar);
    ui_filterToolBar.addAction(actionFilterArticle);
    ui_filterToolBar.addAction(actionFilterComponent);
    ui_filterToolBar.addAction(actionFilterStyle);
    ui_filterToolBar.addAction(actionFilterMedia);

    setTabOrder(&ui_cmsToolBar, &ui_filterToolBar);
    setTabOrder(&ui_filterToolBar, ui_lvBrower);

    m_filterModel.setSourceModel(&m_browerModel);
    m_filterModel.sort(0);
    ui_lvBrower->setModel(&m_filterModel);

    /*QObject::connect(&bgmrpcClient, &NS_BGMRPCClient::BGMRPCClient::connected,
                     this, &CMSWidget::join);*/
    QObject::connect(&bgmrpcClient,
                     &NS_BGMRPCClient::BGMRPCClient::isConnectedChanged, this,
                     [=](bool status) {
                         setEnabled(status);
                         ui_cmsToolBar.setEnabled(status);
                         if (!status) {
                             emit closeAllSubWin();
                             /*m_locationStack.clear();
                             m_currentLocation = t_location();*/
                             toRoot();
                             m_browerModel.clear();
                             /*emit locationChanged();*/
                         }
                     });
    emit bgmrpcClient.isConnectedChanged(false);

    QObject::connect(actionFilterArticle, &QAction::toggled, &m_filterModel,
                     &CMSFilterProxyModel::setFilterArticle);
    QObject::connect(actionFilterComponent, &QAction::toggled, &m_filterModel,
                     &CMSFilterProxyModel::setFilterComponent);
    QObject::connect(actionFilterStyle, &QAction::toggled, &m_filterModel,
                     &CMSFilterProxyModel::setFilterStyle);
    QObject::connect(actionFilterMedia, &QAction::toggled, &m_filterModel,
                     &CMSFilterProxyModel::setFilterMedia);

    QObject::connect(
        &bgmrpcClient, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal, this,
        [=](const QString& object, const QString& sig, const QJsonArray& args) {
            QVariantList vArgs = args.toVariantList();
            QVariantMap vArg0Map = vArgs[0].toMap();
            int vArg0Int = vArgs[0].toInt();
            if (object != "CMS") return;
            if (sig == "newCollection" && m_currentLocation.type == -1)
                m_browerModel.appendRow(browerItemCollection(vArg0Map));
            else if (sig == "updatedCollection") {
                QVariantMap collData = vArg0Map;
                int cID = collData["id"].toInt();
                if (m_currentLocation.type == -1) {
                    QStandardItem* item = findItem(cID);
                    if (item) browerItemCollection(collData, item);
                } else if (m_currentLocation.collectionID == cID)
                    updateLocation(collData);
                // emit locationChanged(collData);
            } else if (sig == "deletedCollection") {
                int cID = vArg0Int;
                if (m_currentLocation.type == -1) {
                    QStandardItem* item = findItem(cID);
                    if (item) m_browerModel.removeRow(item->row());
                } else if (m_currentLocation.collectionID == cID) {
                    backToLocation(-1);
                }
            } else if (sig == "addedCategory" && m_currentLocation.type == 0 &&
                       vArg0Map["cid"].toInt() ==
                           m_currentLocation.collectionID) {
                m_browerModel.appendRow(browerItemCategory(vArg0Map));
            } else if (sig == "updatedCategory") {
                QVariantMap cateData = vArg0Map;
                int cateID = cateData["id"].toInt();
                if (m_currentLocation.type == 0) {
                    if (cateData["cid"].toInt() ==
                        m_currentLocation.collectionID) {
                        QStandardItem* item = findItem(cateID);
                        if (item) browerItemCategory(cateData, item);
                    }
                } else if (m_currentLocation.categoryID == cateID)
                    updateLocation(cateData);
                // emit locationChanged(cateData);
            } else if (sig == "deletedCategory") {
                int cateID = vArg0Int;
                if (m_currentLocation.type == 0) {
                    QStandardItem* item = findItem(cateID);
                    if (item) m_browerModel.removeRow(item->row());
                } else if (m_currentLocation.categoryID == cateID) {
                    /*do {
                        m_currentLocation = m_locationStack.pop();
                    } while (m_currentLocation.type != 0);
                    cg_loadCategoryList(m_currentLocation.collectionID);*/
                    backToLocation(0);
                }
            } else if (sig == "addedResource" && m_currentLocation.type == 1 &&
                       vArg0Map["cateid"].toInt() ==
                           m_currentLocation.categoryID) {
                m_browerModel.appendRow(browerItemResource(vArg0Map));
            } else if (sig == "updatedResource" &&
                       m_currentLocation.type == 1) {
                QStandardItem* item = findItem(vArg0Map["id"].toInt());
                if (item) {
                    if (vArg0Map["cateid"].toInt() ==
                        m_currentLocation.categoryID)
                        browerItemResource(vArg0Map, item);
                    else
                        m_browerModel.removeRow(item->row());
                } else if (vArg0Map["cateid"].toInt() ==
                           m_currentLocation.categoryID)
                    m_browerModel.appendRow(browerItemResource(vArg0Map));
            } /*else if (sig == "updatedResource" &&
                    m_currentLocation.type == 1 &&
                    vArg0Map["cateid"].toInt() ==
                        m_currentLocation.categoryID) {
             QVariantMap resData = vArg0Map;
             QStandardItem* item = findItem(resData["id"].toInt());
             emit resourceUpdated(resData);
             if (item) browerItemResource(resData, item);
         }*/ else if (sig == "deletedResource" && m_currentLocation.type == 1) {
                QStandardItem* item = findItem(vArg0Int);
                if (item) m_browerModel.removeRow(item->row());
            }
        });
}

void
CMSWidget::afterJoined(CallGraph* jcg, const QString& to) {
    CallGraph::start("loadCollectionList")
        ->nodes("loadCollectionList",
                [=](CallGraph* cg, const QVariant&) {
                    cg_loadCollectionList(cg, "end");
                })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant&) {
                    toRoot();
                    jcg->to(to);
                    cg->toFinal();
                })
        ->exec();
}

void
CMSWidget::toRoot() {
    m_currentLocation = t_location();
    m_locationStack.clear();
    updateUIState();
    updateWindowTitle();
    updateLocationState();
}

void
CMSWidget::openResourceContentByID(int rID) {
    if (bgmrpcClient.isConnected()) {
        CallGraph::start("getResource")
            ->nodes("getResource",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient
                            .callMethod("CMS", "resource", { data.toInt() })
                            ->then([=](const QVariant& ret) {
                                QVariant ret0 = ret.toList()[0];
                                if (ret0.isValid())
                                    emit openResource(ret0.toMap());

                                cg->toFinal();
                            });
                    })
            ->exec(rID);
    }
}

/*void
CMSWidget::openResourceContent(const QVariantMap& resource) {
    emit openResource(resource);
}*/

/*void
CMSWidget::join() {
    CallGraph::start("join")
        ->nodes("join",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient.callMethod("CMS", "join", {})
                        ->then([=](const QVariant&) {
                            cg->to("loadCollectionList");
                        });
                })
        ->nodes("loadCollectionList",
                [=](CallGraph* cg, const QVariant&) {
                    cg_loadCollectionList(cg, "end");
                })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant&) {
                    toRoot();
                    cg->toFinal();
                })
        ->exec();
}*/

void
CMSWidget::back() {
    if (!m_locationStack.isEmpty()) {
        m_currentLocation = m_locationStack.pop();
        updateLocationState();
        updateWindowTitle();
        updateUIState();

        if (m_currentLocation.type < 0)
            cg_loadCollectionList();
        else if (m_currentLocation.type == 0)
            cg_loadCategoryList(m_currentLocation.collectionID);
    }
    /*if (!m_locationStack.isEmpty()) {
        m_currentLocation = m_locationStack.pop();
        emit locationChanged();

    if (m_currentLocation.type == -1)
        cg_loadCollectionList();
    else if (m_currentLocation.type == 0) {
        cg_loadCategoryList(m_currentLocation.collectionID);
    }
}*/
}

void
CMSWidget::backToLocation(int type) {
    while (m_currentLocation.type != type)
        m_currentLocation = m_locationStack.pop();

    //    action_Back->setEnabled(m_locationStack.count() > 0);
    //    emit locationChanged();
    updateLocationState();
    updateWindowTitle();
    updateUIState();

    if (m_currentLocation.type < 0)
        cg_loadCollectionList();
    else if (m_currentLocation.type == 0) {
        cg_loadCategoryList(m_currentLocation.collectionID);
    }
}

void
CMSWidget::browerDoubleClicked(const QModelIndex& index) {
    QStandardItem* item =
        m_browerModel.itemFromIndex(m_filterModel.mapToSource(index));
    t_location location = variant2location(item->data());
    if (location.type != 2) {
        m_locationStack.push(m_currentLocation);
        m_currentLocation = location;
        updateLocationState();
        updateWindowTitle();
        updateUIState();

        switch (location.type) {
        case 0:
            cg_loadCategoryList(location.collectionID);
            break;
        case 1:
            cg_loadResourceList(location.collectionID, location.categoryID);
            break;
        }
    } else if (location.type == 2) {
        // openResourceContent(item->data().toMap());

        QVariantMap resMap = item->data().toMap();
        emit locationChanged(2, resMap["cid"].toInt(), resMap["cateid"].toInt(),
                             resMap["id"].toInt());
        emit openResource(resMap);
    }
}

void
CMSWidget::popupCMSMenu(const QPoint& pos) {
    // ui_cmsMenu.exec(pos);
    if (m_currentLocation.type == 1)
        QMenu::exec({ actionNew_Entry, ui_filterMenu.menuAction() }, pos);
    else
        QMenu::exec({ actionNew_Entry }, pos);
}

void
CMSWidget::popupEntryMenu(const QPoint& pos) {
    // ui_entryMenu.exec(pos);
    QVariantMap data = m_browerModel
                           .itemFromIndex(m_filterModel.mapToSource(
                               ui_lvBrower->currentIndex()))
                           ->data()
                           .toMap();
    if (m_currentLocation.type == 1) {
        if (data["type"] != "M")
            QMenu::exec({ actionProperty, actionDelete_Entry,
                          ui_moveToMenu.menuAction() },
                        pos);
        else
            QMenu::exec({ actionProperty, actionDelete_Entry,
                          actionCopy_Image_URL, ui_moveToMenu.menuAction() },
                        pos);
    } else
        QMenu::exec({ actionProperty, actionDelete_Entry }, pos);
}

void
CMSWidget::newEntry() {
    switch (m_currentLocation.type) {
    case -1: {
        CollectionDialog* newCollDlg = new CollectionDialog(this);
        CallGraph::start("showDlg")
            ->nodes("showDlg",
                    [=](CallGraph* cg, const QVariant&) {
                        QObject::connect(
                            newCollDlg, &CollectionDialog::finished, this,
                            [=](int result) {
                                if (result)
                                    cg->to("newCollection",
                                           newCollDlg->collection());
                                else
                                    cg->toFinal();

                                delete newCollDlg;
                            });
                        newCollDlg->open();
                    })
            ->nodes("newCollection",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient
                            .callMethod("CMS", "newCollection", { data })
                            ->then([=](const QVariant&) { cg->toFinal(); });
                    })
            ->exec();
    } break;
    case 0: {
        CategoryDialog* newCateDlg = new CategoryDialog(this);
        CallGraph::start("showDlg")
            ->nodes("showDlg",
                    [=](CallGraph* cg, const QVariant&) {
                        QObject::connect(newCateDlg, &CategoryDialog::finished,
                                         this, [=](int result) {
                                             if (result)
                                                 cg->to("addCategory",
                                                        newCateDlg->category());
                                             else
                                                 cg->toFinal();

                                             delete newCateDlg;
                                         });
                        newCateDlg->open();
                        /*bool ok;
                        QString title = QInputDialog::getText(
                            this, tr("Category"), tr("Title"),
                            QLineEdit::Normal, "no title", &ok);
                        if (!ok)
                            cg->toFinal();
                        else
                            cg->to("addCategory", title);*/
                    })
            ->nodes("addCategory",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient
                            .callMethod(
                                "CMS", "addCategory",
                                { m_currentLocation.collectionID, data })
                            ->then([=](const QVariant&) { cg->toFinal(); });
                    })
            ->exec();
    } break;
    case 1:
        ResourceDialog* newResDlg = new ResourceDialog(this);
        QProgressDialog* postImgProgress = new QProgressDialog(this);
        postImgProgress->setWindowTitle(tr("Post Image"));
        postImgProgress->setLabelText(tr("Post..."));
        postImgProgress->setWindowModality(Qt::ApplicationModal);
        postImgProgress->setCancelButton(nullptr);
        postImgProgress->close();

        CallGraph::start("showDlg")
            ->nodes(
                "showDlg",
                [=](CallGraph* cg, const QVariant&) {
                    QObject::connect(
                        newResDlg, &ResourceDialog::finished, this,
                        [=](int result) {
                            if (result) {
                                QVariantMap resMap =
                                    newResDlg
                                        ->resource(m_currentLocation.categoryID)
                                        .toMap();
                                if (resMap["type"] != "M")
                                    cg->to("addResource", resMap);
                                else
                                    cg->to("addImg", resMap);
                            } else
                                cg->to("end");
                            delete newResDlg;
                        });
                    newResDlg->open();
                })
            ->nodes(
                "addResource",
                [=](CallGraph* cg, const QVariant& data) {
                    bgmrpcClient
                        .callMethod("CMS", "addResource",
                                    { m_currentLocation.collectionID, data })
                        ->then([=](const QVariant&) { cg->to("end"); });
                },
                "addImg",
                [=](CallGraph* cg, const QVariant& data) {
                    cg->to("requestPostImg", data);
                })
            ->nodes("requestPostImg",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient.callMethod("CMSRes", "requestPostImg", {})
                            ->then([=](const QVariant& ret) {
                                cg->to("readImgData",
                                       QVariantList({ ret.toList()[0], data }));
                            });
                    })
            ->nodes(
                "readImgData",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantList list = data.toList();
                    QVariantMap res = list[1].toMap();
                    QString imgFileName = res["content"].toString();
                    res["content"] = list[0];
                    QFile imgFile(imgFileName);
                    if (imgFile.exists() && imgFile.open(QIODevice::ReadOnly)) {
                        QByteArray imgData = imgFile.readAll();
                        imgFile.close();
                        postImgProgress->setValue(0);
                        postImgProgress->setMaximum(imgData.size());
                        postImgProgress->open();
                        cg->to("postImgData", QVariantList({ res, imgData }));
                    } else
                        cg->to("close", res["content"]);
                })
            ->nodes("postImgData",
                    [=](CallGraph* cg, const QVariant& data) {
                        QVariantList list = data.toList();
                        QVariantMap res = list[0].toMap();
                        QString id = res["content"].toString();
                        QByteArray imgData = list[1].toByteArray();
                        QByteArray partData = imgData.left(4096);
                        if (partData.length() > 0) {
                            bgmrpcClient
                                .callMethod(
                                    "CMSRes", "writeImgData",
                                    QVariantList(
                                        { id, QString(partData.toBase64()) }))
                                ->then([=](const QVariant&) {
                                    if (imgData.length() > 4096) {
                                        postImgProgress->setValue(
                                            postImgProgress->value() + 4096);
                                        cg->to("postImgData",
                                               QVariantList(
                                                   { res, imgData.mid(4096) }));
                                    } else {
                                        postImgProgress->setValue(
                                            postImgProgress->value() +
                                            imgData.size());
                                        cg->to("writeImgData", res);
                                    }
                                });
                        } else {
                            cg->to("close", id);
                        }
                    })
            ->nodes("writeImgData",
                    [=](CallGraph* cg, const QVariant& data) {
                        QVariantList list = data.toList();
                        QVariantMap res = data.toMap();
                        bgmrpcClient
                            .callMethod("CMSRes", "writeImgDataEnd",
                                        { res["content"] })
                            ->then([=](const QVariant&) {
                                cg->to("addImgEntry", res);
                            });
                    })
            ->nodes("addImgEntry",
                    [=](CallGraph* cg, const QVariant& data) {
                        QVariantMap res = data.toMap();
                        if (res["alias"].toString().length() == 0)
                            res["alias"] = res["content"];
                        if (res["title"].toString().length() == 0)
                            res["title"] = res["content"];
                        bgmrpcClient
                            .callMethod("CMS", "addResource",
                                        { m_currentLocation.collectionID, res })
                            ->then([=](const QVariant&) { cg->to("end"); });
                    })
            ->nodes("close",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient.callMethod("CMSRes", "close", { data })
                            ->then([=](const QVariant&) { cg->to("end"); });
                    })
            ->nodes("end",
                    [=](CallGraph* cg, const QVariant&) {
                        delete postImgProgress;
                        cg->toFinal();
                    })
            ->exec();
        break;
    }
}

void
CMSWidget::editProperty() {
    if (!ui_lvBrower->currentIndex().isValid()) return;

    switch (m_currentLocation.type) {
    case -1: {
        CollectionDialog* collDlg = new CollectionDialog(this);
        collDlg->setCollection(m_browerModel
                                   .itemFromIndex(m_filterModel.mapToSource(
                                       ui_lvBrower->currentIndex()))
                                   ->data()
                                   .toMap());
        CallGraph::start("showDlg")
            ->nodes("showDlg",
                    [=](CallGraph* cg, const QVariant&) {
                        QObject::connect(collDlg, &CollectionDialog::finished,
                                         this, [=](int result) {
                                             if (result)
                                                 cg->to("updateCollection",
                                                        collDlg->collection());
                                             else
                                                 cg->toFinal();

                                             delete collDlg;
                                         });
                        collDlg->open();
                    })
            ->nodes(
                "updateCollection",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap coll = data.toMap();
                    bgmrpcClient
                        .callMethod("CMS", "updateCollection",
                                    { coll["id"], coll })
                        ->then([=](const QVariant& /*ret*/) { cg->toFinal(); });
                })
            ->exec();
    } break;
    case 0: {
        QVariantMap cateData = m_browerModel
                                   .itemFromIndex(m_filterModel.mapToSource(
                                       ui_lvBrower->currentIndex()))
                                   ->data()
                                   .toMap();
        if (cateData["id"] == -1) break;

        CategoryDialog* cateDlg = new CategoryDialog(this);
        cateDlg->setCategory(cateData);
        CallGraph::start("showDlg")
            ->nodes("showDlg",
                    [=](CallGraph* cg, const QVariant&) {
                        QObject::connect(cateDlg, &CategoryDialog::finished,
                                         this, [=](int result) {
                                             if (result)
                                                 cg->to("updateCategory",
                                                        cateDlg->category());
                                             else
                                                 cg->toFinal();

                                             delete cateDlg;
                                         });
                        cateDlg->open();
                        /*QString title = cateData["title"].toString();
                        bool ok;
                        title = QInputDialog::getText(
                            this, tr("Category"), tr("Title"),
                            QLineEdit::Normal, title, &ok);
                        if (ok)
                            cg->to("updateCategory", title);
                        else
                            cg->toFinal();*/
                    })
            ->nodes("updateCategory",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient
                            .callMethod("CMS", "updateCategory",
                                        { cateData["id"], data })
                            ->then([=](const QVariant&) { cg->toFinal(); });
                    })
            ->exec();
    } break;
    case 1: {
        QVariantMap resData = m_browerModel
                                  .itemFromIndex(m_filterModel.mapToSource(
                                      ui_lvBrower->currentIndex()))
                                  ->data()
                                  .toMap();
        ResourceDialog* resDlg = new ResourceDialog(this);
        resDlg->setResource(resData);

        CallGraph::start("showDlg")
            ->nodes("showDlg",
                    [=](CallGraph* cg, const QVariant&) {
                        QObject::connect(resDlg, &ResourceDialog::finished,
                                         this, [=](int result) {
                                             if (result)
                                                 cg->to("updateResource",
                                                        resDlg->resource());
                                             else
                                                 cg->toFinal();

                                             delete resDlg;
                                         });
                        resDlg->open();
                    })
            ->nodes(
                "updateResource",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap res = data.toMap();
                    bgmrpcClient
                        .callMethod("CMS", "updateResource", { res["id"], res })
                        ->then([=](const QVariant& /*ret*/) { cg->toFinal(); });
                })
            ->exec();
    } break;
    }
}

void
CMSWidget::deleteEntry() {
    if (!ui_lvBrower->currentIndex().isValid()) return;

    switch (m_currentLocation.type) {
    case -1:
        if (QMessageBox::question(
                this, tr("Delete Collection"),
                tr("Do you want to delete the selected Collection?")) ==
            QMessageBox::Yes) {
            CallGraph::start("deleteCollection")
                ->nodes("deleteCollection",
                        [=](CallGraph* cg, const QVariant& data) {
                            bgmrpcClient
                                .callMethod("CMS", "deleteCollection",
                                            { data.toMap()["id"] })
                                ->then([=](const QVariant& /*ret*/) {
                                    cg->toFinal();
                                });
                        })
                ->exec(m_browerModel
                           .itemFromIndex(m_filterModel.mapToSource(
                               ui_lvBrower->currentIndex()))
                           ->data());
        }

        break;
    case 0:
        if (QMessageBox::question(
                this, tr("Delete Category"),
                tr("Do you want to delete the selected Category?")) ==
            QMessageBox::Yes) {
            CallGraph::start("deleteCategory")
                ->nodes("deleteCategory",
                        [=](CallGraph* cg, const QVariant& data) {
                            bgmrpcClient
                                .callMethod("CMS", "deleteCategory",
                                            { data.toMap()["id"] })
                                ->then([=](const QVariant& /*ret*/) {
                                    cg->toFinal();
                                });
                        })
                ->exec(m_browerModel
                           .itemFromIndex(m_filterModel.mapToSource(
                               ui_lvBrower->currentIndex()))
                           ->data());
        }
        break;
    case 1:
        if (QMessageBox::question(
                this, tr("delete Resource"),
                tr("Do you want to delete the selected Resource?")) ==
            QMessageBox::Yes) {
            CallGraph::start("checkType")
                ->nodes("checkType",
                        [=](CallGraph* cg, const QVariant& data) {
                            QVariantMap map = data.toMap();
                            if (map["type"] != "M")
                                cg->to("deleteResource", data);
                            else
                                cg->to("getImgFile", data);
                        })
                ->nodes("deleteResource",
                        [=](CallGraph* cg, const QVariant& data) {
                            bgmrpcClient
                                .callMethod("CMS", "deleteResource",
                                            { data.toMap()["id"] })
                                ->then([=](const QVariant& /*ret*/) {
                                    cg->toFinal();
                                });
                        })
                ->nodes(
                    "getImgFile",
                    [=](CallGraph* cg, const QVariant& data) {
                        bgmrpcClient
                            .callMethod("CMS", "resourceContent",
                                        { data.toMap()["id"] })
                            ->then([=](const QVariant& ret) {
                                cg->to("removeImg",
                                       QVariantList(
                                           { ret.toList()[0].toMap()["content"],
                                             data }));
                            });
                    })
                ->nodes("removeImg",
                        [=](CallGraph* cg, const QVariant& data) {
                            QVariantList list = data.toList();
                            bgmrpcClient
                                .callMethod("CMSRes", "removeImg", { list[0] })
                                ->then([=](const QVariant&) {
                                    cg->to("deleteResource", list[1]);
                                });
                        })
                ->exec(m_browerModel
                           .itemFromIndex(m_filterModel.mapToSource(
                               ui_lvBrower->currentIndex()))
                           ->data());
        }
        break;
    }
}

void
CMSWidget::copyImgURL() {
    if (!ui_lvBrower->currentIndex().isValid()) return;

    CallGraph::start("getImgFileName")
        ->nodes("getImgFileName",
                [=](CallGraph* cg, const QVariant& data) {
                    bgmrpcClient
                        .callMethod("CMS", "resourceContent",
                                    { data.toMap()["id"] })
                        ->then([=](const QVariant& ret) {
                            /*QApplication::clipboard()->setText(
                                ret.toList()[0].toString());*/
                            cg->to("getImgUrl",
                                   ret.toList()[0].toMap()["content"]);
                        });
                })
        ->nodes("getImgUrl",
                [=](CallGraph* cg, const QVariant& data) {
                    bgmrpcClient
                        .callMethod("CMSRes", "imgURL", { data.toString() })
                        ->then([=](const QVariant& ret) {
                            QApplication::clipboard()->setText(
                                ret.toList()[0].toString());
                            cg->toFinal();
                        });
                })
        ->exec(m_browerModel
                   .itemFromIndex(
                       m_filterModel.mapToSource(ui_lvBrower->currentIndex()))
                   ->data());
}

void
CMSWidget::moveToCategory(QAction* act) {
    CallGraph::start("moveToCategory")
        ->nodes("moveToCategory",
                [=](CallGraph* cg, const QVariant& data) {
                    bgmrpcClient
                        .callMethod(
                            "CMS", "updateResource",
                            { data.toMap()["id"],
                              QVariantMap({ { "cateid", act->data() } }) })
                        ->then([=](const QVariant&) { cg->toFinal(); });
                })
        ->exec(m_browerModel
                   .itemFromIndex(
                       m_filterModel.mapToSource(ui_lvBrower->currentIndex()))
                   ->data());
}

void
CMSWidget::cg_loadCollectionList(CallGraph* pcg, const QString& to) {
    CallGraph::start("initial")
        ->nodes("initial",
                [=](CallGraph* cg, const QVariant&) {
                    m_browerModel.clear();
                    cg->to("getCollections");
                })
        ->nodes("getCollections",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient.callMethod("CMS", "collections", {})
                        ->then([=](const QVariant& ret) {
                            cg->to("loadList", ret);
                        });
                })
        ->nodes("loadList",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantList list = data.toList();
                    foreach (const QVariant& itemData, list) {
                        m_browerModel.appendRow(
                            browerItemCollection(itemData.toMap()));
                    }
                    ui_lvBrower->reset();
                    if (pcg && !to.isEmpty()) pcg->to(to);
                    cg->toFinal();
                })
        ->exec();
}

void
CMSWidget::cg_loadCategoryList(int cID, CallGraph* pcg, const QString& to) {
    CallGraph::start("initial")
        ->nodes("initial",
                [=](CallGraph* cg, const QVariant&) {
                    m_browerModel.clear();
                    cg->to("getCategories");
                })
        ->nodes("getCategories",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient.callMethod("CMS", "categories", { cID })
                        ->then([=](const QVariant& ret) {
                            cg->to("loadList", ret);
                        });
                })
        ->nodes("loadList",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantList list = data.toList();
                    foreach (const QVariant& itemData, list) {
                        m_browerModel.appendRow(
                            browerItemCategory(itemData.toMap()));
                    }
                    ui_lvBrower->reset();
                    if (pcg && !to.isEmpty()) pcg->to(to);
                    cg->toFinal();
                })
        ->exec();
}

void
CMSWidget::cg_loadResourceList(int cID, int cateID, CallGraph* pcg,
                               const QString& to) {
    CallGraph::start("initial")
        ->nodes("initial",
                [=](CallGraph* cg, const QVariant&) {
                    m_browerModel.clear();
                    cg->to("getResources");
                })
        ->nodes(
            "getResources",
            [=](CallGraph* cg, const QVariant&) {
                bgmrpcClient.callMethod("CMS", "resources", { cID, cateID })
                    ->then(
                        [=](const QVariant& ret) { cg->to("loadList", ret); });
            })
        ->nodes("loadList",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantList list = data.toList();
                    foreach (const QVariant& itemData, list) {
                        m_browerModel.appendRow(
                            browerItemResource(itemData.toMap()));
                    }
                    ui_lvBrower->reset();
                    if (pcg && !to.isEmpty()) pcg->to(to);
                    cg->toFinal();
                })
        ->exec();
}

CMSWidget::t_location
CMSWidget::variant2location(const QVariant& var) {
    t_location location;

    if (var.isValid()) {
        QVariantMap mVar = var.toMap();
        if (mVar["cid"].isValid()) {
            location.collectionID = mVar["cid"].toInt();

            if (mVar["cateid"].isValid()) {
                location.type = 2;
                location.resourceID = mVar["id"].toInt();
                location.categoryID = mVar["cateid"].toInt();
            } else {
                location.type = 1;
                location.categoryID = mVar["id"].toInt();
            }
        } else {
            location.type = 0;
            location.collectionID = mVar["id"].toInt();
        }

        location.title = mVar["title"].toString();
    } else
        location.type = -2;

    return location;
}

void
CMSWidget::updateLocation(const QVariant& var) {
    t_location changedLocation = variant2location(var);
    if ((changedLocation.type == 0 && m_locationStack.count() == 1 &&
         changedLocation.collectionID == m_currentLocation.collectionID) ||
        (changedLocation.type == 1 && m_locationStack.count() == 2 &&
         changedLocation.categoryID == m_currentLocation.categoryID)) {
        m_currentLocation.title = changedLocation.title;
    } else {
        if (changedLocation.type == 0 && m_locationStack.count() > 1 &&
            changedLocation.collectionID == m_locationStack[1].collectionID)
            m_locationStack[1].title = changedLocation.title;
        else if (changedLocation.type == 1 && m_locationStack.count() > 2 &&
                 changedLocation.categoryID == m_locationStack[2].categoryID)
            m_locationStack[2].title = changedLocation.title;
    }

    updateLocationState();
    updateWindowTitle();
    updateUIState();
}

/*CMSWidget::t_location
CMSWidget::gotoLocation(const QVariant &var)
{
    t_location location = variant2location(var);
    if (location.type != 2) {
        m_locationStack.push(m_currentLocation);
        m_currentLocation = location;
        updateWindowTitle();

        if (m_currentLocation.type == 1) {
            ui_filterToolBar.setVisible(true);
            ui_filterMenu.setVisible(true);
            m_filterModel.sort(0);
        } else {
            ui_filterToolBar.setVisible(false);
            ui_filterMenu.setVisible(false);
        }
    }

    return location;
}*/

void
CMSWidget::updateWindowTitle() {
    QString lStr;
    for (int i = 0; i < m_locationStack.count(); i++) {
        t_location theLocation = m_locationStack[i];
        lStr += theLocation.title + '/';
    }

    lStr += (m_currentLocation.type == -1) ? "/" : m_currentLocation.title;
    emit titleChanged(lStr);
}

void
CMSWidget::updateUIState() {
    ui_moveToMenu.clear();
    if (m_currentLocation.type == 1) {
        ui_filterToolBar.setVisible(true);

        CallGraph::start("getCategories")
            ->nodes("getCategories",
                    [=](CallGraph* cg, const QVariant&) {
                        bgmrpcClient
                            .callMethod("CMS", "categories",
                                        { m_currentLocation.collectionID })
                            ->then([=](const QVariant& ret) {
                                QVariantList list = ret.toList();
                                foreach (const QVariant& itemData, list) {
                                    QVariantMap itemDataMap = itemData.toMap();
                                    QAction* action = ui_moveToMenu.addAction(
                                        itemDataMap["title"].toString());
                                    action->setData(itemDataMap["id"]);
                                }
                                cg->toFinal();
                            });
                    })
            ->exec();
    } else {
        ui_filterToolBar.setVisible(false);
    }

    action_Back->setEnabled(m_currentLocation.type >= 0);
}

void
CMSWidget::updateLocationState() {
    //    QVariantMap changedSignal({ { "type", m_currentLocation.type } });
    if (bgmrpcClient.isConnected()) {
        switch (m_currentLocation.type) {
        case -1:
            emit locationChanged(-1, -1, -1, -1);
            break;
        case 0:
            emit locationChanged(0, m_currentLocation.collectionID, -1, -1);
            //        changedSignal["collectionID"] =
            //        m_currentLocation.collectionID;
            break;
        case 1:
            emit locationChanged(1, m_currentLocation.collectionID,
                                 m_currentLocation.categoryID, -1);
            //        changedSignal["categoryID"] =
            //        m_currentLocation.categoryID;
            break;
        }
    }
    //    emit updateLocationState(changedSignal);
}

QStandardItem*
CMSWidget::browerItemCollection(const QVariantMap& var, QStandardItem* item) {
    if (!item) {
        item = new QStandardItem;
        item->setIcon(QIcon(":/imgs/collection"));
    }

    int seq = var["seq"].toInt();
    QString seqStr = seq >= 0 ? QString("#%1 ").arg(seq) : "";

    QString title = seqStr + var["title"].toString();
    QString alias = var["alias"].toString();

    QString secondLine(alias.length() > 0 ? alias : "");
    secondLine += var["private"].toBool() ? "🔒" : "";
    if (secondLine.length() > 0) title += "\n" + secondLine;

    item->setText(title);
    item->setData(var);
    item->setData(0, Qt::UserRole + 2);
    item->setData(var["id"], Qt::UserRole + 3);

    return item;
}

QStandardItem*
CMSWidget::browerItemCategory(const QVariantMap& var, QStandardItem* item) {
    if (!item) {
        item = new QStandardItem;
        item->setIcon(QIcon(":/imgs/category.png"));
    }
    int seq = var["seq"].toInt();
    QString seqStr = seq >= 0 ? QString("#%1 ").arg(seq) : "";
    /*item->setText(seqStr + var["title"].toString() +
                  (var["hide"].toBool() ? "\n[HIDE]" : ""));*/

    QString title = seqStr + var["title"].toString();
    QString alias = var["alias"].toString();

    QString secondLine(alias.length() > 0 ? alias : "");
    secondLine += var["hide"].toBool() ? "👁⃫" : "";
    secondLine += var["private"].toBool() ? "🔒" : "";

    if (secondLine.length() > 0) title += "\n" + secondLine;

    item->setText(title);
    item->setData(var);
    item->setData(1, Qt::UserRole + 2);
    item->setData(var["id"], Qt::UserRole + 3);

    return item;
}

QStandardItem*
CMSWidget::browerItemResource(const QVariantMap& var, QStandardItem* item) {
    if (!item) item = new QStandardItem;

    QString resType = var["type"].toString();
    /*if (resType == "A")
        item->setIcon(QIcon(":/imgs/article.png"));
    else if (resType == "C")
        item->setIcon(QIcon(":/imgs/component.png"));
    else if (resType == "S")
        item->setIcon(QIcon(":/imgs/style.png"));
    else if (resType == "M")
        item->setIcon(QIcon(":/imgs/media.png"));*/
    item->setIcon(resIcon(resType));

    int seq = var["seq"].toInt();
    QString title =
        (seq >= 0 ? QString("#%1 ").arg(seq) : "") + var["title"].toString();

    QString alias = var["alias"].toString();
    QString secondLine(alias.length() > 0 ? alias : "");
    secondLine += var["hide"].toBool() ? "👁⃫" : "";
    secondLine += var["private"].toBool() ? "🔒" : "";

    if (secondLine.length() > 0) title += "\n" + secondLine;

    item->setText(title);
    item->setData(var);
    item->setData(2, Qt::UserRole + 2);
    item->setData(var["id"], Qt::UserRole + 3);

    return item;
}

QStandardItem*
CMSWidget::findItem(int id) {
    QStandardItem* item = nullptr;
    bool found = false;
    for (int i = 0; i < m_browerModel.rowCount(); i++) {
        item = m_browerModel.item(i);
        if (item->data(Qt::UserRole + 3).toInt() == id) {
            found = true;
            break;
        }
    }

    return found ? item : nullptr;
}
