OpenGL 3D Part 2
=================================
3D mini-game with a 3rd person controller on a spherical world.

The aim is to escape from the two agents that are chasing the player.

![mars](https://user-images.githubusercontent.com/37274614/104811200-14190280-580b-11eb-9d71-63f3cac8dd09.gif)

## Table of content
* [Controls](#controls)
* [How to implement spherical movement](#how-to-achieve-spherical-movement)
* [Dependencies](#dependencies)
* [License](#license)

## Spherical movement
* Store up and the front vectors of the player. (Also, update them accordingly when the player moves.)
* Use those vectors to find the equation of the circle that passes through the current position of the player and have (0,0,0) center point. Here is a [guide](https://www.quora.com/A-problem-in-3D-geometry-what-is-the-equation-of-the-circle-see-details) for that.
* Automatically the circle has got the same center point and same radius as Mars.
* To move forward/backward, just increase/decrease the angle in the equation and calculate the new coordinates, up and the front vectors.
* To rotate the player, rotate the forward vector and calculate the new circle accordingly.
* See main.cpp -> void CircularM(float) for the implementation.

## Controls
**Player control:** WASD

**Camera control:** Mouse
- **X Key:** Switch between the 3rd person camera and the independent camera control
- **Independent camera mode**
  - **W & S:** forward/backward
  - **A & D:** right/left
  - **R & F:** up / down

## Dependencies
* [KHR](https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h)
* [GLEW](http://glew.sourceforge.net/)
* [GLFW](https://github.com/glfw/glfw)
* [glm](https://github.com/g-truc/glm)

## LICENSE
[MIT](../LICENSE)

