#include "mimemultipart.h"
#include "stdlib.h"
#include <QTime>
#include <QCryptographicHash>

const QString MULTI_PART_NAMES[] = {
    "multipart/mixed",         //    Mixed
    "multipart/digest",        //    Digest
    "multipart/alternative",   //    Alternative
    "multipart/related",       //    Related
    "multipart/report",        //    Report
    "multipart/signed",        //    Signed
    "multipart/encrypted"      //    Encrypted
};

MimeMultiPart::MimeMultiPart(MultiPartType type)
{
    this->type = type;
    this->cType = MULTI_PART_NAMES[this->type];
    this->cEncoding = _8Bit;

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QByteArray().append(rand()));
    cBoundary = md5.result().toHex();
}

MimeMultiPart::~MimeMultiPart()
{

}

void MimeMultiPart::addPart(MimePart *part)
{
    parts.append(part);
}

const QList<MimePart *> &MimeMultiPart::getParts() const
{
    return parts;
}

void MimeMultiPart::prepare()
{
    QList<MimePart *>::iterator it;
    content = "";

    for (it = parts.begin(); it != parts.end(); it++) {
        content += "--" + cBoundary.toUtf8() + "\r\n";
        (*it)->prepare();
        content += (*it)->toString().toUtf8();
    };

    content += "--" + cBoundary.toUtf8() + "--\r\n";
    MimePart::prepare();
}

void MimeMultiPart::setMimeType(const MultiPartType type)
{
    this->type = type;
    this->cType = MULTI_PART_NAMES[type];
}

MimeMultiPart::MultiPartType MimeMultiPart::getMimeType() const
{
    return type;
}
