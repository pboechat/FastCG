#ifndef FASTCG_TRANSFORM_H
#define FASTCG_TRANSFORM_H

#include <FastCG/World/Component.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <cassert>
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
            if (mpParent != nullptr)
            {
                mpParent->RemoveChild(this);
            }
            mpParent = pParent;
            if (mpParent != nullptr)
            {
                mpParent->AddChild(this);
            }
            mNeedUpdate = true;

            Update();
        }

        inline const std::vector<Transform *> &GetChildren() const
        {
            return mChildren;
        }

        inline const glm::vec3 &GetWorldPosition() const
        {
            return mWorldTransform.position;
        }

        inline const glm::vec3 &GetPosition() const
        {
            return mLocalTransform.position;
        }

        inline void SetPosition(const glm::vec3 &position)
        {
            mLocalTransform.position = position;
            mNeedUpdate = true;

            Update();
        }

        inline const glm::quat &GetWorldRotation() const
        {
            return mWorldTransform.rotation;
        }

        inline const glm::quat &GetRotation() const
        {
            return mLocalTransform.rotation;
        }

        inline void SetRotation(const glm::quat &rRotation)
        {
            mLocalTransform.rotation = rRotation;
            mNeedUpdate = true;

            Update();
        }

        inline const glm::vec3 &GetWorldScale() const
        {
            return mWorldTransform.scale;
        }

        inline const glm::vec3 &GetScale() const
        {
            return mLocalTransform.scale;
        }

        inline void SetScale(const glm::vec3 &rScale)
        {
            mLocalTransform.scale = rScale;
            mNeedUpdate = true;

            Update();
        }

        inline void Rotate(const glm::vec3 &rEulerAngles)
        {
            mLocalTransform.rotation = mLocalTransform.rotation * glm::quat(glm::radians(rEulerAngles));
            mNeedUpdate = true;

            Update();
        }

        inline void RotateAround(float angle, const glm::vec3 &rAxis)
        {
            mLocalTransform.rotation = glm::rotate(mLocalTransform.rotation, glm::radians(angle), rAxis);
            mNeedUpdate = true;

            Update();
        }

        inline void RotateAroundLocal(float angle, const glm::vec3 &rAxis)
        {
            auto newLocal =
                glm::rotate(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), glm::radians(angle), rAxis);
            newLocal = glm::translate(newLocal, mLocalTransform.position);
            mLocalTransform.position = glm::vec3(newLocal[3][0], newLocal[3][1], newLocal[3][2]);
            mLocalTransform.rotation = glm::quat(newLocal);
            mNeedUpdate = true;

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

        inline void SetModel(const glm::mat4 &rModel)
        {
            mLocalTransform.position = rModel[3];
            mLocalTransform.scale = {glm::length(glm::vec3(rModel[0])), glm::length(glm::vec3(rModel[1])),
                                     glm::length(glm::vec3(rModel[2]))};
            glm::mat4 normModel = rModel;
            normModel[0] /= mLocalTransform.scale.x;
            normModel[1] /= mLocalTransform.scale.y;
            normModel[2] /= mLocalTransform.scale.z;
            mLocalTransform.rotation = glm::quat_cast(normModel);
            mNeedUpdate = true;

            Update();
        }

        inline GameObject *GetGameObject()
        {
            return mpGameObject;
        }

        inline const GameObject *GetGameObject() const
        {
            return mpGameObject;
        }

        inline bool HasUpdated() const
        {
            return mHasUpdated;
        }

        friend class GameObject;
        friend class WorldSystem;

    private:
        struct SRT
        {
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 position;

            SRT() : scale(1, 1, 1), rotation(1, 0, 0, 0), position(0, 0, 0)
            {
            }

            SRT(const glm::vec3 &rScale, const glm::quat &rRotation, const glm::vec3 &rPosition)
                : scale(rScale), rotation(rRotation), position(rPosition)
            {
            }

            SRT operator*(const SRT &rOther) const
            {
                return SRT(scale * rOther.scale, rotation * rOther.rotation,
                           position + rotation * (rOther.position * scale));
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
                glm::mat4 scaleMatrix(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);

                glm::mat4 rotationMatrix = glm::toMat4(rotation);

                glm::mat4 translateMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, position.x, position.y, position.z, 1);

                return translateMatrix * rotationMatrix * scaleMatrix;
            }
        };

        GameObject *mpGameObject;
        Transform *mpParent{nullptr};
        std::vector<Transform *> mChildren;
        SRT mLocalTransform;
        SRT mWorldTransform;
        bool mNeedUpdate{false};
        bool mHasUpdated{false};

        Transform(GameObject *pGameObject, const glm::vec3 &rScale = glm::vec3{1, 1, 1},
                  const glm::quat &rRotation = glm::quat{1, 0, 0, 0}, const glm::vec3 &rPosition = glm::vec3{0, 0, 0})
            : mpGameObject(pGameObject), mLocalTransform(rScale, rRotation, rPosition),
              mWorldTransform(rScale, rRotation, rPosition)
        {
        }

        void Update(bool forceUpdate = false)
        {
            if (mHasUpdated)
            {
                mHasUpdated = false;
            }

            if (mNeedUpdate || forceUpdate)
            {
                if (mpParent != nullptr)
                {
                    mWorldTransform = mpParent->mWorldTransform * mLocalTransform;
                }
                else
                {
                    mWorldTransform = mLocalTransform;
                }
                mNeedUpdate = false;
                mHasUpdated = true;
            }

            for (auto *pChild : mChildren)
            {
                pChild->Update(mHasUpdated);
            }
        }

        inline void AddChild(Transform *pChild)
        {
            assert(pChild);
            pChild->GetGameObject()->SetActive(
                mpGameObject->IsActive()); // if parent is inactive, set as inactive as well
            mChildren.emplace_back(pChild);
        }

        inline void RemoveChild(Transform *pChild)
        {
            assert(pChild);
            auto it = std::find(mChildren.begin(), mChildren.end(), pChild);
            assert(it != mChildren.end());
            mChildren.erase(it);
        }
    };

}

#endif
