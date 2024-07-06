#pragma once
#include "Object.h"
#include "GameObjects/Portal.h"
#include "GameObjects/Player.h"

namespace nori {
    class Scene {
    public:
        virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player) = 0;
        virtual void Unload() {};
    };
};