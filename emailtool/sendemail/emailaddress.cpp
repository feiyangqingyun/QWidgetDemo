#include "emailaddress.h"

EmailAddress::EmailAddress(const QString &address, const QString &name)
{
	this->address = address;
	this->name = name;
}

EmailAddress::~EmailAddress()
{
}

void EmailAddress::setName(const QString &name)
{
	this->name = name;
}

void EmailAddress::setAddress(const QString &address)
{
	this->address = address;
}

const QString &EmailAddress::getName() const
{
	return name;
}

const QString &EmailAddress::getAddress() const
{
	return address;
}
