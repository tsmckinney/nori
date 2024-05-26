#include "Engine.h"
#include "Physical.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
#include "Level5.h"
#include "Level6.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <algorithm>

Engine* GH_ENGINE = nullptr;
Player* GH_PLAYER = nullptr;
const Input* GH_INPUT = nullptr;
int GH_REC_LEVEL = 0;
int64_t GH_FRAME = 0;


Engine::Engine() : window(NULL), context(NULL) {
  GH_ENGINE = this;
  GH_INPUT = &input;
  isFullscreen = false;

  CreateGLWindow();
  InitGLObjects();

  player.reset(new Player);
  GH_PLAYER = player.get();

  vScenes.push_back(std::shared_ptr<Scene>(new Level1));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level3));
  vScenes.push_back(std::shared_ptr<Scene>(new Level4));
  vScenes.push_back(std::shared_ptr<Scene>(new Level5));
  vScenes.push_back(std::shared_ptr<Scene>(new Level6));

  LoadScene(0);

  sky.reset(new Sky);
}

Engine::~Engine() {
  SDL_GL_MakeCurrent(NULL, NULL);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int Engine::Run() {
  if (!window || !context) {
    return 1;
  }



  //Setup the timer
  const int64_t ticks_per_step = timer.SecondsToTicks(GH_DT);
  int64_t cur_ticks = timer.GetTicks();
  GH_FRAME = 0;

  //Game loop
  SDL_Event event;
  while (true) {
      while (SDL_PollEvent(&event))
      {
          if (event.type == SDL_EVENT_QUIT) {
              DestroyGLObjects();
              return 0;
          }
          else if (event.type == SDL_EVENT_KEY_DOWN)
          {
              if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z')
              {
                  input.key_press[toupper(event.key.keysym.sym)] = true;
                  input.key[toupper(event.key.keysym.sym)] = true;
              }
              else if (event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9')
              {
                  input.key_press[event.key.keysym.sym] = true;
                  input.key[event.key.keysym.sym] = true;
              }
              else if (event.key.keysym.sym == SDLK_ESCAPE)
              {
                  DestroyGLObjects();
                  return 0;
              }
              else if (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & SDL_KMOD_LALT)) {
                  ToggleFullscreen();
              }
          }
          else if (event.type == SDL_EVENT_KEY_UP) {
              if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z')
              {
                  input.key[toupper(event.key.keysym.sym)] = false;
              }
              else if (event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9')
              {
                  input.key[event.key.keysym.sym] = false;
              }
          }
          else if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
              input.UpdateRaw(event.button.button, event.motion.xrel, event.motion.yrel);
          }
      }
    //Confine the cursor

    if (input.key_press['1']) {
        LoadScene(0);
    }
    else if (input.key_press['2']) {
        LoadScene(1);
    }
    else if (input.key_press['3']) {
        LoadScene(2);
    }
    else if (input.key_press['4']) {
        LoadScene(3);
    }
    else if (input.key_press['5']) {
        LoadScene(4);
    }
    else if (input.key_press['6']) {
        LoadScene(5);
    }
    else if (input.key_press['7']) {
        LoadScene(6);
    }

    //Used fixed time steps for updates
    const int64_t new_ticks = timer.GetTicks();
    for (int i = 0; cur_ticks < new_ticks && i < GH_MAX_STEPS; ++i) {
        Update();
        cur_ticks += ticks_per_step;
        GH_FRAME += 1;
        input.EndFrame();
    }
    cur_ticks = (cur_ticks < new_ticks ? new_ticks : cur_ticks);

    //Setup camera for rendering
    const float n = GH_CLAMP(NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
    main_cam.worldView = player->WorldToCam();
    main_cam.SetSize(iWidth, iHeight, n, GH_FAR);
    main_cam.UseViewport();

    //Render scene
    GH_REC_LEVEL = GH_MAX_RECURSION;
    Render(main_cam, 0, nullptr);
    SDL_GL_SwapWindow(window);
  }

  DestroyGLObjects();
  return 0;
}

void Engine::LoadScene(int ix) {
  //Clear out old scene
  if (curScene) { curScene->Unload(); }
  vObjects.clear();
  vPortals.clear();
  player->Reset();

  //Create new scene
  curScene = vScenes[ix];
  curScene->Load(vObjects, vPortals, *player);
  vObjects.push_back(player);
}

void Engine::Update() {
  //Update
  for (size_t i = 0; i < vObjects.size(); ++i) {
    assert(vObjects[i].get());
    vObjects[i]->Update();
  }

  //Collisions
  //For each physics object
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (!physical) { continue; }
    Matrix4 worldToLocal = physical->WorldToLocal();

    //For each object to collide with
    for (size_t j = 0; j < vObjects.size(); ++j) {
      if (i == j) { continue; }
      Object& obj = *vObjects[j];
      if (!obj.mesh) { continue; }

      //For each hit sphere
      for (size_t s = 0; s < physical->hitSpheres.size(); ++s) {
        //Brings point from collider's local coordinates to hits's local coordinates.
        const Sphere& sphere = physical->hitSpheres[s];
        Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
        Matrix4 localToUnit = worldToUnit * obj.LocalToWorld();
        Matrix4 unitToWorld = worldToUnit.Inverse();

        //For each collider
        for (size_t c = 0; c < obj.mesh->colliders.size(); ++c) {
          Vector3 push;
          const Collider& collider = obj.mesh->colliders[c];
          if (collider.Collide(localToUnit, push)) {
            //If push is too small, just ignore
            push = unitToWorld.MulDirection(push);
            vObjects[j]->OnHit(*physical, push);
            physical->OnCollide(*vObjects[j], push);

            worldToLocal = physical->WorldToLocal();
            worldToUnit = sphere.LocalToUnit() * worldToLocal;
            localToUnit = worldToUnit * obj.LocalToWorld();
            unitToWorld = worldToUnit.Inverse();
          }
        }
      }
    }
  }

  //Portals
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (physical) {
      for (size_t j = 0; j < vPortals.size(); ++j) {
        if (physical->TryPortal(*vPortals[j])) {
          break;
        }
      }
    }
  }
}

void Engine::Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal) {
  //Clear buffers
  if (GH_USE_SKY) {
    glClear(GL_DEPTH_BUFFER_BIT);
    sky->Draw(cam);
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  //Create queries (if applicable)
  GLuint queries[GH_MAX_PORTALS];
  GLuint drawTest[GH_MAX_PORTALS];
  assert(vPortals.size() <= GH_MAX_PORTALS);
  if (occlusionCullingSupported) {
    glGenQueriesARB((GLsizei)vPortals.size(), queries);
  }

  //Draw scene
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->Draw(cam, curFBO);
  }

  //Draw portals if possible
  if (GH_REC_LEVEL > 0) {
    //Draw portals
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
          vPortals[i]->DrawPink(cam);
          glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
      }
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &drawTest[i]);
        }
      };
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glDeleteQueriesARB((GLsizei)vPortals.size(), queries);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i].get() != skipPortal) {
        if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0)) {
          continue;
        } else {
          vPortals[i]->Draw(cam, curFBO);
        }
      }
    }
    GH_REC_LEVEL += 1;
  }
  
#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

void Engine::CreateGLWindow() {
    SDL_Init(SDL_INIT_EVENTS && SDL_INIT_AUDIO && SDL_INIT_CAMERA && SDL_INIT_GAMEPAD && SDL_INIT_HAPTIC && SDL_INIT_JOYSTICK && SDL_INIT_SENSOR && SDL_INIT_TIMER && SDL_INIT_VIDEO);

  //Always start in windowed mode
  iWidth = GH_SCREEN_WIDTH;
  iHeight = GH_SCREEN_HEIGHT;

  //Create the window

  window = SDL_CreateWindow(GH_TITLE, iWidth, iHeight, SDL_WINDOW_OPENGL);
  SDL_GL_LoadLibrary(NULL);


  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, context);

  if (GH_START_FULLSCREEN) {
    ToggleFullscreen();
  }
  if (GH_HIDE_MOUSE) {
      ConfineCursor();
  }
}

void Engine::InitGLObjects() {
  //Initialize extensions
  gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  glViewport(0, 0, w, h);

  //Basic global variables
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  //Check GL functionality
  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &occlusionCullingSupported);

  //Attempt to enalbe vsync (if failure then oh well)
  SDL_GL_SetSwapInterval(1);
}

void Engine::DestroyGLObjects() {
  curScene->Unload();
  vObjects.clear();
  vPortals.clear();
}


float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for (size_t i = 0; i < vPortals.size(); ++i) {
    dist = GH_MIN(dist, vPortals[i]->DistTo(player->pos));
  }
  return dist;
}

void Engine::ConfineCursor()
{
    SDL_SetRelativeMouseMode(true);
    SDL_SetWindowMouseGrab(window, true);
}

void Engine::ToggleFullscreen() {
  isFullscreen = !isFullscreen;
  if (isFullscreen) {
      SDL_SetWindowFullscreen(window, true);
      SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
      const SDL_DisplayMode* display_mode = SDL_GetDesktopDisplayMode(display_id);
      iWidth = display_mode->w;
      iHeight = display_mode->h;
      SDL_SetWindowSize(window, iWidth, iHeight);
      glViewport(0, 0, iWidth, iHeight);

  } else {
      SDL_SetWindowFullscreen(window, false);
      iWidth = GH_SCREEN_WIDTH;
      iHeight = GH_SCREEN_HEIGHT;
      SDL_SetWindowSize(window, iWidth, iHeight);
      glViewport(0, 0, iWidth, iHeight);
  }
}
