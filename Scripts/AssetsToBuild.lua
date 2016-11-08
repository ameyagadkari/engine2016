--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	-- The top-level table is a dictionary whose keys
	-- correspond to the asset types (inherited from cbAssetTypeInfo)
	-- that you have defined in AssetBuildSystem.lua
	gameobjects =
	{
		-- The actual assets can be defined simply as relative path strings (the common case)
		"gameobjects/plane.txtgobj",
		--[["gameobjects/snake.txtgobj",
		"gameobjects/cube.txtgobj",
		"gameobjects/apple.txtgobj",
		"gameobjects/mynamewithsubject.txtgobj",]]
	},
	meshes =
	{
		-- The actual assets can be defined simply as relative path strings (the common case)
		"meshes/plane.txtmesh",
		--[["meshes/snake.txtmesh",
		"meshes/newcube.txtmesh",
		"meshes/apple.txtmesh",
		"meshes/mynamewithsubject.txtmesh",]]
	},
	shaders =
	{
		-- But it is also possible to define them as tables with more information
		-- (shaders must specify an additional command line argument for ShaderBuilder.exe
		-- to specify whether an asset is a vertex shader or a fragment shader)
		{ path = "shaders/vertex.txtshader", arguments = { "vertex" } },
		{ path = "shaders/fragment.txtshader", arguments = { "fragment" } },
	},
}
