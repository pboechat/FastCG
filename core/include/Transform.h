#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <Component.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
	inline glm::vec3 GetPosition() const
	{
		return mWorldPosition;
	}

	inline void SetPosition(const glm::vec3& position)
	{
		mWorldPosition = position;
	}

	inline glm::mat3 GetRotation() const
	{
		return glm::toMat3(mWorldRotation);
	}

	inline glm::mat4 GetModel() const
	{
		glm::mat4 worldTransform = glm::toMat4(mWorldRotation);
		worldTransform = glm::translate(worldTransform, mWorldPosition);
		glm::mat4 localTransform = glm::toMat4(mLocalRotation);
		return worldTransform * localTransform;
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
		mWorldRotation = glm::rotate(mWorldRotation, angle, axis);
	}

	inline void RotateAroundLocal(float angle, const glm::vec3& axis)
	{
		mLocalRotation = glm::rotate(mLocalRotation, angle, axis);
	}

	inline glm::vec3 GetForward() const
	{
		return glm::normalize(mLocalRotation * glm::vec3(0.0f, 0.0f, -1.0f));
	}

	inline glm::vec3 GetUp() const
	{
		return glm::normalize(mLocalRotation * glm::vec3(0.0f, 1.0f, 0.0f));
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
	glm::vec3 mWorldPosition;
	glm::quat mWorldRotation;
	glm::quat mLocalRotation;

	Transform(GameObject* pGameObject) :
		mpGameObject(pGameObject),
		mWorldPosition(0.0f, 0.0f, 0.0f)
	{
	}

	~Transform() 
	{
	}

};

#endif
