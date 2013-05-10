#ifndef TYPE_H_
#define TYPE_H_

#include <string>

class Type
{
public:
	Type(const std::string& rName, const Type* pBaseType);
	~Type();

	inline const std::string& GetName() const
	{
		return mName;
	}

	inline bool IsExactly(const Type& rType) const
	{
		return &rType == this;
	}

	inline const Type* GetBaseType() const
	{
		return mpBaseType;
	}

	bool IsDerived(const Type& rType) const;

private:
	std::string mName;
	const Type* mpBaseType;

};

#define DECLARE_TYPE \
public: \
	static const Type TYPE; \
	inline virtual const Type& GetType() const { return TYPE; } \

#define IMPLEMENT_TYPE(className, baseClassName) \
	const Type className::TYPE(#className, &baseClassName::TYPE)

#endif