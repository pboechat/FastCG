#ifndef FASTCG_WORLD_LOADER_H
#define FASTCG_WORLD_LOADER_H

#include <FastCG/GameObject.h>

#include <string>

namespace FastCG
{
    class WorldLoader
    {
    public:
        static GameObject *Load(const std::string &rFilePath);
        static std::string Dump(GameObject *pWorldRoot);

    private:
        WorldLoader() = delete;
        ~WorldLoader() = delete;
    };

}

#endif