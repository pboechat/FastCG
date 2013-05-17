#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <Component.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
	inline Transform* GetParent()
	{
		return mpParent;
	}

	inline void SetParent(Transform* pParent)
	{
		mpParent = pParent;
	}

	inline glm::vec3 GetWorldPosition() const
	{
		if (mpParent != 0)
		{
			return mpParent->GetWorldPosition() + mWorldPosition;
		}
		else
		{
			return mWorldPosition;
		}
	}

	inline void SetWorldPosition(const glm::vec3& position)
	{
		if (mpParent != 0)
		{
			mWorldPosition = position - mpParent->GetWorldPosition();
		}
		else
		{
			mWorldPosition = position;
		}
	}

	inline void SetLocalPosition(const glm::vec3& position)
	{
		mLocalPosition = position;
	}

	inline glm::quat GetWorldRotation() const
	{
		if (mpParent != 0)
		{
			return GetWorldRotation() * mWorldRotation;
		}
		else
		{
			return mWorldRotation;
		}
	}

	inline glm::mat4 GetWorldTransform() const
	{
		glm::mat4 worldTransform = glm::toMat4(mWorldRotation);
		worldTransform = glm::translate(worldTransform, mWorldPosition);

		if (mpParent != 0)
		{
			return mpParent->GetWorldTransform() * worldTransform;
		}
		else 
		{
			return worldTransform;
		}
	}

	inline void SetWorldTransform(const glm::mat4& rWorldTransform)
	{
		mWorldRotation = glm::toQuat(rWorldTransform);
		mWorldPosition = glm::vec3(rWorldTransform[3]);
	}

	inline void SetLocalTransform(const glm::mat4& rLocalTransform)
	{
		mLocalRotation = glm::toQuat(rLocalTransform);
		mLocalPosition = glm::vec3(rLocalTransform[3]);
	}

	inline glm::quat GetLocalRotation() const
	{
		return mLocalRotation;
	}

	inline const glm::vec3& GetScale() const
	{
		return mScale;
	}

	inline void SetScale(const glm::vec3& rScale)
	{
		mScale = rScale;
	}

	inline glm::mat4 GetModel() const
	{
		glm::mat4 localTransform = glm::toMat4(mLocalRotation);
		localTransform = glm::translate(localTransform, mLocalPosition);
		return GetWorldTransform() * localTransform;
	}

	inline void Translate(const glm::vec3& position)
	{
		mWorldPosition += mLocalRotation * position;
	}

	inline void Rotate(const glm::vec3& rEulerAngles)
	{
		mWorldRotation = mWorldRotation * glm::quat(rEulerAngles);
	}

	inline void RotateAround(float angle, const glm::vec3& axis)
	{
		glm::mat4 worldTransform = glm::toMat4(mWorldRotation);
		worldTransform = glm::translate(worldTransform, mWorldPosition);
		worldTransform = glm::rotate(worldTransform, angle, axis);
		SetWorldTransform(worldTransform);
	}

	inline void RotateAroundLocal(float angle, const glm::vec3& axis)
	{
		glm::mat4 localTransform = glm::toMat4(mLocalRotation);
		localTransform = glm::translate(localTransform, mLocalPosition);
		localTransform = glm::rotate(localTransform, angle, axis);
		SetLocalTransform(localTransform);
	}

	inline glm::vec3 GetForward() const
	{
		return glm::normalize(GetWorldRotation() * mLocalRotation * glm::vec3(0.0f, 0.0f, -1.0f));
	}

	inline glm::vec3 GetUp() const
	{
		return glm::normalize(GetWorldRotation() * mLocalRotation * glm::vec3(0.0f, 1.0f, 0.0f));
	}

	inline GameObject* GetGameObject()
	{
		return mpGameObject;
	}

	inline const GameObject* GetGameObject() const
	{
		return mpGameObject;
	}

	friend class GameObject;

private:
	GameObject* mpGameObject;
	Transform* mpParent;
	glm::vec3 mWorldPosition;
	glm::quat mWorldRotation;
	glm::vec3 mLocalPosition;
	glm::quat mLocalRotation;
	glm::vec3 mScale;

	Transform(GameObject* pGameObject) :
		mpGameObject(pGameObject),
		mpParent(0),
		mWorldPosition(0.0f, 0.0f, 0.0f),
		mLocalPosition(0.0f, 0.0f, 0.0f),
		mScale(1.0f, 1.0f, 1.0f)
	{
	}

	~Transform() 
	{
	}

};

#endif
