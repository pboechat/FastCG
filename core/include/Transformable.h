#ifndef TRANSFORMABLE_H_
#define TRANSFORMABLE_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transformable
{
public:
	Transformable();
	virtual ~Transformable();

	const glm::vec3& GetPosition() const
	{
		return mPosition;
	}

	const glm::quat& GetRotation() const
	{
		return mRotation;
	}

	const glm::vec3& GetScale() const
	{
		return mScale;
	}

	void SetPosition(const glm::vec3& position)
	{
		mPosition = position;
	}

	void SetRotation(const glm::quat& rotation)
	{
		mRotation = rotation;
	}

	void SetScale(const glm::vec3& scale)
	{
		mScale = scale;
	}

protected:
	glm::vec3 mPosition;
	glm::quat mRotation;
	glm::vec3 mScale;

};

#endif
