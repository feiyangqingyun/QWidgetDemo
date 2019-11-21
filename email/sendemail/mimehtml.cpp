#include "mimehtml.h"

MimeHtml::MimeHtml(const QString &html) : MimeText(html)
{
	this->cType = "text/html";
}

MimeHtml::~MimeHtml() {}

void MimeHtml::setHtml(const QString &html)
{
	this->text = html;
}

const QString &MimeHtml::getHtml() const
{
	return text;
}

void MimeHtml::prepare()
{
	MimeText::prepare();
}
