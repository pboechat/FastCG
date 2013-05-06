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

	template<class U>
	Pointer(U* pObject);

	template<class U>
	Pointer(Pointer<U>& rPointer);
	~Pointer();

	inline operator T* () const;
	inline T& operator*() const;
	inline T* operator->() const;

	inline T* Get() const;

	template<class U>
	Pointer<T>& operator=(U* pObject);

	Pointer<T>& operator=(const Pointer<T>& rPointer);

	template<class U>
	Pointer<T>& operator=(/*const */Pointer<U>& rPointer);

	template<class U>
	inline bool operator==(U* pObject) const;

	template<class U>
	inline bool operator!=(U* pObject) const;

	template<class U>
	inline bool operator==(const Pointer<U>& rReference) const;

	template<class U>
	inline bool operator!=(const Pointer<U>& rReference) const;

	inline ReferenceCounter* GetReferenceCounter();

protected:
	T* mpObject;
	ReferenceCounter* mpReferenceCounter;

};

template<class T>
Pointer<T>::Pointer(T* pObject = 0) :
	mpObject(pObject),
	mpReferenceCounter(0)
{
	mpReferenceCounter = new ReferenceCounter();
	mpReferenceCounter->AddReference();
}

template<class T>
template<class U>
Pointer<T>::Pointer(U* pObject) :
	mpObject(pObject),
	mpReferenceCounter(0)
{
	mpReferenceCounter = new ReferenceCounter();
	mpReferenceCounter->AddReference();
}

template<class T>
template<class U>
Pointer<T>::Pointer(Pointer<U>& rPointer) :
	mpObject(rPointer.Get()),
	mpReferenceCounter(rPointer.GetReferenceCounter())
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
inline ReferenceCounter* Pointer<T>::GetReferenceCounter()
{
	return mpReferenceCounter;
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
template<class U>
inline Pointer<T>& Pointer<T>::operator=(U* pObject)
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
Pointer<T>& Pointer<T>::operator=(const Pointer<T>& rPointer)
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
template<class U>
inline Pointer<T>& Pointer<T>::operator=(/*const */Pointer<U>& rPointer)
{
	if (mpObject != rPointer.Get())
	{
		if (mpReferenceCounter->Release() == 0)
		{
			delete mpObject;
			delete mpReferenceCounter;
		}

		mpObject = rPointer.Get();
		mpReferenceCounter = rPointer.GetReferenceCounter();
		mpReferenceCounter->AddReference();
	}

	return *this;
}

template<class T>
template<class U>
inline bool Pointer<T>::operator==(U* pObject) const
{
	return mpObject == pObject;
}

template<class T>
template<class U>
inline bool Pointer<T>::operator!=(U* pObject) const
{
	return mpObject != pObject;
}

template<class T>
template<class U>
inline bool Pointer<T>::operator==(const Pointer<U>& rPointer) const
{
	return mpObject == rPointer.Get();
}

template<class T>
template<class U>
inline bool Pointer<T>::operator!=(const Pointer<U>& rPointer) const
{
	return mpObject != rPointer.Get();
}

#endif
