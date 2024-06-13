#pragma once
#include <SDL3/SDL.h>


class Timer {
public:
  Timer() {
    frequency = SDL_GetPerformanceFrequency();
  }

  void Start() {
      t1=SDL_GetPerformanceCounter();
  }

  float Stop() {
      t2=SDL_GetPerformanceCounter();
    return float(t2 - t1) / frequency;
  }

  int64_t GetTicks() {
      t2=SDL_GetPerformanceCounter();
    return t2;
  }

  int64_t SecondsToTicks(float s) {
    return int64_t(float(frequency) * s);
  }

  float StopStart() {
    const float result = Stop();
    t1 = t2;
    return result;
  }

private:
  Uint64 frequency;        // ticks per second
  Uint64 t1, t2;           // ticks
};
