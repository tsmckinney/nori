#pragma once
#include "NoriMain.h"
using namespace nori;
class Level4 : public Scene {
public:
  virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player) override;
};
