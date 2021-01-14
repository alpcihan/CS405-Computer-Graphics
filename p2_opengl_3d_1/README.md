OpenGL 3D Part 1
=================================
Multiple 3D scenes from scratch that cover various basic concepts of OpenGL and 3D rendering in general.

## Table of Content
* [Scenes](#scenes)
* [Key Control](#key-control)
* [Dependencies](#dependencies)
* [License](#license)

## Key control
User can switch between the scenes by using the keyboard.

* **Q:** Scene 1
* **W:** Scene 2
* **E:** Scene 3
* **R:** Scene 4
* **T:** Scene 5
* **Y:** Scene 6
* **Z:** Switches between scene 6.1 and scene 6.2 at the scene 6
* **X:** Enable/Disaple noise height generation at scene 6.2


## Scenes



### Scene 1

![scene1](https://user-images.githubusercontent.com/37274614/104497805-f457c380-55eb-11eb-9c4d-84771b6668ef.gif)
### Scene 2
![scene2](https://user-images.githubusercontent.com/37274614/104498845-49e0a000-55ed-11eb-91d0-ea9c78044663.gif)
### Scene 3
![scene3](https://user-images.githubusercontent.com/37274614/104498952-6e3c7c80-55ed-11eb-8fab-1536eb87f3c3.gif)
### Scene 4
![scene4](https://user-images.githubusercontent.com/37274614/104499067-96c47680-55ed-11eb-89b0-a1010654043d.gif)
### Scene 5
![scene5](https://user-images.githubusercontent.com/37274614/104499181-be1b4380-55ed-11eb-97b8-44f2c3589441.gif)
### Scene 6 (Bonus)
#### Scene 6.1
![scene6_1](https://user-images.githubusercontent.com/37274614/104499266-e7d46a80-55ed-11eb-8fb7-e0c923388e10.gif)
#### Scene 6.2
- Used Perlin Noise from [this link](https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83#file-glsl-noise-md) for the height manipulation in the vertex shader.

![scene_6_2](https://user-images.githubusercontent.com/37274614/104499422-236f3480-55ee-11eb-92df-1e8115cdd0f6.gif)

## Dependencies
* [KHR](https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h)
* [GLEW](http://glew.sourceforge.net/)
* [GLFW](https://github.com/glfw/glfw)
* [glm](https://github.com/g-truc/glm)

## LICENSE
[MIT](../LICENSE)
