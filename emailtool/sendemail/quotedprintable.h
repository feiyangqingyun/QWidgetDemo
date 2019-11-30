#ifndef QUOTEDPRINTABLE_H
#define QUOTEDPRINTABLE_H

#include <QObject>
#include <QByteArray>

class QuotedPrintable : public QObject
{
	Q_OBJECT
public:

	static QString &encode(const QByteArray &input);
	static QByteArray &decode(const QString &input);

private:
	QuotedPrintable();
};

#endif // QUOTEDPRINTABLE_H
