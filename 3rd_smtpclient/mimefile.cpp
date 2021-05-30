#include "mimefile.h"
#include <QFileInfo>

MimeFile::MimeFile(QFile *file)
{
	this->file = file;
	this->cType = "application/octet-stream";
	this->cName = QFileInfo(*file).fileName();
	this->cEncoding = Base64;
}

MimeFile::~MimeFile()
{
	delete file;
}

void MimeFile::prepare()
{
	file->open(QIODevice::ReadOnly);
	this->content = file->readAll();
	file->close();
	MimePart::prepare();
}
