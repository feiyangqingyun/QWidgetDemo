#ifndef WASMHELPER_H
#define WASMHELPER_H

#include <QObject>

class WasmHelper
{    
public:
    //弹出js信息框
    static void showMessage(const QString &text);
    //弹出js输入框
    static QString getInput(const QString &title, const QString &text);

    //获取iframe样式
    static QString getIframeStyle(const QRect &rect);
    //打开iframe窗体
    static void openIframe(const QString &flag, const QString &url, const QRect &rect);
    //重新加载iframe窗体
    static void reloadIframe(const QString &flag, const QString &url);
    //移动iframe窗体
    static void moveIframe(const QString &flag, const QRect &rect);
    //隐藏iframe窗体
    static void hideIframe(const QString &flag);
};

#endif // WASMHELPER_H
