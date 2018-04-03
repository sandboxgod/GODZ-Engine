========================================================================
    DYNAMIC LINK LIBRARY : GodzEngine Project Overview
========================================================================

Started in Nov. 1, 2003-

Features:

- Uses Octree for fast culling and collision
- LUA 5.0.1 Integration for Scripting/Configuration
- Uses DirectX 9
- Advanced RTTI Support (Class Loading, Instantiation-by-Name)
- Collision Detection using AABB Trees, AABB Sweep Volumes
- Shaders (HLSL)
- Uses package-based file format for meshes and object serialization

Goals:

- Master DirectX 9.0+
- Improve knowledge of MFC programming
- Improve knowledge of Win32 programming
- Learn how to program octrees/quadtrees
- Learn how to integrate a physics system into an engine
- Program Bot AI utilizing C++ for core AI and a scripting language for special
circumstances and reactions
- Master vertex & pixel shaders
- Utilize tools such as NVIDIA's Cg Browser and ATI's RenderMonkey (okay, one of these two)
- Import data from a variety of formats (.unr)
- Gain more experience with outputing data in a FLEXIBLE, binary format
- Port the GODZ modification for UT 2003 to this new engine (just a small subset of the mod).
- Design a Scene Editor


--collision, micheal maynard
vel -= normal * normal.Dot(vel)


Developer Diary-

Dec 7, 2003- I've started the Editor for the Engine. Learned how to use CPropertySheets.
Also, fixed a bug in the core framework that prevented the engine from automatically
registering Generic Subclasses. This version adds 'ClassFlags' allowing the engine to
identify objects.

Jan 2, 2004- Added the ability for C++ classes to be dynamically loaded using text strings.
Jan 3, '04- Restructured a great part of the engine. Dropped all the file importers 
(.ASE, .X File Parser) in anticipation of going to my own file format for maps.
Started Octree Spatial Sorting System, rewrote the DX9 Driver, and added my first
scene graph system. Btw, currently Gaia was still being included even though it's not
used.
Jan 8, '04- Pulled all the DirectX9 mesh stuff into a model resource. Updated the Octree
to only divide if 3 actors exist.
Jan 24 - Added support Singleton patterns and reference counting. Completed the mesh subdivision
algorithm (will be used by octree).

Feb 7, 2004- Rewrote the WEntity class to return a 'collision primitive' that manages
all collision detection for the object. A simple collision detection system has been
added. A static octree has been added (polygon soup container). Currently looking
into generating better bounding boxes for the polygon soup meshes and improving the
first person camera. Might also look into adding a more flexible memory system.

Feb 9, '04- My contract ended with Secret Level which gives me more time for this project. Got
the first person camera (first draft) completed. Also added support for 'debug boxes' for
all the scene nodes.
Feb 14, '04 - Got multitextured static models working. Improved collisions with the AABB Tree.
Feb 19, '04 - Added support for skeletal character animation. Started tackling lighting the scene.
Feb 29, '04 - Added serialization support, physics Falling, better Win32 UI, and MoveActor() to LevelNode.
I guess it was somewhere around this time I decided to merge the editor right into the actual
game. This should enable developers & gameplay programmers to more easily debug their code.
Mar 7, '04 - Finally fixed the first person camera to be frame rate independant! Cleaned up the
core framework by removing unused classes like StaticMesh and StaticActor. Improved Win32 codebase and
fixed some serialization issues. Rewrote the engine to work with handles rather than declare pointers
to generics.
Mar 13, 04 - Added TArrays, fixed bounding box scaling issue, fully implemented property editor (using labels),
and fixed the SceneNode::Trace() routine to return the closest hit package to the ray's origin (instead
of simply returning after first hit).
Mar 23, 2004 - Implemented an AABB Sweep algorithm, full serialization support, dynamic configuration system,
packages (DLLs, EXE, and GODZ files are all referenced by a package pointer), and added some better profiling
stat gathering to the test cases.
Apr 6, 2004 - Fixed a bug whereas the View matrix was not known to the DX9 Driver. This in turn, fixed
shaders- which now have begun to work now that they know the view matrix.... Now the engine has very nice HLSL
shader pipeline implemented. Started working on a plugin for 3ds max 4/5 to export animations.
Apr 12, '04 - started working at TKO-Software. Let's see how productive I am!
Apr 21, '04 - Now the engine is truly frame rate independant (fixed a bug that screwed deltatime, this was evident
on 3.2 ghz machines). Got enums properly serializing and it's displayed within a nice combobox within the
properteries editor.
Apr 30, '04 - Started all over on the .ase file importer and got it working. Reorganized the max 
character exporter to use the Game Exporter SDK and started the Character Browser.
May 31, '04 - Completed a first pass at the custom animation system. Currently, it performs all of it's computations
in the CPU. right now the worst bug is that models are usually oriented in wrong position.
June 12, '04 - Optimized models imported from GSA. during the mesh creation process, we simply make sure all vertices
are unique. 
June 13, '04 - changed up the scene pipeline. Now the scene nodes render their contents directly. This speeds
up performance quite a bit. Also discovered I seriously need to take a closer look into optimizing static octrees.
June 19, '04 - finally fixed the exception encountered when the node tree is being drawn.
Jun 26, '04 - started working on mesh instances.
July 2, 04 - mesh instances are complete. I also ripped out the old camera code and created my own Euler based
camera from scratch.
Aug 15, 04 - skeletal meshes/animation can be serialized, character browser almost done, speed up performance
of the engine, & started an asset browser. Switched WEntities to use matrices instead of maintaining vectors for
rotation/position. Also added the WRotator type.
Sept 4, 04 - implemented an original View Frustum class, squashed all the memory leaks identified by Paul Nettle's code.
Sept 19, 04 - got matrix palette skinning working - just need to get it to work with models over 22 bones. 
Also, noticed a significant drop in performance for skeletal meshes- they now avg around 800 fps instead of 1000 on my 
geforce card.
Oct 15, 04 - hmm, got somewhat behind because of crunching at my job for a few weeks. Got matrix palette skinning
working nicely. started working on full screen capability now.
Nov 6, 04 - started per pixel lighting, learned how to write shaders using assembly. Trying to get Seraph animating...
Classes are now stored to a global array and they now use their own heap. 
Nov 21, 04 - Implemented weighted lighting (for the skinmesh shader), proximity searches, reorganized the octree,
improved importing textures (can assign material classes), and added ability to add shaders to meshes.
Nov 28, 04 - Added the capability for any entity to use AABB Trees. Now users can assign collision types to
entities.
Dec 6, '04 - Shaders can now be applied on a per material basis. Objects can be copied via their Serialize()
function --> GenericObject::GetClone(). Also added the beginning of a HUD class.
Dec 9, 04 - Updated the BlinnPhong Shader. Fixed the DirectX Importer.
Dec 18, 04 - Re-enabled debug volumes, 3rd person camera improvements, and got collision volumes per bone integrated.
Dec 20, 04 - Fixed a serious view frustum problem in the editor dialogs (mesh, character)
Dec 27, 04 - Started working on syncing animations with the character movement
Jan 2, 05  - Redesigned the shader pipeline
July 8, 05 - Redesigned core framework so that objects get stored in a vector. Classes are still stored in an array. Classes
and Objects are now allocated by the Heap manager.
July 9, 05 - Added IObjectLoader interface to FileReader and LevelNode. Provides a clean interface for classes to interrogate
objects that are being loaded from a GODZ package file.
Sept 15, 05 - Collision Volumes now use the foreground color correctly. Minor Optimization to collision volume transformation.
Oct 11, 05 - Finally got the Seraphs working. Updated the animation system to work with matrices given in world space. This way
the resulting animation perfectly matches the motion from the authoring software (3dsmax, etc).
Oct 15, 05 - Experienced another major breakthrough. Since framerate shot up from 24 fps with 3 characters onscreen to 400 fps
after removing STL::Vector access from my animation/render loops-
I thought it was time to switch to software skinning. So, after switching to the PerPixel vertex type I've noticed my
average fps has shot up another 300 fps. Also, I rewrote GenericNode/NodeList. They are now template based. NodeList can now
be accessed like an array and guarantes contigious memory access. Lastly, started working on a Shader Management system
that loads all the shaders in a directory.
Oct 23, 05 - fixed a bug whereas 'extra' Materials were getting serialized. recall that each skeletal instance spawns its' own
set of materials. When the skeletalmesh was saved, all these materials would get saved. The fix to this is to only serialize global objects
that do not have a 'Owner'. Child objects now have to be directly output by their parents. Also, discovered it can be dangerous
to enable full DirectX debugging. It slows down performance a ton, crashes FRAPS, etc. Updated skeletalmesh system to use the mesh chunks
solution for hardware skinning (although I still havent enabled GPU skinning yet). Now, blend vertex type is used for mesh chunks- 
per pixel format used for software animation. Also, implemented vertex animation for skeletal models.
---ENGINE VERSION is now 1002---
Oct 30, 05 - fixed a bug that made an entity lose it's mesh instance when a property was updated. Now the
currently selected model will be highlighted even if it has a shader applied. Updated GenericNode.h to return
a reference to the object when the array operator is used. Updated SkelMeshInstance to use NodeList for bones.
Nov 18, '05 - Added Color4f type to the engine. Materials now have diffuse, ambient, specular, emissive, and power members. Shaders
have been updated to work with these new material properties. Started working on getting the first godz city map into the game.
Fixed GenericObject heap issue. Also, material changes will now be visible in the viewport in Character/Mesh viewer.
Nov 26, 05 - render state changes are now pop/push unto a stack.
Nov 28, 05 - GenericProperty has been cleaned up- now all types are stored in the m_void member.
Jan 22, 06 - Added some shaders I wrote, shaders are now a shared resource (for best speed), added a serializable object
to materials allowing each material to configure shader settings (float,float4,texture).
Jan 29, 06 - Input System created- now key inputs are translated into generic key actions. Each key only allowed one action.
Coded some shaders.
Jan 31, 06 - Updated Input System, greatly improved camera controls (was unplayable if user had low frame rate).
Added support for variable size float array for serializable properties (no longer hard coded for size 4).
Mar 9, 06 - Finally fixed the camera system (was related to the way we grabbed messages from Win32). Now- just need to smooth out animation....
June 11, 06 - Added support for projective texturing, improved asset browser, and added support for background color changes
for viewports. Also, recently made some fixes to collision and updated the core so that callbacks can be sent around whenever
something is collided.
June 27, 06 - Shauni Osborne, my baby boy was born this day. Shortly afterwards I finally got sliding (collision response)
working. It doesnt seem very fast at all atm but I hope I can get it optimized shortly.
Oct 29, 06 - Fixed sliding + got nice frame rate independant movement happening. I think this day marks a huge turning point
because we finally have a good, robust polygon soup collision detection system + nice, fluid movement
Nov 21, 06 - Okay, in this build I finally fixed the 3rd person camera. It's a complex problem but the bottom line is
that I was computing CalcBehindView() AFTER the view matrix got set in the DX9 Driver. So now, what I have learned is that
I must make sure to set the View Matrix BEFORE driver->SetViewMatrix is called. So now the 'jitter' character movement has been
resolved. I also made some significant collision detection/response updates to the engine. I'm finally done with sliding. Its taken
me since early June to get this all resolved (like 6 months!). Then again it wasn't really 6 months because I took a lot of breaks to
play MMORPGs :(
Dec 3, '06 - Reenabled GPU Skinning. Not sure if I realized this before, but the BlendVertex can still be used in the per pixel shader
lightning programs. Took longer than I thought to reenable skinning because I messed up in the shader (didn't realize POSITION needed to be float4
for the bone transformation). Had to download an old version of my shader to solve that one
Dec 9, '06 - Previously what I used to do was allocate DX9 resources for the 'default' mesh instance. However, since we neevr display the default
instance this was a big waste. Tonight I updated the system to allocate everything in system memory initially. We should only create
DX9 resources when the default mesh instance is cloned now
Mar 30, 07 - Got PSSM working (initial implementation). still need to resolve some issues with projection issues in which causes some of my shadows
not to work correctly.
Apr 4, 07 - back from my interview with sony austin (was apr 3, 07). Fixed our input system
Apr 10, 07 - updated engine to go multithreading. 
Apr 29, 07 - moved bone transformation "Game" side. This way gameplay code can safely receive notifications when
animations are complete (ONAnimEnd()). Now what we do is simply copy all the bone matrices so that it can
be rendered. If we need to optimize this- we can possibly only update bone transformations
for meshes currently in view (if pass view frustum check update bones using current delta always). Also, this means the Character Browser is now safe to use.
May 5 '07 - Took out the SetBottomAt() logic from WModel because it was throwing off shadows (GodzGame, was using m_viewBox.min.z) caster AABB
Aug 21, '07 - Added Jumping Physics and an early skill based system (yay!)
Oct 28, '07 - Added Fog effects. Learned that the Fog register can be set in the Vertex Shader however, the video driver will use
my Fog param and blend that with the pixel color computed in the pixel shader.
Feb 3, 2008 - Move the PushGenericObject() code out of StaticConstructObject() function. This way, the Resource manager
can safely instantiate Generic objects on the fly. Next, move all Archive code into ResourceLoader code. So,
all package loading would have to be relocated to it. After loading is done the Game thread can request all the Generic
objects that have been created and then proceed to register these objects manually. Also added support for CRC32

july 29, 08 - engine running at 13 fps even when I'm rendering nothing. Something is wrong with visual .net 2003 is my guess :(
May try running the new build on my old computer and see what fps I get or just try the build on another machine.



- Now Entities need to update their WModel when their location changes



TODO:
Dec 31, '07 - started investigating true seamless resource loading..... 

Plans (options):


*Need to move that global object vector into the Game Thread directly if possible.

Note - GetCurrentProcessId().




-Need to Batch Shaders, etc
- Update DX9ModelResource. perhaps change around all the Draw() and move it into a BatchRenderer class that knows
 how to work with ModelResources and batch them together. might need to put in Godzengine.DLL
 
- change all mutex locks into critical_section
- Need to let each modelresource specify a "offset" matrix. this way entities can be composed of multiple resources that
have their own collision and can be culled inidividually

Serious Show Stoppers:

1) When I added Vertex Fog on Oct 28, '07 encountered massive slowdown. Not sure if Fog casued this. I would have
to remove the code in the DX9 Driver that turns it on plus the Fog output in the shaders.

Known Issues: 

1) GSA importer has some issues with multi-object materials
2) ASE importer needs to flip/exchange y and z

Helpful sources:
http://www.gametutorials.com
http://hyper.sunjapan.com.cn/~hz/win32/commctl5.htm (docs on win32 controls)
http://www.dev-gallery.com/programming/opengl/colision/collision2.htm (ray-plane intersection)

Books:
3D Game Engine Design by David Eberly
Introduction to 3D Game Programming with DIRECTX 9.0 by Frank D. Luna
Elementary Linear Algebra APPLICATIONS VERSION


Notes:

* In order to run this with FRAPS, make sure to run the .exe by itself. dont run it from Visual Studio
* problem related to DLLs. you can't allocate memory and copy that into a STL
vector like so--->

void getParams(vector<int*> &list) //list vector is passed from another DLL...
{
	list.push_back(whatever...);
}

causes another DLL heap exception!	