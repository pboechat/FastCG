#ifndef POINTER_H_
#define POINTER_H_

class ReferenceCounter
{
public:
	ReferenceCounter() :
		mCounter(0)
	{
	}

	inline void AddReference()
	{
		mCounter++;
	}

	inline int Release()
	{
		return mCounter;
	}

private:
	int mCounter;

};

template<class T>
class Pointer
{
public:
	Pointer(T* pObject = 0);
	Pointer(const Pointer& rPointer);
	~Pointer();

	inline operator T* () const;
	inline T& operator*() const;
	inline T* operator->() const;

	inline T* Get() const;

	Pointer& operator=(T* pObject);
	Pointer& operator=(const Pointer& rPointer);

	inline bool operator==(T* pObject) const;
	inline bool operator!=(T* pObject) const;
	inline bool operator==(const Pointer& rReference) const;
	inline bool operator!=(const Pointer& rReference) const;

protected:
	T* mpObject;
	ReferenceCounter* mpReferenceCounter;

};

template<class T>
Pointer<T>::Pointer(T* pObject) :
	mpObject(pObject),
	mpReferenceCounter(0)
{
	mpReferenceCounter = new ReferenceCounter();
	mpReferenceCounter->AddReference();
}

template<class T>
Pointer<T>::Pointer(const Pointer& rPointer) :
	mpObject(rPointer.mpObject),
	mpReferenceCounter(rPointer.mpReferenceCounter)
{
	mpReferenceCounter->AddReference();
}

template<class T>
Pointer<T>::~Pointer()
{
	if (mpReferenceCounter->Release() == 0)
	{
		delete mpObject;
		delete mpReferenceCounter;
	}
}

template<class T>
inline Pointer<T>::operator T* () const
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
		if (mpReferenceCounter->Release() == 0)
		{
			delete mpObject;
			delete mpReferenceCounter;
		}

		mpObject = pObject;
		mpReferenceCounter = new ReferenceCounter();
		mpReferenceCounter->AddReference();
	}

	return *this;
}

template<class T>
Pointer<T>& Pointer<T>::operator=(const Pointer& rPointer)
{
	if (mpObject != rPointer.mpObject)
	{
		if (mpReferenceCounter->Release() == 0)
		{
			delete mpObject;
			delete mpReferenceCounter;
		}

		mpObject = rPointer.mpObject;
		mpReferenceCounter = rPointer.mpReferenceCounter;
		mpReferenceCounter->AddReference();
	}

	return *this;
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
