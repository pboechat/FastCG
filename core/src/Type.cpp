#include <Type.h>

Type::Type(const std::string& rName, const Type* pBaseType) :
		mName(rName),
		mpBaseType(pBaseType)
{
}

Type::~Type()
{
}

bool Type::IsDerived(const Type& rType) const
{
	const Type* pSearch = this;
	while (pSearch)
	{
		if (pSearch == &rType)
		{
			return true;
		}

		pSearch = pSearch->mpBaseType;
	}

	return false;
}