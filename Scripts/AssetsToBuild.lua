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
		"gameobjects/snake.txtgobj",
		"gameobjects/cube.txtgobj",
		"gameobjects/apple.txtgobj",
		"gameobjects/mynamewithsubject.txtgobj",
		"gameobjects/brownbear.txtgobj",
		"gameobjects/orangebear.txtgobj",
	},
	gameobjects2d = 
	{
		"gameobjects2d/logo.txtg2dobj",
	},
}
