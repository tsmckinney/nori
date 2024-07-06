# Nori
A NonEuclidean game engine written in C++ OpenGL.
To see what the original project was about, check out this video:
https://youtu.be/kEB11PQ9Eo8

## Building

### Source Code Dependencies
Add SDL3, glad2(make sure you use the Core profile, otherwise it may not work!), and imgui (docking branch, latest version), to the main directory, naming them approporately.  Once you build the project, add SDL3.dll to the build.

## Controls
* **Mouse** - Look around
* **AWSD** - Movement
* **1 - 7** - Switch between different demo rooms
* **Alt + Enter** - Toggle Fullscreen
* **Esc** - Exit demo

## Important note:
There is a bug where on some systems, if you use a mouse button when ImGui isn't active, the camera starts spinning. I'm trying to fix this up, but in the meantime, resetting the scene may fix the problem.

## Thanks
- To CodeParade for starting this project
- To Srinivasa314 and CzechBlueBear for coming up with the idea of porting the project to SDL
- To several OpenGL tutorials for helping me port this project to modern OpenGL
- To nothings for stb_image
- To TheCherno for an architectual guide