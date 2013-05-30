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

	inline glm::vec3 GetPosition() const
	{
		return glm::vec3(GetModel()[3]);
	}

	inline void SetPosition(const glm::vec3& position)
	{
		if (mpParent != 0)
		{
			SetLocalPosition(position - mpParent->GetPosition());
		}
		else
		{
			SetLocalPosition(position);
		}
	}

	inline glm::vec3 GetLocalPosition() const
	{
		return glm::vec3(mLocalTransform[3]);
	}

	inline void SetLocalPosition(const glm::vec3& position)
	{
		mLocalTransform[3] = glm::vec4(position, 1.0f);
	}

	inline glm::quat GetRotation() const
	{
		return glm::toQuat(GetModel());
	}

	inline void SetRotation(const glm::quat& rRotation)
	{
		if (mpParent != 0)
		{
			SetLocalRotation(glm::inverse(GetRotation()) * rRotation);
		}
		else
		{
			SetLocalRotation(rRotation);
		}
	}

	inline glm::quat GetLocalRotation() const
	{
		return glm::toQuat(mLocalTransform);
	}

	inline void SetLocalRotation(const glm::quat& rLocalRotation)
	{
		glm::mat4 transform = glm::toMat4(rLocalRotation);
		transform = glm::translate(transform, GetLocalPosition());
		transform = glm::scale(transform, GetLocalScale());
		mLocalTransform = transform;
	}

	inline glm::vec3 GetScale() const
	{
		glm::mat4& rModel = GetModel();
		return glm::vec3(rModel[0][0], rModel[1][1], rModel[2][2]);
	}

	inline glm::vec3 GetLocalScale() const
	{
		return glm::vec3(mLocalTransform[0][0], mLocalTransform[1][1], mLocalTransform[2][2]);
	}

	inline void Rotate(const glm::vec3& rEulerAngles)
	{
		mLocalTransform = mLocalTransform * glm::toMat4(glm::quat(rEulerAngles));
	}

	inline void ScaleLocal(const glm::vec3& rScale)
	{
		mLocalTransform = glm::scale(mLocalTransform, rScale);
	}

	inline void RotateAround(float angle, const glm::vec3& axis)
	{
		mLocalTransform = glm::rotate(GetModel(), angle, axis);
	}

	inline void RotateAroundLocal(float angle, const glm::vec3& axis)
	{
		mLocalTransform = glm::rotate(mLocalTransform, angle, axis);
	}

	inline glm::vec3 GetForward() const
	{
		return glm::normalize(GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
	}

	inline glm::vec3 GetUp() const
	{
		return glm::normalize(GetRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
	}

	inline glm::mat4 GetModel() const
	{
		if (mpParent != 0)
		{
			return mpParent->GetModel() * mLocalTransform;
		}
		else
		{
			return mLocalTransform;
		}
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
	glm::mat4 mLocalTransform;

	Transform(GameObject* pGameObject) :
		mpGameObject(pGameObject),
		mpParent(0)
	{
	}

	~Transform() 
	{
	}

};

#endif
