#include "global.h"

#include <QIcon>

QIcon
resIcon(const QString &type) {
    if (type == "A")
        return QIcon(":/imgs/article.png");
    else if (type == "C")
        return QIcon(":/imgs/component.png");
    else if (type == "S")
        return QIcon(":/imgs/style.png");
    else if (type == "M")
        return QIcon(":/imgs/media.png");
    else
        return QIcon();
}
