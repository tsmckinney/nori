#pragma once
#include "NoriMain.h"
using namespace nori;
class Level2 : public Scene {
public:
  Level2(int rooms) : num_rooms(rooms) {}

  virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player) override;

private:
  int num_rooms;
};
