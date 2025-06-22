#version 460 core


layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;

layout(location = 0) out vec3 out_Color;
layout(location = 1) out vec3 out_Normal;
layout(location = 2) out vec2 out_UV;

layout (push_constant) uniform PushConstants
{
	mat4 ViewProjection;
	mat4 Transform;
} u_PushConstants;



void main()
{
	gl_Position = 	u_PushConstants.ViewProjection * 
					u_PushConstants.Transform * 
					vec4(a_Position, 1.0);

	// Gets the rotation of the transform https://youtu.be/QFf91u8bZlk?t=2854 
	//			0 -> 1     scaling   idk     removes position offset
	out_Normal = normalize(transpose(inverse(mat3(u_PushConstants.Transform))) * a_Normal);
	out_UV = a_UV;
}