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
		-- The actual assets can be defined simply as relative path strings (the common case
		"gameobjects/center.txtgobj",
		"gameobjects/ceiling.txtgobj",
		"gameobjects/floor.txtgobj",
		"gameobjects/frontwall.txtgobj",
		"gameobjects/backwall.txtgobj",
		"gameobjects/rightwall.txtgobj",
		"gameobjects/leftwall.txtgobj",
		"gameobjects/wallbraceouter.txtgobj",
		"gameobjects/railing.txtgobj",
		"gameobjects/firstfloorceiling.txtgobj",
		"gameobjects/firstfloor.txtgobj",
		"gameobjects/archwayceiling.txtgobj",
		"gameobjects/insidewalls.txtgobj",
		"gameobjects/wallbraceinner.txtgobj",
		--"gameobjects/playerfirstperson.txtgobj",
		"gameobjects/playerthirdperson.txtgobj",
		"gameobjects/playerthirdpersonremote.txtgobj",
		"gameobjects/myteamflagserver.txtgobj",
		"gameobjects/otherteamflagserver.txtgobj",
		"gameobjects/myteamflagclient.txtgobj",
		"gameobjects/otherteamflagclient.txtgobj",
		"gameobjects/myscorezoneserver.txtgobj",
		"gameobjects/otherscorezoneserver.txtgobj",
		"gameobjects/myscorezoneclient.txtgobj",
		"gameobjects/otherscorezoneclient.txtgobj",
	},
	materials =
	{
		"materials/primitive_material.txtmaterial",
	},
	fonts =
	{
		"fonts/myfont.txtfont",
	},
	meshes =
	{
		"meshes/collisiondata.txtmesh",
	},
	gameobjects2d =
	{
		"gameobjects2d/ui/checked.txtg2dobj",
		"gameobjects2d/ui/unchecked.txtg2dobj",
		"gameobjects2d/ui/sliderbar.txtg2dobj",
		--"gameobjects2d/logo.txtg2dobj",
		--"gameobjects2d/score.txtg2dobj",
		"gameobjects2d/network/networkscreenbackground.txtg2dobj",
	},
	audio = 
	{
		"audio/2D/backgroundmusic.wav",
	},
}
