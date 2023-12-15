#ifndef MDKWIDGET_H
#define MDKWIDGET_H

#include "mdkinclude.h"
#include "qpainter.h"

class MdkWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    explicit MdkWidget(QWidget *parent = 0);
    ~MdkWidget();

    void setUrl(const QString& url);
    void play();
    void pause();
    void stop();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    mdk::Player *player;
};

#endif // MDKWIDGET_H
