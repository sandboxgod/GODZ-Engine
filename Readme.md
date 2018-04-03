This project is sample code from GODZ Engine. Note a lot of this code dates all the way back to 2010 and earlier. 

There's tons of things I'd do differently.
* Use Git for Source Control rather than Perforce. So we are mising the original history.
* Probably wouldn't use MACROS to bake the dynamic type information. Instead I'd go with a stronger Composition
* Cleaner code in some places.

Do not expect this project to compile as-is right now. This repo is intended to show off some of the old stuff I've worked on.

Projects in this repo:
* RyotianEd - is a C# editor that hooks into a SQL Database where it stores Map information. The idea is to allow Designers to collaborate in realtime on Levels.
* BigLevel - is an example Level where we spawn entities, a camera, sun, etc
* CoreFramework - Base engine files
* LUA - This is the popular LUA scripting language
* Shaders - some HLSL shaders I wrote to handle Deferred Rendering, SkyDome, GPU Skinned Meshes, etc.
* Bullet - it's a lib I grabbed from the Web for Collision detection.
