#version 460 core

layout(location = 0) in vec3 in_Color;
layout(location = 1) in vec3 in_Normal;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 lightDir = normalize(vec3(1, 1, 0));


	float intensity = max(dot(in_Normal, lightDir), 0.1);


	out_color = vec4(vec3(intensity, intensity, intensity) * in_Color, 1);
}