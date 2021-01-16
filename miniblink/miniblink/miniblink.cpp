#include "miniblink.h"
#include "qapplication.h"
#include "qdebug.h"

void onLoadingFinish(wkeWebView, void *param, const wkeString, wkeLoadingResult result, const wkeString)
{
    //qDebug() << "onLoadingFinish" << result;
    //在注册函数的地方就已经传入了类指针
    miniblink *widget = (miniblink *)param;
    //0 = WKE_LOADING_SUCCEEDED, 1 = WKE_LOADING_FAILED, 2 = WKE_LOADING_CANCELED
    widget->loadFinish(result == 0);
}

jsValue WKE_CALL_TYPE objName_receiveData(jsExecState es, void *param)
{
    if (0 == jsArgCount(es)) {
        return jsUndefined();
    }

    //挨个取出参数,设定的通用方法,只有两个参数
    jsValue arg0 = jsArg(es, 0);
    jsValue arg1 = jsArg(es, 1);
    if (!jsIsString(arg0)) {
        return jsUndefined();
    }

    //在注册函数的地方就已经传入了类指针
    miniblink *widget = (miniblink *)param;
    QString type = QString::fromStdString(jsToString(es, arg0));
    QVariant data = QString::fromStdString(jsToString(es, arg1));
    widget->receiveData(type, data);
    return jsUndefined();
}

miniblink::miniblink(QWidget *parent) : QWidget(parent)
{
    //第一步先初始化动态库
    init();
    //第二步初始化浏览器控件
    //创建一个浏览器控件,放入句柄
    webView = wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)this->winId(), 0, 0, this->width(), this->height());
    //关联完成信号
    wkeOnLoadingFinish(webView, onLoadingFinish, this);
    //设置浏览器控件可见
    wkeShowWindow(webView, TRUE);
    //注册通用的接收数据的方法,一定要放在这里在网页加载前执行
    wkeJsBindFunction("objName_receiveData", objName_receiveData, this, 2);
}

void miniblink::init()
{
    //全局只需要初始化一次
    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        //不同的构建套件位数加载不同的动态库
#ifdef Q_OS_WIN64
        QString file = qApp->applicationDirPath() + "/miniblink_64.dll";
#else
        QString file = qApp->applicationDirPath() + "/miniblink.dll";
#endif
        const wchar_t *path = reinterpret_cast<const wchar_t *>(file.utf16());
        wkeSetWkeDllPath(path);
        bool ok = wkeInitialize();
        qDebug() << QString("init miniblink %1").arg(ok ? "ok" : "error");
    }
}

void miniblink::release()
{
    wkeFinalize();
}

void miniblink::resizeEvent(QResizeEvent *)
{
    wkeResize(webView, this->width(), this->height());
}

void miniblink::loadFinish(bool ok)
{
    emit loadFinished(ok);
}

void miniblink::receiveData(const QString &type, const QVariant &data)
{
    emit receiveDataFromJs(type, data);
}

void miniblink::load(const QString &url, bool file)
{
    const char *temp = url.toLocal8Bit().data();
    if (file) {
        wkeLoadFile(webView, temp);
    } else {
        wkeLoadURL(webView, temp);
    }
}

void miniblink::setHtml(const QString &html, const QString &baseUrl)
{
    wkeLoadHtmlWithBaseUrl(webView, html.toLocal8Bit().data(), baseUrl.toLocal8Bit().data());
}

void miniblink::runJs(const QString &js)
{
    wkeRunJS(webView, js.toLocal8Bit().data());
}
