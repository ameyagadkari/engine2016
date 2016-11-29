/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

// The version of GLSL to use must come first
#version 420

#endif

// Types
//======

#if defined( EAE6320_PLATFORM_D3D )

	#define vec2 float2
	#define vec3 float3
	#define vec4 float4
	#define mat2 float2x2
	#define mat3 float3x3
	#define mat4 float4x4

#elif defined( EAE6320_PLATFORM_GL )

	#define float2 vec2
	#define float3 vec3
	#define float4 vec4
	#define float2x2 mat2
	#define float3x3 mat3
	#define float4x4 mat4

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

#endif

// Operators
//=================
#if defined( EAE6320_PLATFORM_GL )

#	define mul(a, b) a * b 

#endif

// Texture Objects
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareTexture2DObject( i_name, i_id ) Texture2D i_name : register( t##i_id );

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareTexture2DObject( i_name, i_id ) layout( binding = i_id ) uniform sampler2D i_name;

#endif

// Sampler State Objects
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareSamplerStateObject( i_name, i_id ) SamplerState i_name : register( s##i_id );

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareSamplerStateObject( i_name, i_id )

#endif

// Sampled Color
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareSampledColor( i_texture2D, i_samplerstate, i_texcoord ) i_texture2D.Sample( i_samplerstate, i_texcoord );

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareSampledColor( i_texture2D, i_samplerstate, i_texcoord ) texture2D( i_texture2D, i_texcoord );

#endif

DeclareConstantBuffer( constantBuffer_frame, 0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToScreen;
	float g_elapsedSecondCount_total;
};

DeclareConstantBuffer( constantBuffer_drawCall, 1 )
{
	float4x4 g_transform_localToWorld;
};

DeclareConstantBuffer( constantBuffer_material, 2 )
{
	float4 g_color;
};

DeclareTexture2DObject( primaryTexture, 0 )

DeclareSamplerStateObject( samplerStateHLSL, 0 )