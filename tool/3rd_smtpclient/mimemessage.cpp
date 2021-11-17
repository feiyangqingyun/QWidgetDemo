#include "mimemessage.h"

#include <QDateTime>
#include "quotedprintable.h"
#include <typeinfo>

MimeMessage::MimeMessage(bool createAutoMimeContent) :
    hEncoding(MimePart::_8Bit)
{
    if (createAutoMimeContent) {
        this->content = new MimeMultiPart();
    }
}

MimeMessage::~MimeMessage()
{
}

MimePart &MimeMessage::getContent()
{
    return *content;
}

void MimeMessage::setContent(MimePart *content)
{
    this->content = content;
}

void MimeMessage::setSender(EmailAddress *e)
{
    this->sender = e;
}

void MimeMessage::addRecipient(EmailAddress *rcpt, RecipientType type)
{
    switch (type) {
        case To:
            recipientsTo << rcpt;
            break;

        case Cc:
            recipientsCc << rcpt;
            break;

        case Bcc:
            recipientsBcc << rcpt;
            break;
    }
}

void MimeMessage::addTo(EmailAddress *rcpt)
{
    this->recipientsTo << rcpt;
}

void MimeMessage::addCc(EmailAddress *rcpt)
{
    this->recipientsCc << rcpt;
}

void MimeMessage::addBcc(EmailAddress *rcpt)
{
    this->recipientsBcc << rcpt;
}

void MimeMessage::setSubject(const QString &subject)
{
    this->subject = subject;
}

void MimeMessage::addPart(MimePart *part)
{
    if (typeid(*content) == typeid(MimeMultiPart)) {
        ((MimeMultiPart *) content)->addPart(part);
    };
}

void MimeMessage::setHeaderEncoding(MimePart::Encoding hEnc)
{
    this->hEncoding = hEnc;
}

const EmailAddress &MimeMessage::getSender() const
{
    return *sender;
}

const QList<EmailAddress *> &MimeMessage::getRecipients(RecipientType type) const
{
    switch (type) {
        default:
        case To:
            return recipientsTo;

        case Cc:
            return recipientsCc;

        case Bcc:
            return recipientsBcc;
    }
}

const QString &MimeMessage::getSubject() const
{
    return subject;
}

const QList<MimePart *> &MimeMessage::getParts() const
{
    if (typeid(*content) == typeid(MimeMultiPart)) {
        return ((MimeMultiPart *) content)->getParts();
    } else {
        QList<MimePart *> *res = new QList<MimePart *>();
        res->append(content);
        return *res;
    }
}

QString MimeMessage::toString()
{
    QString mime;
    mime = "From:";

    QByteArray name = sender->getName().toUtf8();
    if (name != "") {
        switch (hEncoding) {
            case MimePart::Base64:
                mime += " =?utf-8?B?" + QByteArray().append(name).toBase64() + "?=";
                break;

            case MimePart::QuotedPrintable:
                mime += " =?utf-8?Q?" + QuotedPrintable::encode(QByteArray().append(name)).replace(' ', "_").replace(':', "=3A") + "?=";
                break;

            default:
                mime += " " + name;
        }
    }

    mime += " <" + sender->getAddress() + ">\r\n";

    mime += "To:";
    QList<EmailAddress *>::iterator it;
    int i;

    for (i = 0, it = recipientsTo.begin(); it != recipientsTo.end(); ++it, ++i) {
        if (i != 0) {
            mime += ",";
        }

        QByteArray name = (*it)->getName().toUtf8();
        if (name != "") {
            switch (hEncoding) {
                case MimePart::Base64:
                    mime += " =?utf-8?B?" + QByteArray().append(name).toBase64() + "?=";
                    break;

                case MimePart::QuotedPrintable:
                    mime += " =?utf-8?Q?" + QuotedPrintable::encode(QByteArray().append(name)).replace(' ', "_").replace(':', "=3A") + "?=";
                    break;

                default:
                    mime += " " + name;
            }
        }

        mime += " <" + (*it)->getAddress() + ">";
    }

    mime += "\r\n";
    if (recipientsCc.size() != 0) {
        mime += "Cc:";
    }

    for (i = 0, it = recipientsCc.begin(); it != recipientsCc.end(); ++it, ++i) {
        if (i != 0) {
            mime += ",";
        }

        QByteArray name = (*it)->getName().toUtf8();
        if (name != "") {
            switch (hEncoding) {
                case MimePart::Base64:
                    mime += " =?utf-8?B?" + QByteArray().append(name).toBase64() + "?=";
                    break;

                case MimePart::QuotedPrintable:
                    mime += " =?utf-8?Q?" + QuotedPrintable::encode(QByteArray().append(name)).replace(' ', "_").replace(':', "=3A") + "?=";
                    break;

                default:
                    mime += " " + name;
            }
        }

        mime += " <" + (*it)->getAddress() + ">";
    }

    if (recipientsCc.size() != 0) {
        mime += "\r\n";
    }

    mime += "Subject: ";

    switch (hEncoding) {
        case MimePart::Base64:
            mime += "=?utf-8?B?" + QByteArray().append(subject.toUtf8()).toBase64() + "?=";
            break;

        case MimePart::QuotedPrintable:
            mime += "=?utf-8?Q?" + QuotedPrintable::encode(QByteArray().append(subject.toUtf8())).replace(' ', "_").replace(':', "=3A") + "?=";
            break;

        default:
            mime += subject;
    }

    mime += "\r\n";
    mime += "MIME-Version: 1.0\r\n";

    mime += content->toString();
    return mime;
}
