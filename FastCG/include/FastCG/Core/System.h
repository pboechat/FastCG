#ifndef FASTCG_SYSTEM_H
#define FASTCG_SYSTEM_H

namespace FastCG
{
    class BaseApplication;

}

#define FASTCG_DECLARE_SYSTEM(className, argsClassName)                                                                \
public:                                                                                                                \
    static className *GetInstance();                                                                                   \
                                                                                                                       \
private:                                                                                                               \
    static void Create(const argsClassName &rArgs);                                                                    \
    static void Destroy();                                                                                             \
    friend class FastCG::BaseApplication

#define FASTCG_IMPLEMENT_SYSTEM(className, argsClassName)                                                              \
    static className *sp##className = nullptr;                                                                         \
    void className::Create(const argsClassName &rArgs)                                                                 \
    {                                                                                                                  \
        sp##className = new className(rArgs);                                                                          \
    }                                                                                                                  \
    void className::Destroy()                                                                                          \
    {                                                                                                                  \
        delete sp##className;                                                                                          \
    }                                                                                                                  \
    className *className::GetInstance()                                                                                \
    {                                                                                                                  \
        return sp##className;                                                                                          \
    }

#endif