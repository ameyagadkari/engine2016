/*
	This is an example of a fragment shader
*/

// The version of GLSL to use must come first
#version 420

// Constants
//==========

layout( std140, binding = 0 ) uniform constantBuffer_frame
{
	float g_elapsedSecondCount_total;
};

// Input
//======

// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
// will be interpolated across the triangle and given as input to the fragment shader

layout( location = 0 ) in vec4 i_color;

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()
{
	// For now set the fragment as the interpolated color
	o_color = i_color;
}
