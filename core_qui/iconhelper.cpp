#include "iconhelper.h"

QScopedPointer<IconHelper> IconHelper::self;
IconHelper *IconHelper::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new IconHelper);
        }
    }

    return self.data();
}

IconHelper::IconHelper(QObject *parent) : QObject(parent)
{
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/image/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }
}

QFont IconHelper::getIconFont()
{
    return this->iconFont;
}

void IconHelper::setIcon(QLabel *lab, const QChar &icon, quint32 size)
{
    iconFont.setPixelSize(size);
    lab->setFont(iconFont);
    lab->setText(icon);
}

void IconHelper::setIcon(QAbstractButton *btn, const QChar &icon, quint32 size)
{
    iconFont.setPixelSize(size);
    btn->setFont(iconFont);
    btn->setText(icon);
}

QPixmap IconHelper::getPixmap(const QColor &color, const QChar &icon, quint32 size,
                              quint32 pixWidth, quint32 pixHeight, int flags)
{
    QPixmap pix(pixWidth, pixHeight);
    pix.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(color);

    iconFont.setPixelSize(size);
    painter.setFont(iconFont);
    painter.drawText(pix.rect(), flags, icon);
    painter.end();

    return pix;
}

QPixmap IconHelper::getPixmap(QToolButton *btn, bool normal)
{
    QPixmap pix;
    int index = btns.indexOf(btn);
    if (index >= 0) {
        if (normal) {
            pix = pixNormal.at(index);
        } else {
            pix = pixDark.at(index);
        }
    }

    return pix;
}

QPixmap IconHelper::getPixmap(QToolButton *btn, int type)
{
    QPixmap pix;
    int index = btns.indexOf(btn);
    if (index >= 0) {
        if (type == 0) {
            pix = pixNormal.at(index);
        } else if (type == 1) {
            pix = pixHover.at(index);
        } else if (type == 2) {
            pix = pixPressed.at(index);
        } else if (type == 3) {
            pix = pixChecked.at(index);
        }
    }

    return pix;
}

void IconHelper::setStyle(QFrame *frame, QList<QToolButton *> btns, QList<QChar> icons,
                          quint32 iconSize, quint32 iconWidth, quint32 iconHeight,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    int btnCount = btns.count();
    int charCount = icons.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    QStringList qss;
    qss.append(QString("QFrame>QToolButton{border-style:none;border-width:0px;"
                       "background-color:%1;color:%2;}").arg(normalBgColor).arg(normalTextColor));
    qss.append(QString("QFrame>QToolButton:hover,QFrame>QToolButton:pressed,QFrame>QToolButton:checked"
                       "{background-color:%1;color:%2;}").arg(darkBgColor).arg(darkTextColor));

    frame->setStyleSheet(qss.join(""));

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    for (int i = 0; i < btnCount; i++) {
        QPixmap pixNormal = getPixmap(normalTextColor, icons.at(i), iconSize, iconWidth, iconHeight);
        QPixmap pixDark = getPixmap(darkTextColor, icons.at(i), iconSize, iconWidth, iconHeight);

        QToolButton *btn = btns.at(i);
        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);

        this->btns.append(btn);
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixDark);
        this->pixHover.append(pixDark);
        this->pixPressed.append(pixDark);
        this->pixChecked.append(pixDark);
    }
}

void IconHelper::setStyle(QWidget *widget, const QString &type, int borderWidth, const QString &borderColor,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    QStringList qss;
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;"
                       "color:%2;background:%3;}").arg(type).arg(normalTextColor).arg(normalBgColor));

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover,"
                       "QWidget[flag=\"%1\"] QAbstractButton:pressed,"
                       "QWidget[flag=\"%1\"] QAbstractButton:checked{"
                       "border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(borderColor).arg(darkTextColor).arg(darkBgColor));

    widget->setStyleSheet(qss.join(""));
}

void IconHelper::removeStyle(QList<QToolButton *> btns)
{
    for (int i = 0; i < btns.count(); i++) {
        for (int j = 0; j < this->btns.count(); j++) {
            if (this->btns.at(j) == btns.at(i)) {
                this->btns.at(j)->removeEventFilter(this);
                this->btns.removeAt(j);
                this->pixNormal.removeAt(j);
                this->pixDark.removeAt(j);
                this->pixHover.removeAt(j);
                this->pixPressed.removeAt(j);
                this->pixChecked.removeAt(j);
                break;
            }
        }
    }
}

void IconHelper::setStyle(QWidget *widget, QList<QToolButton *> btns, QList<QChar> icons,
                          quint32 iconSize, quint32 iconWidth, quint32 iconHeight,
                          const QString &type, int borderWidth, const QString &borderColor,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    int btnCount = btns.count();
    int charCount = icons.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    //如果图标是左侧显示则需要让没有选中的按钮左侧也有加深的边框,颜色为背景颜色
    QStringList qss;
    if (btns.at(0)->toolButtonStyle() == Qt::ToolButtonTextBesideIcon) {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:solid;border-radius:0px;%2border-color:%3;color:%4;background:%5;}")
                   .arg(type).arg(strBorder).arg(normalBgColor).arg(normalTextColor).arg(normalBgColor));
    } else {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
                   .arg(type).arg(normalTextColor).arg(normalBgColor));
    }

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover,"
                       "QWidget[flag=\"%1\"] QAbstractButton:pressed,"
                       "QWidget[flag=\"%1\"] QAbstractButton:checked{"
                       "border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(borderColor).arg(darkTextColor).arg(darkBgColor));

    qss.append(QString("QWidget#%1{background:%2;}").arg(widget->objectName()).arg(normalBgColor));
    qss.append(QString("QWidget>QToolButton{border-width:0px;"
                       "background-color:%1;color:%2;}").arg(normalBgColor).arg(normalTextColor));
    qss.append(QString("QWidget>QToolButton:hover,QWidget>QToolButton:pressed,QWidget>QToolButton:checked{"
                       "background-color:%1;color:%2;}").arg(darkBgColor).arg(darkTextColor));

    widget->setStyleSheet(qss.join(""));

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    for (int i = 0; i < btnCount; i++) {
        QPixmap pixNormal = getPixmap(normalTextColor, icons.at(i), iconSize, iconWidth, iconHeight);
        QPixmap pixDark = getPixmap(darkTextColor, icons.at(i), iconSize, iconWidth, iconHeight);

        QToolButton *btn = btns.at(i);
        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);

        this->btns.append(btn);
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixDark);
        this->pixHover.append(pixDark);
        this->pixPressed.append(pixDark);
        this->pixChecked.append(pixDark);
    }
}

void IconHelper::setStyle(QWidget *widget, QList<QToolButton *> btns, QList<QChar> icons, const IconHelper::StyleColor &styleColor)
{
    int btnCount = btns.count();
    int charCount = icons.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    quint32 iconSize = styleColor.iconSize;
    quint32 iconWidth = styleColor.iconWidth;
    quint32 iconHeight = styleColor.iconHeight;
    quint32 borderWidth = styleColor.borderWidth;
    QString type = styleColor.type;

    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    //如果图标是左侧显示则需要让没有选中的按钮左侧也有加深的边框,颜色为背景颜色
    QStringList qss;
    if (btns.at(0)->toolButtonStyle() == Qt::ToolButtonTextBesideIcon) {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:solid;border-radius:0px;%2border-color:%3;color:%4;background:%5;}")
                   .arg(type).arg(strBorder).arg(styleColor.normalBgColor).arg(styleColor.normalTextColor).arg(styleColor.normalBgColor));
    } else {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
                   .arg(type).arg(styleColor.normalTextColor).arg(styleColor.normalBgColor));
    }

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.hoverTextColor).arg(styleColor.hoverBgColor));
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:pressed{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.pressedTextColor).arg(styleColor.pressedBgColor));
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:checked{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.checkedTextColor).arg(styleColor.checkedBgColor));

    qss.append(QString("QWidget#%1{background:%2;}").arg(widget->objectName()).arg(styleColor.normalBgColor));
    qss.append(QString("QWidget>QToolButton{border-width:0px;background-color:%1;color:%2;}").arg(styleColor.normalBgColor).arg(styleColor.normalTextColor));
    qss.append(QString("QWidget>QToolButton:hover{background-color:%1;color:%2;}").arg(styleColor.hoverBgColor).arg(styleColor.hoverTextColor));
    qss.append(QString("QWidget>QToolButton:pressed{background-color:%1;color:%2;}").arg(styleColor.pressedBgColor).arg(styleColor.pressedTextColor));
    qss.append(QString("QWidget>QToolButton:checked{background-color:%1;color:%2;}").arg(styleColor.checkedBgColor).arg(styleColor.checkedTextColor));

    widget->setStyleSheet(qss.join(""));

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    for (int i = 0; i < btnCount; i++) {
        QPixmap pixNormal = getPixmap(styleColor.normalTextColor, icons.at(i), iconSize, iconWidth, iconHeight);
        QPixmap pixHover = getPixmap(styleColor.hoverTextColor, icons.at(i), iconSize, iconWidth, iconHeight);
        QPixmap pixPressed = getPixmap(styleColor.pressedTextColor, icons.at(i), iconSize, iconWidth, iconHeight);
        QPixmap pixChecked = getPixmap(styleColor.checkedTextColor, icons.at(i), iconSize, iconWidth, iconHeight);

        QToolButton *btn = btns.at(i);
        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);

        this->btns.append(btn);
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixHover);
        this->pixHover.append(pixHover);
        this->pixPressed.append(pixPressed);
        this->pixChecked.append(pixChecked);
    }
}

bool IconHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (watched->inherits("QToolButton")) {
        QToolButton *btn = (QToolButton *)watched;
        int index = btns.indexOf(btn);
        if (index >= 0) {
            if (event->type() == QEvent::Enter) {
                btn->setIcon(QIcon(pixHover.at(index)));
            } else if (event->type() == QEvent::MouseButtonPress) {
                btn->setIcon(QIcon(pixPressed.at(index)));
            } else if (event->type() == QEvent::Leave) {
                if (btn->isChecked()) {
                    btn->setIcon(QIcon(pixChecked.at(index)));
                } else {
                    btn->setIcon(QIcon(pixNormal.at(index)));
                }
            }
        }
    }

    return QObject::eventFilter(watched, event);
}
