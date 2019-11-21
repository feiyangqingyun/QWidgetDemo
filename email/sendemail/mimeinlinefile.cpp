#include "mimeinlinefile.h"

MimeInlineFile::MimeInlineFile(QFile *f)
	: MimeFile(f)
{
}

MimeInlineFile::~MimeInlineFile()
{}

void MimeInlineFile::prepare()
{
	this->header += "Content-Disposition: inline\r\n";
	MimeFile::prepare();
}
