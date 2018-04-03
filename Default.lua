--GODZ Engine Configuration

--Import Dynamic libraries
Imports={
  "DX9RenderPipeline",  --Imports DirectX9 Renderer DLL
}



PostEffects={
	HDRLighting=false,
	HDRExposureLevel = 0.7,
	Multisampling=false   --turns on antialiasing
}

RenderFx = {
	Show_Fog = false	--turns on fog rendering.
}

Window={
  HideMouse=true,
}

Application={
	FullScreen = false,
	PreferredWidth = 1200,
	PreferredHeight = 900
}


Camera={
  Speed=1500
}