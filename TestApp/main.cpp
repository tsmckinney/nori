#include "Nori.h"
#include "Scenes/Level1.h"
#include "Scenes/Level2.h"
#include "Scenes/Level3.h"
#include "Scenes/Level4.h"
#include "Scenes/Level5.h"
#include "Scenes/Level6.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>

class EngineTest : public nori::Engine
{
public:
	EngineTest();
	~EngineTest();

private:

};

EngineTest::EngineTest()
{
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level1));
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level2(3)));
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level2(8)));
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level3));
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level4));
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level5));
	vScenes.push_back(std::shared_ptr<nori::Scene>(new Level6));

	LoadScene(0);
}

EngineTest::~EngineTest()
{
}

nori::Engine* nori::makeEngine() {
	return new EngineTest();
}