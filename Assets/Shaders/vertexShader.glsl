/*
	This is an example of a vertex shader
*/

// The version of GLSL to use must come first
#version 420

// Constants
//==========

layout( std140, binding = 0 ) uniform constantBuffer_frame
{
	float g_elapsedSecondCount_total;
};
layout( std140, binding = 1 ) uniform constantBuffer_drawCall
{
	vec2 g_objectPosition_screen;
};

// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()

// These values come from one of the sVertex that we filled the vertex buffer with in C code
layout( location = 0 ) in vec2 i_vertexPosition_local;
layout( location = 1 ) in vec4 i_color;

// Output
//=======

// None; the vertex shader must always output a position value,
// but unlike HLSL where the value is explicit
// GLSL has an implicit required variable called "gl_Position"

// Any other data is optional; the GPU doesn't know or care what it is,
// and will merely interpolate it across the triangle
// and give us the resulting interpolated value in a fragment shader.
// It is then up to us to use it however we want to.
// The locations are used to match the vertex shader outputs
// with the fragment shader inputs
// (note that Direct3D uses arbitrarily assignable "semantics").
layout( location = 0 ) out vec4 o_color;

// Entry Point
//============

void main()
{
	// Calculate position
	{
		// The vertex's output position is calculated by adding
		// the vertex's position offset from the local origin
		// to the object's screen position
		gl_Position = vec4( g_objectPosition_screen + i_vertexPosition_local, 0.0, 1.0 );
	}
	// Pass the input color to the fragment shader unchanged
	{
		o_color = i_color;
	}
}
