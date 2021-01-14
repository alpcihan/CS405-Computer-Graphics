CPU Ray Tracer
=================================
Reimplemented [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) and [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html) using [glm](https://glm.g-truc.net/0.9.8/index.html).

Additionally, experimented with various [supersampling](https://en.wikipedia.org/wiki/Supersampling) methods. Also, created a complex box class (it is basically a box with plus (+) shape removed from its center).

## Table of content
* [Scenes](#scenes)
* [How to run](#how-to-run)

## Scenes

### Scene 1

<p align="center">
<img width="638" alt="task1_1_PNG" src="https://user-images.githubusercontent.com/37274614/104493231-d12a1580-55e5-11eb-99e9-c71b2b0c0177.png">
</p>

### Scene 2

* **Without AA**
<p align="center">
<img width="639" alt="task2_without_aa_PNG" src="https://user-images.githubusercontent.com/37274614/104493556-3da51480-55e6-11eb-8745-a713698aff6b.png">
</p>

* **Random Supersampling Algorithm**
<p align="center">
<img width="640" alt="task2_aa_random_zoom" src="https://user-images.githubusercontent.com/37274614/104493612-557c9880-55e6-11eb-8a06-f517bc8bc21b.png">
</p>

* **Grid Supersampling Algorithm**
<p align="center">
<img width="636" alt="task2_aa_grid_zoom" src="https://user-images.githubusercontent.com/37274614/104493672-6f1de000-55e6-11eb-8b15-bd8025c4b8e8.png">
</p>

### Scene 3

<p align="center">
<img width="632" alt="task4_PNG" src="https://user-images.githubusercontent.com/37274614/104493789-9c6a8e00-55e6-11eb-9083-59123300e57d.png">
</p>

### Scene 4

<p align="center">
<img width="633" alt="task5_1PNG" src="https://user-images.githubusercontent.com/37274614/104493876-c0c66a80-55e6-11eb-8385-c1d18e1b240c.png">
</p>

### Scene 5

<p align="center">
<img width="636" alt="task6_1PNG" src="https://user-images.githubusercontent.com/37274614/104493973-e0f62980-55e6-11eb-961c-44bf9445258a.png">
</p>

### Scene 6 (Bonus)

- Created a new 3d shape class (./src/box_complex.h) and experimented with its rotations.

<p align="center">
<img width="1918" alt="task3_task7_PNG" src="https://user-images.githubusercontent.com/37274614/104494060-008d5200-55e7-11eb-9641-3987bd02cbca.png">
</p>

## How to run
1) Install [glm](https://glm.g-truc.net/0.9.8/index.html)
2) Run main.cpp
