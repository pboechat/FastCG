#ifndef POINTER_H_
#define POINTER_H_

template<class T>
class Pointer
{
public:
	Pointer(T* pObject = 0);
	Pointer(const Pointer& rPointer);
	~Pointer();

	inline operator T*() const;
	inline T& operator*() const;
	inline T* operator->() const;

	inline T* Get() const;

	Pointer& operator=(T* pObject);
	Pointer& operator=(Pointer& rReference);

	inline void IncrementReferences();
	inline void DecrementReferences();

	inline bool operator==(T* pObject) const;
	inline bool operator!=(T* pObject) const;
	inline bool operator==(const Pointer& rReference) const;
	inline bool operator!=(const Pointer& rReference) const;

protected:
	T* mpObject;
	int mReferences;

};

template<class T>
Pointer<T>::Pointer(T* pObject)
{
	mpObject = pObject;
	if (mpObject)
	{
		IncrementReferences();
	}
}

template<class T>
Pointer<T>::Pointer(const Pointer& rPointer)
{
	mpObject = rPointer.mpObject;
	if (mpObject)
	{
		IncrementReferences();
	}
}

template<class T>
Pointer<T>::~Pointer()
{
	if (mpObject)
	{
		DecrementReferences();
	}
}

template<class T>
inline Pointer<T>::operator T*() const
{
	return mpObject;
}

template<class T>
inline T& Pointer<T>::operator*() const
{
	return *mpObject;
}

template<class T>
inline T* Pointer<T>::operator->() const
{
	return mpObject;
}

template<class T>
inline T* Pointer<T>::Get() const
{
	return mpObject;
}

template<class T>
Pointer<T>& Pointer<T>::operator=(T* pObject)
{
	if (mpObject != pObject)
	{
		if (pObject)
		{
			IncrementReferences();
		}

		if (mpObject)
		{
			DecrementReferences();
		}

		mpObject = pObject;
	}

	return *this;
}

template<class T>
Pointer<T>& Pointer<T>::operator=(Pointer& rPointer)
{
	if (mpObject != rPointer.mpObject)
	{
		if (rPointer.mpObject)
		{
			rPointer.IncrementReferences();
		}

		if (mpObject)
		{
			DecrementReferences();
		}

		mpObject = rPointer.mpObject;
	}

	return *this;
}

template<class T>
inline void Pointer<T>::IncrementReferences()
{
	mReferences++;
}

template<class T>
inline void Pointer<T>::DecrementReferences()
{
	if (--mReferences == 0)
	{
		delete mpObject;
	}
}

template<class T>
inline bool Pointer<T>::operator==(T* pObject) const
{
	return mpObject == pObject;
}

template<class T>
inline bool Pointer<T>::operator!=(T* pObject) const
{
	return mpObject != pObject;
}

template<class T>
inline bool Pointer<T>::operator==(const Pointer& rPointer) const
{
	return mpObject == rPointer.mpObject;
}

template<class T>
inline bool Pointer<T>::operator!=(const Pointer& rPointer) const
{
	return mpObject != rPointer.mpObject;
}

#endif
