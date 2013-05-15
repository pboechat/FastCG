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
		return glm::vec3(mModel[3]);
	}

	inline void SetPosition(const glm::vec3& position)
	{
		mModel[3] = glm::vec4(position, 1.0f);
	}

	inline glm::mat3 GetRotation() const
	{
		return glm::mat3(mModel);
	}

	inline const glm::mat4& GetModel() const
	{
		return mModel;
	}

	inline void Translate(const glm::vec3& position)
	{
		mModel = glm::translate(mModel, position);
	}

	inline void Rotate(float angle, const glm::vec3& axis)
	{
		mModel = glm::rotate(mModel, angle, axis);
	}

	inline void RotateAround(float angle, const glm::vec3& axis)
	{
		glm::quat rotation = glm::angleAxis(angle, axis);
		mModel = glm::toMat4(rotation) * mModel;
	}

	inline void RotateAroundLocal(float angle, const glm::vec3& axis)
	{
		glm::vec3 center = GetPosition();
		glm::quat rotation = glm::angleAxis(angle, axis);
		mModel = glm::translate(glm::toMat4(rotation), center);
	}

	inline glm::vec3 GetForward() const
	{
		return glm::normalize(GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
	}

	inline glm::vec3 GetUp() const
	{
		return glm::normalize(GetRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
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
	glm::mat4 mModel;

	Transform(GameObject* pGameObject) :
		mpGameObject(pGameObject),
		mModel(1.0f)
	{
	}

	~Transform() 
	{
	}

};

#endif
