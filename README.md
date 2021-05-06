# ChanGameEngine

This is my personal game engine, which is a kind of experiment lab for me. I've started this project as the game engine project, but it's just becoming my experiment. However, I still have the goal to make my own game engine. 

the program structure and the build system of this project is not good because I didn't know anything about those topics when I started this project. But I've experienced the game engine development in the company. Now, I am ready to change this project. Before doing that, I can show what I did with this project.



## 2018.08 ~ 2019.01

I started this project for my portfolio to get a job as game engine / graphics programmer. I studied the math, OpenGL, game physics and so on to make my own game engine. During this period, I implemented the basic graphics and physics parts of the game engine. This is the final scene I made with my engine :

![first_result](img/firstResult.png)

Here is the editing gif of the scene :

![firstResult_gif](img/firstResult.gif)

you can see the video of this [here](https://youtu.be/KSpS1TO2YgM).



While I was doing this project, I implemented these things:

**Graphics**

* The graphics engine is based on the deferred shading. All of objects in game world will be drawn by the technique.
* PostProcessing : HDR, Bloom, SSAO.
* Terrain Rendering with Perlin noise and height map (you can see the terrain rendering and physics [here](https://www.youtube.com/watch?v=gEwG9HVjTYk))

**Light** (you can see the video editing lights [here]([Implemented Shadow of Dir, Point, Spot Light - YouTube](https://www.youtube.com/watch?v=0eW7ttd--IM)))

* The types of light : Directional, Point, and Spot light.

* Light Interactions : Position, Direction, Ambient/Diffuse/Specular Color, Attenuation factors, light radius.
* Range Visualizer for each light type
* Shadow cast for each light type
* Editing the shadow frustum for each light type

**Physics**

* Most of physics theories and implementations are based on the "Game Physics Engine Development" Book.
* And other codes such as Broad Phase, DynamicAABBtree, Ray Picking are based on Box2D and BulletPhysics Library.
* Terrain Physics between terrain triangles and spheres.



## 2020.12 ~ 2021.01

I had stopped this project after I got the job as a game engine programmer. But I have an interest in the high quality rendering, especially using ray tracing. So, I got enough time at this period to study a little of the ray tracing. This is the simple scene I made with the ray tracing :

![ray_trace_simple_trial](img/raytracing_try.png)

I Implemented these for this ray-traced scene :

* **Rendering Equation** : I'm using [the Graphics Codex](http://graphicscodex.com/index.php) to learn the rendering equation. And I also refer to the sample code provided with the Graphics Codex in [G3D Innovation Engine](https://casual-effects.com/g3d/www/index.html).
* **Primitive Intersection** : Sphere, Plane, Ray, Line Segment, Triangle. I referred to the [RTCD](https://realtimecollisiondetection.net/) to implement all of the intersections.
* **Bounding Volume Hierarchy** : In order to accelerate the ray intersection, I used the Dynamic AABB Tree, which I implemented by referring to the [box2D](https://github.com/erincatto/box2d) code.

In addition to the above, I used the threads to accelerate the ray tracing by separating the pixels with some threads.

Because I'm using the Graphics Codex to improve my graphics ability, I am going to study BSDF, Path Tracing, Photon Mapping, and so on. Before going deeper on this, I realized that I don't have enough base knowledge on statistics. So I've decided to study it. After that, I can restart this ray tracing work.

