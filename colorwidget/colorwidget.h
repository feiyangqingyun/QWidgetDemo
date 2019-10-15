#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QWidget>

class QGridLayout;
class QVBoxLayout;
class QLabel;
class QLineEdit;

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT ColorWidget : public QWidget
#else
class ColorWidget : public QWidget
#endif

{
    Q_OBJECT
public:
    static ColorWidget *Instance();
    explicit ColorWidget(QWidget *parent = 0);
    ~ColorWidget();    

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    static ColorWidget *instance;
    QClipboard *cp;
    bool pressed;
    QTimer *timer;

    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labColor;
    QLabel *label;
    QLabel *labWeb;
    QLineEdit *txtWeb;
    QLabel *labRgb;
    QLineEdit *txtRgb;
    QLabel *labPoint;
    QLineEdit *txtPoint;

private Q_SLOTS:
    void showColorValue();
};

#endif // COLORWIDGET_H
