#pragma once
#include "Engine.h"    
#include <iostream>
extern nori::Engine* nori::makeEngine();
int main(int argc, char** argv) {
    //Open console in debug mode

    //Run the main engine
    auto engine = nori::makeEngine();
    engine->Run();
    delete engine;
}
