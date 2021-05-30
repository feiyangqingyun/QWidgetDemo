#include "mimetext.h"

MimeText::MimeText(const QString &txt)
{
	this->text = txt;
	this->cType = "text/plain";
	this->cCharset = "utf-8";
	this->cEncoding = _8Bit;
}

MimeText::~MimeText() { }

void MimeText::setText(const QString &text)
{
	this->text = text;
}

const QString &MimeText::getText() const
{
	return text;
}

void MimeText::prepare()
{
	this->content.clear();
    this->content.append(text.toUtf8());
	MimePart::prepare();
}
