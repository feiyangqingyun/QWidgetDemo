#ifndef MIMEFILE_H
#define MIMEFILE_H

#include "mimepart.h"
#include <QFile>

class MimeFile : public MimePart
{
	Q_OBJECT
public:

	MimeFile(QFile *f);
	~MimeFile();

protected:
	QFile *file;
	virtual void prepare();

};

#endif // MIMEFILE_H
