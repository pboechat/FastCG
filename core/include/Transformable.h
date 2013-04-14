#ifndef TRANSFORMABLE_H_
#define TRANSFORMABLE_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transformable
{
public:
	Transformable() :
		mModel(1.0f)
	{
	}

	virtual ~Transformable()
	{
	}

	inline glm::vec3 GetPosition() const
	{
		return glm::vec3(mModel[3]);
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

protected:
	glm::mat4 mModel;

};

#endif
