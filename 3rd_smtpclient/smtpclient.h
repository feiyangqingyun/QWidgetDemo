#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QtGui>
#include <QtNetwork>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif
#include "mimemessage.h"

class SmtpClient : public QObject
{
	Q_OBJECT
public:
	enum AuthMethod {
		AuthPlain,
		AuthLogin
	};

	enum SmtpError {
		ConnectionTimeoutError,
		ResponseTimeoutError,
		AuthenticationFailedError,
		ServerError,    // 4xx smtp error
		ClientError     // 5xx smtp error
	};

	enum ConnectionType {
		TcpConnection,
		SslConnection,
		TlsConnection       // STARTTLS
	};

	SmtpClient(const QString &host = "locahost", int port = 25, ConnectionType ct = TcpConnection);

	~SmtpClient();

	const QString &getHost() const;
	void setHost(QString &host);

	int getPort() const;
	void setPort(int port);

	const QString &getName() const;
	void setName(const QString &name);

	ConnectionType getConnectionType() const;
	void setConnectionType(ConnectionType ct);

	const QString &getUser() const;
	void setUser(const QString &host);

	const QString &getPassword() const;
	void setPassword(const QString &password);

	SmtpClient::AuthMethod getAuthMethod() const;
	void setAuthMethod(AuthMethod method);

	const QString &getResponseText() const;
	int getResponseCode() const;

	int getConnectionTimeout() const;
	void setConnectionTimeout(int msec);

	int getResponseTimeout() const;
	void setResponseTimeout(int msec);

	QTcpSocket *getSocket();

	bool connectToHost();
	bool login();
	bool login(const QString &user, const QString &password, AuthMethod method = AuthLogin);

	bool sendMail(MimeMessage &email);
	void quit();

protected:
	QTcpSocket *socket;
	QString host;
	int port;
	ConnectionType connectionType;
	QString name;

	QString user;
	QString password;
	AuthMethod authMethod;

	int connectionTimeout;
	int responseTimeout;

	QString responseText;
	int responseCode;

	class ResponseTimeoutException {};
    void waitForResponse();
	void sendMessage(const QString &text);

signals:
	void smtpError(SmtpError e);

};

#endif // SMTPCLIENT_H
