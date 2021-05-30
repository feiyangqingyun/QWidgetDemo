#ifndef MIMETEXT_H
#define MIMETEXT_H

#include "mimepart.h"

class MimeText : public MimePart
{
public:

	MimeText(const QString &text = "");
	~MimeText();

	void setText(const QString &text);
	const QString &getText() const;

protected:
	QString text;
	void prepare();

};

#endif // MIMETEXT_H
