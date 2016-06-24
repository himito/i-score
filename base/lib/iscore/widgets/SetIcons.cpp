#include "SetIcons.hpp"

void setIcons(QAction *action, const QString &iconOn, const QString &iconOff) {
    QIcon icon;
    icon.addPixmap(iconOn, QIcon::Mode::Selected);
    icon.addPixmap(iconOn, QIcon::Mode::Active);
    icon.addPixmap(iconOn, QIcon::Mode::Normal, QIcon::State::On);

    icon.addPixmap(iconOff, QIcon::Mode::Normal);
    action->setIcon(icon);
}

QIcon makeIcons(const QString &iconOn, const QString &iconOff) {
    return genIconFromPixmaps(iconOn, iconOff);
}

QIcon genIconFromPixmaps(const QString &iconOn, const QString &iconOff) {
    QIcon icon;
    icon.addPixmap(iconOn, QIcon::Mode::Selected);
    icon.addPixmap(iconOn, QIcon::Mode::Active);
    icon.addPixmap(iconOn, QIcon::Mode::Normal, QIcon::State::On);
    icon.addPixmap(iconOn, QIcon::Mode::Selected, QIcon::State::On);
    icon.addPixmap(iconOn, QIcon::Mode::Active, QIcon::State::On);

    icon.addPixmap(iconOff, QIcon::Mode::Normal);
    return icon;
}
