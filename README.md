# ChanGameEngine

This is the my personal game engine, which is a kind of experiment lab for me. I've started this project as the game engine project, but it's just becoming my experiment. However, I still have the goal to make my own game engine. 

the program structure and the build system of this project is not good because I didn't know anything about those topics when I started this project. But I've experienced the game engine development in the company. Now, I am ready to change this project. Before doing that, I can show what I did with this project.



## 2018.08 ~ 2019.01

I started this project for my portfolio to get a job as game engine / graphics programmer. I studied the math, OpenGL, game physics and so on to make my own game engine. During this period, I implemented the basic graphics and physics parts of the game engine. This is the final scene I made with my engine :

![first_result](img/firstResult.png)

Here is the editing gif of the scene :

![firstResult_gif](img/firstResult.gif)

you can see the video of this [here](https://youtu.be/KSpS1TO2YgM).



While I was doing this project, I implemented these list:

**Graphics**

* The graphics engine is based on the deferred shading. All of objects in game world will be drawn by the technique.
* PostProcessing : HDR, Bloom, SSAO.
* Terrain Rendering with Perlin noise and height map (you can see the terrain rendering and physics [here](https://www.youtube.com/watch?v=gEwG9HVjTYk))

**Light** (you can see the light editing video [here]([Implemented Shadow of Dir, Point, Spot Light - YouTube](https://www.youtube.com/watch?v=0eW7ttd--IM)))

* The types of light : Directional, Point, and Spot light.

* Editing light : Position, Direction, Ambient/Diffuse/Specular Color, Attenunation factors, light radius.
* Range Visualizer for each light type
* shadow cast for each light type
* editing the shadow frustum for each light type

**Physics**

* Most of physics theories and implementations are based on the "Game Physics Engine Development" Book.
* And other codes such as broadphase, dynamicAABBtree, rayPick are based on Box2D and BulletPhysics Library.
* Terrain Physics between terrain triangles and spheres.

