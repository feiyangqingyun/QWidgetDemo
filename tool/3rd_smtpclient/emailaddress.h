#ifndef EMAILADDRESS_H
#define EMAILADDRESS_H

#include <QObject>

class EmailAddress : public QObject
{
	Q_OBJECT
public:
	EmailAddress(const QString &address, const QString &name = "");
	~EmailAddress();

	void setName(const QString &name);
	void setAddress(const QString &address);

	const QString &getName() const;
	const QString &getAddress() const;

private:
	QString name;
	QString address;

};

#endif // EMAILADDRESS_H
