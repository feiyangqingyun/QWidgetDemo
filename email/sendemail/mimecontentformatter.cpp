#include "mimecontentformatter.h"

MimeContentFormatter::MimeContentFormatter(int max_length) :
	max_length(max_length)
{}

QString MimeContentFormatter::format(const QString &content, bool quotedPrintable) const
{
	QString out;

	int chars = 0;

	for (int i = 0; i < content.length() ; ++i) {
		chars++;

		if (!quotedPrintable) {
			if (chars > max_length) {
				out.append("\r\n");
				chars = 1;
			}
		} else {
			if (content.at(i) == '\n') {       // new line
				out.append(content.at(i));
				chars = 0;
				continue;
			}

			if ((chars > max_length - 1)
			        || ((content.at(i) == '=') && (chars > max_length - 3))) {
				out.append('=');
				out.append("\r\n");
				chars = 1;
			}

		}

		out.append(content.at(i));
	}

	return out;
}

void MimeContentFormatter::setMaxLength(int l)
{
	max_length = l;
}

int MimeContentFormatter::getMaxLength() const
{
	return max_length;
}
