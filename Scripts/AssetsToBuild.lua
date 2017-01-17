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
		"gameobjects/ceiling.txtgobj",
		"gameobjects/floor.txtgobj",
		"gameobjects/frontwall.txtgobj",
		"gameobjects/backwall.txtgobj",
		"gameobjects/rightwall.txtgobj",
		"gameobjects/leftwall.txtgobj",
	},
	--[[gameobjects2d = 
	{
		"gameobjects2d/logo.txtg2dobj",
		"gameobjects2d/score.txtg2dobj",
	},]]
}
