#ifndef FASTCG_CONSTANT_BUFFER_H
#define FASTCG_CONSTANT_BUFFER_H

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>
#include <initializer_list>
#include <cstring>
#include <cstdint>
#include <cassert>

namespace FastCG
{
    class ConstantBuffer
    {
    public:
        class Member
        {
        public:
            Member(const std::string &rName, float value) : mName(rName),
                                                            mType(Type::FLOAT),
                                                            mDefaultValue_float(value)
            {
            }

            Member(const std::string &rName, const glm::vec2 &rValue) : mName(rName),
                                                                        mType(Type::VEC2),
                                                                        mDefaultValue_vec2(rValue)
            {
            }

            Member(const std::string &rName, const glm::vec4 &rValue) : mName(rName),
                                                                        mType(Type::VEC4),
                                                                        mDefaultValue_vec4(rValue)
            {
            }

            Member(const Member &rOther) : mName(rOther.mName),
                                           mType(rOther.mType),
                                           mDefaultValue_vec4(rOther.mDefaultValue_vec4) // largest union member
            {
            }

            inline const auto &GetName() const
            {
                return mName;
            }

            inline bool IsFloat() const
            {
                return mType == Type::FLOAT;
            }

            inline bool IsVec2() const
            {
                return mType == Type::VEC2;
            }

            inline bool IsVec4() const
            {
                return mType == Type::VEC4;
            }

        private:
            enum class Type : uint8_t
            {
                FLOAT,
                VEC2,
                VEC4
            };

            std::string mName;
            Type mType;
            union
            {
                int32_t mDefaultValue_int;
                float mDefaultValue_float;
                glm::vec2 mDefaultValue_vec2;
                glm::vec4 mDefaultValue_vec4;
            };

            inline const auto *GetValue() const
            {
                return (const void *)&mDefaultValue_int;
            }

            inline uint32_t GetSize() const
            {
                // FIXME: Using glsl type sizes
                switch (mType)
                {
                case Type::FLOAT:
                    return 4;
                case Type::VEC2:
                    return 8;
                case Type::VEC4:
                    return 16;
                default:
                    assert(false);
                }
                return 0;
            }

            friend class ConstantBuffer;
        };

        ConstantBuffer(const std::vector<Member> &rMembers) : mMembers(rMembers)
        {
            Initialize();
        }

        ConstantBuffer(const std::initializer_list<Member> &rMembers) : mMembers(rMembers)
        {
            Initialize();
        }

        ConstantBuffer(const ConstantBuffer &rOther) : mAlignment(rOther.mAlignment),
                                                       mMembers(rOther.mMembers),
                                                       mOffsets(rOther.mOffsets),
                                                       mSize(rOther.mSize)
        {
            mData = std::make_unique<uint8_t[]>(mSize);
            memcpy((void *)&mData[0], rOther.GetData(), mSize);
        }

        inline const auto &GetMembers() const
        {
            return mMembers;
        }

        inline bool GetMemberValue(const std::string &rName, float &rValue) const
        {
            return GetMemberValueInternal(rName, rValue);
        }

        inline bool GetMemberValue(const std::string &rName, glm::vec2 &rValue) const
        {
            return GetMemberValueInternal(rName, rValue);
        }

        inline bool GetMemberValue(const std::string &rName, glm::vec4 &rValue) const
        {
            return GetMemberValueInternal(rName, rValue);
        }

        inline bool SetMemberValue(const std::string &rName, float value)
        {
            return SetMemberValueInternal(rName, value);
        }

        inline bool SetMemberValue(const std::string &rName, const glm::vec2 &rValue)
        {
            return SetMemberValueInternal(rName, rValue);
        }

        inline bool SetMemberValue(const std::string &rName, const glm::vec4 &rValue)
        {
            return SetMemberValueInternal(rName, rValue);
        }

        inline uint32_t GetAlignment() const
        {
            return mAlignment;
        }

        inline uint32_t GetSize() const
        {
            return mSize;
        }

        inline const uint8_t *GetData() const
        {
            return mData.get();
        }

        inline void SetData(const uint8_t *data, size_t offset, size_t size)
        {
            assert((offset + size) <= mSize);
            memcpy(mData.get() + offset, data, size);
        }

    private:
        uint32_t mAlignment{0}; // GPU memory alignment
        std::unique_ptr<uint8_t[]> mData;
        const std::vector<Member> mMembers;
        std::vector<uint32_t> mOffsets;
        uint32_t mSize{0};

        inline void Initialize()
        {
            // FIXME: Using std140 layout rules
            for (const auto &rMember : mMembers)
            {
                auto memberSize = rMember.GetSize();
                mAlignment = std::max(memberSize, mAlignment);
                auto padding = mSize % memberSize;
                auto offset = mSize + padding;
                auto boundary = (offset + memberSize) % 16;
                if (boundary > 0 && boundary < memberSize)
                {
                    offset += offset / 16;
                }
                mOffsets.emplace_back(offset);
                mSize = offset + memberSize;
            }
            mSize += mSize / 16;

            if (mSize > 0)
            {
                mData = std::make_unique<uint8_t[]>(mSize);
                for (size_t i = 0; i < mMembers.size(); ++i)
                {
                    memcpy((void *)&mData[mOffsets[i]], mMembers[i].GetValue(), mMembers[i].GetSize());
                }
            }
        }

        inline bool GetMemberOffset(const std::string &rName, uint32_t &rOffset) const
        {
            for (size_t i = 0; i < mMembers.size(); ++i)
            {
                if (mMembers[i].GetName() == rName)
                {
                    rOffset = mOffsets[i];
                    return true;
                }
            }
            assert(false);
            return false;
        }

        template <typename T>
        inline bool SetMemberValueInternal(const std::string &rName, const T &rValue)
        {
            uint32_t offset;
            if (!GetMemberOffset(rName, offset))
            {
                return false;
            }
            memcpy((void *)&mData[offset], (const void *)&rValue, sizeof(T));
            return true;
        }

        template <typename T>
        inline bool GetMemberValueInternal(const std::string &rName, T &rValue) const
        {
            uint32_t offset;
            if (!GetMemberOffset(rName, offset))
            {
                return false;
            }
            memcpy((void *)&rValue, &mData[offset], sizeof(T));
            return true;
        }
    };

}

#endif