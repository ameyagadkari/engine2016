/*
	This is an example of a vertex shader
*/

// Constants
//==========

cbuffer constantBuffer_frame : register( b0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToScreen;
	float g_elapsedSecondCount_total;
}
cbuffer constantBuffer_drawCall : register( b1 )
{
	float4x4 g_transform_localToWorld;
}

// Entry Point
//============

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	// These values come from one of the sVertex that we filled the vertex buffer with in C code
	in const float3 i_vertexPosition_local : POSITION,
	in const float4 i_color : COLOR,

	// Output
	//=======

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out float4 o_vertexPosition_screen : SV_POSITION,

	// Any other data is optional; the GPU doesn't know or care what it is,
	// and will merely interpolate it across the triangle
	// and give us the resulting interpolated value in a fragment shader.
	// It is then up to us to use it however we want to.
	// The "semantics" are used to match the vertex shader outputs
	// with the fragment shader inputs
	// (note that OpenGL uses arbitrarily assignable number IDs to do the same thing).
	out float4 o_color : COLOR

	)
{
	// Calculate the position of this vertex on screen
	{
		// The position stored in the vertex is in "local space",
		// meaning that it is relative to the center (or "origin", or "pivot")
		// of the mesh.
		// The graphics hardware needs the position of the vertex
		// in normalized device coordinates,
		// meaning where the position of the vertex should be drawn
		// on the screen.
		// This position that we need to output, then,
		// is the result of taking the original vertex in local space
		// and transforming it into "screen space".

		// Any matrix transformations that include translation
		// will operate on a float4 position,
		// which _must_ have 1 for the w value
		float4 vertexPosition_local = float4( i_vertexPosition_local, 1.0 );
		// Transform the vertex from local space into world space
		float4 vertexPosition_world = mul( vertexPosition_local, g_transform_localToWorld );
		// Transform the vertex from world space into camera space
		float4 vertexPosition_camera = mul( vertexPosition_world, g_transform_worldToCamera );
		// Project the vertex from camera space into screen space
		o_vertexPosition_screen = mul( vertexPosition_camera, g_transform_cameraToScreen );
	}
	// Pass the input color to the fragment shader unchanged
	{
		o_color = i_color;
	}
}
