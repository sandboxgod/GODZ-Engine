--Global Gameplay/Engine Settings

Engine={
	--Turn this flag on if you want to dynamically alter bone movement. For regular playback,
	--leave this flag of for optimum performance
	UseHierarchicalAnimation=false, 

	--optimize software skeletal animation. Covert skeletal animation to vertex animation 
	--(more memory intense but saves CPU)
	UseSavedSkelFrames=false,
	UseHWSkinning=true, 
}

Editor={
	SplitWidth=10000,
	SplitDepth=10000,
	SplitMinNumberOfTriangles=1000
}

Camera={
	MouseLook = true,
	MoveSpeed = 100
}

Shadows={
	--Parellel split - number of splits to render
	NumSplits=4,
	ShadowMapSize=2048,

	--Distance to convert shadow-lights into non-casters
	ShadowLightFadeDist=9000, 

	--0=Low (1 map + clipped vbox), 1=Med, 2=High, 3=Ultra
	ShadowMapQuality=2,
	Lamda=0.65, --PSSM
	ViewBoxSize = 5000, --for low settings computers/users
	DisplayShadowmaps=false,
	ManualSplitPlanes=true,
	SplitRange0 = 0,  --ignored but here for reference!
	SplitRange1 = 0.05,
	SplitRange2 = 0.13,
	SplitRange3 = 0.3,
	SplitRange4 = 1, -- ignored, for reference
}

Shaders = {
DefaultShader = "Shaders\\world_lit_blinn_phong.fxo",
DefaultSkinShader = "Shaders\\Shadow_MatrixPalette.fxo",
}

--Configure the Graphics Renderer
Driver={
  ResourceManagerClass="DX9RenderPipeline.DXResourceManager",
  Class="DX9RenderPipeline.DX9Driver",
  ShadowMapManagerClass="DX9RenderPipeline.DX9ShadowMapManager",
  HDRManagerClass="DX9RenderPipeline.DX9HDRManager",
  Use_SW=false,			
  DebugVertexShaders=false,
  DisableDeferredRenderer=false, --for low specs
--DebugNodeTree=true,
--RenderEntityBound=true,
  ShowFPS=true,
--RenderCollisionTree=true,
--ShowCameraInfo=true,
  UseVSync=true,  
  UseShadowMaps=true,  
}

--Animation Sequences. Every character needs to define these animation sequences. Note, the
--max limit of animation sets that can be defined is 255. 
--types: Run, Walk

Sequences={
	"None",  --requires specialized code to play the anim
	"RunF",   --running animation
	"RunB",
	"RunL",
	"RunR",
	"Walk",  --walking animation
	"Idle",  --idle animation
	"Jump",  --jumping animation
	"Firing"
}

-- system folders
Folders={
	Characters="Data",    --character models folder
	Meshes="Data",  	--meshes folder
	Textures="Data",    -- textures folder
}

--key mappings
KeyActions={
	Key_ArrowUp = "MoveUp",
	Key_ArrowDown="MoveDown",
	Key_ArrowLeft="MoveLeft",
	Key_ArrowRight="MoveRight",
	Key_W="MoveUp",
	Key_A="MoveLeft",
	Key_S="MoveDown",
	Key_D="MoveRight",
	Key_Spacebar="Jump",
	Key_C="Crouch",
	Key_E="RotateRight",
	Key_Q="RotateLeft",
	Key_V="ViewEditor",
	Key_Z="PitchUp",
	Key_X="PitchDown",
	Key_Ctrl="MoveCamDown",
	Key_B="TakeSnapshotShadowMap",
	Key_N="DebugFirstPassOnly",
	Key_M="SelectNextCamera",
}


