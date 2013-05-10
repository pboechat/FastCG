#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <Component.h>
#include <GameObject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform : public Component
{
	DECLARE_TYPE;

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

	inline glm::vec3 GetForward() const
	{
		return glm::vec3(mModel * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
	}

	inline glm::vec3 GetUp() const
	{
		return glm::vec3(mModel * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	friend class GameObject;

private:
	glm::mat4 mModel;

	Transform() :
		mModel(1.0f)
	{
	}

	~Transform()
	{
	}

};

typedef Pointer<Transform> TransformPtr;

#endif
