#ifndef CMSWIDGET_H
#define CMSWIDGET_H

#include <QMenu>
#include <QStandardItemModel>
#include <QToolBar>

#include "cmsfilterproxymodel.h"
#include "global.h"
#include "ui_cmswidget.h"

class CMSWidget : public QWidget, private Ui::CMSWidget {
    Q_OBJECT

public:
    explicit CMSWidget(QWidget* parent = nullptr);
    void afterJoined(CallGraph* jcg, const QString& to);

signals:
    void titleChanged(const QString& title);
    // void locationChanged(const QVariant& data = QVariant());
    void openResource(const QVariantMap& resource);
    void closeAllSubWin();
    void locationChanged(int type, int cID, int cateID, int resID);
    // void resourceUpdated(const QVariantMap& resource);

public slots:
    void toRoot();

    void openResourceContentByID(int rID);

    // void toByPath(const QString& path);
    // void toCollectionByID(int cid);
    // void toCategoryByID(int cateID);

    // void openResourceContent(const QVariantMap& resource);

private slots:
    // void join();

    void back();
    void backToLocation(int type);
    void browerDoubleClicked(const QModelIndex& index);
    void popupCMSMenu(const QPoint& pos);
    void popupEntryMenu(const QPoint& pos);

    void newEntry();
    void editProperty();
    void deleteEntry();
    void copyImgURL();

    void moveToCategory(QAction* act);

private:
    void cg_loadCollectionList(CallGraph* pcg = nullptr,
                               const QString& to = QString());
    void cg_loadCategoryList(int cID, CallGraph* pcg = nullptr,
                             const QString& to = QString());
    void cg_loadResourceList(int cID, int cateID, CallGraph* pcg = nullptr,
                             const QString& to = QString());

    struct t_location;
    t_location variant2location(const QVariant& var);
    // t_location gotoLocation(const QVariant& var = QVariant());
    // void backLocation();
    void updateLocation(const QVariant& var);
    void updateWindowTitle();
    void updateUIState();

    void updateLocationState();

    QStandardItem* browerItemCollection(const QVariantMap& var,
                                        QStandardItem* item = nullptr);
    QStandardItem* browerItemCategory(const QVariantMap& var,
                                      QStandardItem* item = nullptr);
    QStandardItem* browerItemResource(const QVariantMap& var,
                                      QStandardItem* item = nullptr);

    QStandardItem* findItem(int id);

private:
    struct t_location {
        int type = -1;
        int collectionID;
        int categoryID;
        int resourceID;
        QString title;
    };
    QStandardItemModel m_browerModel;
    CMSFilterProxyModel m_filterModel;

    QStack<t_location> m_locationStack;
    t_location m_currentLocation;

    QToolBar ui_cmsToolBar;
    QToolBar ui_filterToolBar;

    // QMenu ui_cmsMenu;
    QMenu ui_filterMenu;
    // QMenu ui_entryMenu;
    QMenu ui_moveToMenu;
};

#endif  // CMSWIDGET_H
