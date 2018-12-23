# ChanGameEngine

Hello. I'm Chanhaeng Lee who is studying Computer Graphics!
I'm making my own game engine, the simple one.

The features which I implemented so far are.. 

--- Updating ---

1. Graphics
- The graphics engine is based on the deferred shading. All of objects in game world will be drawn by the technique.

1) Game Object
~~

2) Light
- The types of light : Directional, Point, and Spot light.
- Editing the position of light
- Editing the direction of light
- Editing the ambient, diffuse, specular color of each light type
- Editing the Attenuation factors of each light type except for the directional light.
- Editing the radius of light for deferred shading performance
- Showing the range visualizer of each light type
- Selecting the shadow cast of each light type
- Editing the shadow frustum of each light type
- Showing the depth map of each light type for debugging

2. Physics
- Most of physics theories and implementations are based on the "Game Physics Engine Development" Book.
- And other codes such as broadphase, dynamicAABBtree, rayPick are based on Box2D and BulletPhysics Library.

3. Common
- Object Picking
