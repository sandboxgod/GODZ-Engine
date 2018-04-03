--Local settings

--Import Dynamic libraries
Imports={
  "DX9RenderPipeline",  --Imports DirectX9 Renderer DLL
}

Application={
	FullScreen = false
}

Window={
  HideMouse=false,
  MouseSpeed=1.7
}

PostEffects={
	HDRLighting=false,
	Multisampling=false   --turns on antialiasing
}

RenderFx = {
	Show_Fog = false	--turn off fog which will mess up our Mesh Browsers
}

--options for the character dialog
CharacterDialog={
ModelRotate=0.3			--amount you want to rotate model by
}

Shadows={	
	--Parellel split - number of splits to render
	NumSplits=0
}