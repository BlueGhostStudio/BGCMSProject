#include "ressubwinwidgetbase.h"

#include <QIcon>
#include <QMdiSubWindow>

#include "global.h"

ResSubWinWidgetBase::ResSubWinWidgetBase(QWidget* __parent__)
    : QWidget(__parent__) {
    QObject::connect(
        &bgmrpcClient, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray& args) {
            QVariantMap vArg0Map = args.toVariantList()[0].toMap();
            int vArg0Int = args.toVariantList()[0].toInt();
            if (obj == "CMS") {
                if (sig == "updatedResource" &&
                    vArg0Map["id"] == m_resource["id"]) {
                    m_resource = vArg0Map;
                    setWindow();
                } else if ((sig == "deletedCollection" &&
                            vArg0Int == m_resource["cid"]) ||
                           (sig == "deletedCategory" &&
                            vArg0Int == m_resource["cateid"]) ||
                           (sig == "deletedResource" &&
                            vArg0Int == m_resource["id"]))
                    closeAboutDeleted();
            }
        });
}

int
ResSubWinWidgetBase::resID() const {
    return m_resource["id"].toInt();
}

bool
ResSubWinWidgetBase::modified() {
    return false;
}

void
ResSubWinWidgetBase::openResource(const QVariantMap& resource) {
    m_resource = resource;
    setWindow();
    // emit winIDChanged(winID());
}

void
ResSubWinWidgetBase::closeAboutDeleted() {
    // qobject_cast<QMdiSubWindow*>(parent())->close();
    emit closeWin();
}

void
ResSubWinWidgetBase::setWindow() {
    /*setWindowTitle(m_resource["title"].toString() + (modified() ? "*" : ""));

    qobject_cast<QMdiSubWindow*>(parent())->setWindowIcon(
        resIcon(m_resource["type"].toString()));*/
    emit updateWindowTitle(
        m_resource["title"].toString() + (modified() ? "*" : ""),
        resIcon(m_resource["type"].toString()));
}
