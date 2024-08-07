#pragma once
#include "GameHeader.h"
#include "Camera.h"
#include "Input.h"
#include "Object.h"
#include "GameObjects/Portal.h"
#include "GameObjects/Player.h"
#include "Timer.h"
#include "Scene.h"
#include "Sky.h"
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
namespace nori {
    class Engine {
    public:
        Engine();
        ~Engine();

        int Run();
        void Update();
        void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
        void LoadScene(int ix);

        const Player& GetPlayer() const { return *player; }
        float NearestPortalDist() const;

    protected:
        void CreateGLWindow();
        void InitGLObjects();
        void DestroyGLObjects();
        void SetupInputs();
        void ConfineCursor();
        void ToggleFullscreen();

        SDL_Window* window;
        SDL_GLContext context;

        int iWidth;         // window width
        int iHeight;        // window height
        bool isFullscreen;   // fullscreen state

        Camera main_cam;
        Input input;
        Timer timer;

        std::vector<std::shared_ptr<Object>> vObjects;
        std::vector<std::shared_ptr<Portal>> vPortals;
        std::shared_ptr<Sky> sky;
        std::shared_ptr<Player> player;

        GLint occlusionCullingSupported;

        std::vector<std::shared_ptr<Scene>> vScenes;
        std::shared_ptr<Scene> curScene;
    };
    Engine* makeEngine();
};