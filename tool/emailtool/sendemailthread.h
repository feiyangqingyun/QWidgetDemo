#ifndef SENDEMAILTHREAD_H
#define SENDEMAILTHREAD_H

#include <QThread>
#include <QMutex>
#include <QStringList>

class SendEmailThread : public QThread
{
	Q_OBJECT
public:
    static SendEmailThread *Instance();
    explicit SendEmailThread(QObject *parent = 0);
    ~SendEmailThread();

protected:
	void run();

private:
    static QScopedPointer<SendEmailThread> self;
    QMutex mutex;
	volatile bool stopped;

	QString emialTitle;         //邮件标题
	QString sendEmailAddr;      //发件人邮箱
	QString sendEmailPwd;       //发件人密码
	QString receiveEmailAddr;   //收件人邮箱,可多个中间;隔开
	QStringList contents;       //正文内容
	QStringList fileNames;      //附件

signals:
    void receiveEmailResult(const QString &result);

public slots:    
    void stop();
    void setEmailTitle(const QString &emailTitle);
    void setSendEmailAddr(const QString &sendEmailAddr);
    void setSendEmailPwd(const QString &sendEmailPwd);
    void setReceiveEmailAddr(const QString &receiveEmailAddr);
    void append(const QString &content, const QString &fileName);

};

#endif // SENDEMAILTHREAD_H
