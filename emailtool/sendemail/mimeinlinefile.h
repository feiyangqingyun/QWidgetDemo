#ifndef MIMEINLINEFILE_H
#define MIMEINLINEFILE_H

#include "mimefile.h"

class MimeInlineFile : public MimeFile
{
public:

	MimeInlineFile(QFile *f);
	~MimeInlineFile();

protected:
	virtual void prepare();

};

#endif // MIMEINLINEFILE_H
