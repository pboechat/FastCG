#ifndef FASTCG_TRANSFORM_H
#define FASTCG_TRANSFORM_H

#include <FastCG/Component.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace FastCG
{
	class WorldSystem;

	class Transform
	{
	public:
		inline Transform *GetParent()
		{
			return mpParent;
		}

		inline void SetParent(Transform *pParent)
		{
			if (mpParent != pParent)
			{
				pParent->AddChild(this);
			}

			mpParent = pParent;

			Update();
		}

		inline const std::vector<Transform *> &GetChildren() const
		{
			return mChildren;
		}

		inline const glm::vec3 &GetPosition() const
		{
			return mWorldTransform.position;
		}

		inline void SetPosition(const glm::vec3 &position)
		{
			mLocalTransform.position = position;
			Update();
		}

		inline const glm::quat &GetRotation() const
		{
			return mWorldTransform.rotation;
		}

		inline void SetRotation(const glm::quat &rRotation)
		{
			mLocalTransform.rotation = rRotation;
			Update();
		}

		inline const glm::vec3 &GetScale() const
		{
			return mWorldTransform.scale;
		}

		inline void SetScale(const glm::vec3 &rScale)
		{
			mLocalTransform.scale = rScale;
			Update();
		}

		inline void Rotate(const glm::vec3 &rEulerAngles)
		{
			mLocalTransform.rotation = mLocalTransform.rotation * glm::quat(rEulerAngles);
			Update();
		}

		inline void RotateAround(float angle, const glm::vec3 &rAxis)
		{
			mLocalTransform.rotation = glm::rotate(mLocalTransform.rotation, angle, rAxis);
			Update();
		}

		inline void RotateAroundLocal(float angle, const glm::vec3 &rAxis)
		{
			auto newLocal = glm::rotate(glm::mat4(), angle, rAxis);
			newLocal = glm::translate(newLocal, mLocalTransform.position);

			mLocalTransform.position = glm::vec3(newLocal[3][0], newLocal[3][1], newLocal[3][2]);
			mLocalTransform.rotation = glm::quat(newLocal);

			Update();
		}

		inline glm::vec3 GetUp() const
		{
			return glm::normalize(mWorldTransform.rotation * glm::vec3(0, 1, 0));
		}

		inline glm::vec3 GetDown() const
		{
			return glm::normalize(mWorldTransform.rotation * glm::vec3(0, -1, 0));
		}

		inline glm::vec3 GetRight() const
		{
			return glm::normalize(mWorldTransform.rotation * glm::vec3(1, 0, 0));
		}

		inline glm::vec3 GetLeft() const
		{
			return glm::normalize(mWorldTransform.rotation * glm::vec3(-1, 0, 0));
		}

		inline glm::vec3 GetForward() const
		{
			return glm::normalize(mWorldTransform.rotation * glm::vec3(0, 0, -1));
		}

		inline glm::vec3 GetBack() const
		{
			return glm::normalize(mWorldTransform.rotation * glm::vec3(0, 0, 1));
		}

		inline glm::mat4 GetModel() const
		{
			return mWorldTransform.ToMat4();
		}

		inline GameObject *GetGameObject()
		{
			return mpGameObject;
		}

		inline const GameObject *GetGameObject() const
		{
			return mpGameObject;
		}

		friend class GameObject;
		friend class WorldSystem;

	private:
		struct SRT
		{
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 position;

			SRT() : scale(1, 1, 1)
			{
			}

			SRT(const glm::vec3 &rScale, const glm::quat &rRotation, const glm::vec3 &rPosition) : scale(rScale),
																								   rotation(rRotation),
																								   position(rPosition)
			{
			}

			SRT operator*(const SRT &rOther) const
			{
				return SRT(scale * rOther.scale, rotation * rOther.rotation, position + rOther.position);
			}

			SRT &operator=(const SRT &rOther)
			{
				scale = rOther.scale;
				rotation = rOther.rotation;
				position = rOther.position;
				return *this;
			}

			glm::mat4 ToMat4() const
			{
				glm::mat4 scaleMatrix(
					scale.x, 0, 0, 0,
					0, scale.y, 0, 0,
					0, 0, scale.z, 0,
					0, 0, 0, 1);

				glm::mat4 rotationMatrix = glm::toMat4(rotation);

				glm::mat4 translateMatrix(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					position.x, position.y, position.z, 1);

				return translateMatrix * rotationMatrix * scaleMatrix;
			}
		};

		GameObject *mpGameObject;
		Transform *mpParent{nullptr};
		std::vector<Transform *> mChildren;
		SRT mLocalTransform;
		SRT mWorldTransform;

		Transform(GameObject *pGameObject, const glm::vec3 &rScale = glm::vec3{1, 1, 1}, const glm::quat &rRotation = {}, const glm::vec3 &rPosition = glm::vec3{0, 0, 0}) : mpGameObject(pGameObject),
																																											 mLocalTransform(rScale, rRotation, rPosition)
		{
		}

		void Update()
		{
			if (mpParent != nullptr)
			{
				mWorldTransform = mpParent->mWorldTransform * mLocalTransform;
			}
			else
			{
				mWorldTransform = mLocalTransform;
			}

			for (auto *pChild : mChildren)
			{
				pChild->Update();
			}
		}

		inline void AddChild(Transform *pChild)
		{
			pChild->mpGameObject->SetActive(mpGameObject->IsActive()); // if parent is inactive, set as inactive as well
			mChildren.emplace_back(pChild);
		}
	};

}

#endif
