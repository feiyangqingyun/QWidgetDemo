#ifndef MIMEMESSAGE_H
#define MIMEMESSAGE_H

#include "mimepart.h"
#include "mimemultipart.h"
#include "emailaddress.h"
#include <QList>

class MimeMessage : public QObject
{
public:

	enum RecipientType {
		To,                 // primary
		Cc,                 // carbon copy
		Bcc                 // blind carbon copy
	};


	MimeMessage(bool createAutoMimeConent = true);
	~MimeMessage();

	void setSender(EmailAddress *e);
	void addRecipient(EmailAddress *rcpt, RecipientType type = To);
	void addTo(EmailAddress *rcpt);
	void addCc(EmailAddress *rcpt);
	void addBcc(EmailAddress *rcpt);
	void setSubject(const QString &subject);
	void addPart(MimePart *part);

	void setHeaderEncoding(MimePart::Encoding);

	const EmailAddress &getSender() const;
	const QList<EmailAddress *> &getRecipients(RecipientType type = To) const;
	const QString &getSubject() const;
	const QList<MimePart *> &getParts() const;

	MimePart &getContent();
	void setContent(MimePart *content);

	virtual QString toString();


protected:
	EmailAddress *sender;
	QList<EmailAddress *> recipientsTo, recipientsCc, recipientsBcc;
	QString subject;
	MimePart *content;

	MimePart::Encoding hEncoding;

};

#endif // MIMEMESSAGE_H
