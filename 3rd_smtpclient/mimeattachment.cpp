#include "mimeattachment.h"
#include <QFileInfo>

MimeAttachment::MimeAttachment(QFile *file)
	: MimeFile(file)
{
}

MimeAttachment::~MimeAttachment()
{
}

void MimeAttachment::prepare()
{
	this->header += "Content-disposition: attachment\r\n";
	MimeFile::prepare();
}
