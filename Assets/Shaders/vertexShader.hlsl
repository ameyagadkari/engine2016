/*
	This is an example of a vertex shader
*/

// Constants
//==========

cbuffer constantBuffer_frame : register( b0 )
{
	float g_elapsedSecondCount_total;
}
cbuffer constantBuffer_drawCall : register( b1 )
{
	float2 g_objectPosition_screen;
}

// Entry Point
//============

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	// These values come from one of the sVertex that we filled the vertex buffer with in C code
	in const float2 i_vertexPosition_local : POSITION,
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
		// The vertex's output position is calculated by adding
		// the vertex's position offset from the local origin
		// to the object's screen position
		o_vertexPosition_screen = float4( g_objectPosition_screen + i_vertexPosition_local, 0.0, 1.0 );
	}
	// Pass the input color to the fragment shader unchanged
	{
		o_color = i_color;
	}
}
