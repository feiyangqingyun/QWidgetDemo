#include "wasmhelper.h"
#include "qrect.h"
#include "emscripten.h"
#include "emscripten/html5.h"

//弹出js信息框
EM_JS(void, showMessageJs, (const char *text), {
    alert(UTF8ToString(text));
})

//弹出js输入框
EM_JS(const char *, getInputJs, (const char *title, const char *defaultText), {
    var result = prompt(UTF8ToString(title), UTF8ToString(defaultText));
    if (!result)
    {
        result = "";
    }
    return stringToNewUTF8(result);
})

//打开iframe窗体
EM_JS(void, openIframeJs, (const char *flag, const char *url, const char *style), {
    //如果存在则只移动位置
    var id = UTF8ToString(flag);
    var iframe = document.getElementById(id);
    if (iframe)
    {
        iframe.style = UTF8ToString(style);
        return;
    }

    iframe = document.createElement('iframe');
    iframe.id = id;
    iframe.src = UTF8ToString(url);
    iframe.style = UTF8ToString(style);
    iframe.scrolling = 'no';
    document.body.appendChild(iframe);
})

//加载iframe窗体
EM_JS(void, reloadIframeJs, (const char *flag, const char *url), {
    var iframe = document.getElementById(UTF8ToString(flag));
    if (iframe)
    {
        iframe.src = UTF8ToString(url);
    }
})

//移动iframe窗体
EM_JS(void, moveIframeJs, (const char *flag, const char *style), {
    var iframe = document.getElementById(UTF8ToString(flag));
    if (iframe)
    {
        iframe.style = UTF8ToString(style);
    }
})

//隐藏iframe窗体
EM_JS(void, hideIframeJs, (const char *flag), {
    var iframe = document.getElementById(UTF8ToString(flag));
    if (iframe)
    {
        iframe.style = "display:none";
    }
})

void WasmHelper::showMessage(const QString &text)
{
    showMessageJs(text.toUtf8().constData());
}

QString WasmHelper::getInput(const QString &title, const QString &text)
{
    return getInputJs(title.toUtf8().constData(), text.toUtf8().constData());
}

QString WasmHelper::getIframeStyle(const QRect &rect)
{
    QString style = QString("border:0px;position:absolute;margin:0px;padding:0px;z-index:10000;opacity:1.0;");
    style += QString("left:%1px;").arg(rect.x());
    style += QString("top:%1px;").arg(rect.y());
    style += QString("width:%1px;").arg(rect.width());
    style += QString("height:%1px;").arg(rect.height());
    return style;
}

void WasmHelper::openIframe(const QString &flag, const QString &url, const QRect &rect)
{
    QString style = getIframeStyle(rect);
    openIframeJs(flag.toUtf8().constData(), url.toUtf8().constData(), style.toUtf8().constData());
}

void WasmHelper::reloadIframe(const QString &flag, const QString &url)
{
    reloadIframeJs(flag.toUtf8().constData(), url.toUtf8().constData());
}

void WasmHelper::moveIframe(const QString &flag, const QRect &rect)
{
    QString style = getIframeStyle(rect);
    moveIframeJs(flag.toUtf8().constData(), style.toUtf8().constData());
}

void WasmHelper::hideIframe(const QString &flag)
{
    hideIframeJs(flag.toUtf8().constData());
}
