#ifndef ZHTOPY_H
#define ZHTOPY_H

/**
 * 汉字转拼音类 作者:feiyangqingyun(QQ:517216493) 2019-02-16
 * 1. 汉字转拼音。
 * 2. 汉字转拼音简拼。
 * 3. 汉字转拼音首字母。
 */

#include <QObject>
#include <QStringList>

#ifdef quc
class Q_DECL_EXPORT ZhToPY : public QObject
#else
class ZhToPY : public QObject
#endif

{
    Q_OBJECT
public:
    static ZhToPY *Instance();
    explicit ZhToPY(QObject *parent = 0);

private:
    static QScopedPointer<ZhToPY> self;
    QStringList listPY;
    QStringList listJP;

public:
    //载入拼音文件
    void loadPY(const QString &fileName = "zhtopy.txt");
    //汉字转拼音
    QString zhToPY(const QString &chinese);
    //汉字转字母简拼
    QString zhToJP(const QString &chinese);
    //汉字转首字母
    QString zhToZM(const QString &chinese);
};

#endif // ZHTOPY_H
