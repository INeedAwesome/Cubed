#version 460 core

layout(location = 0) in vec3 in_Color;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D u_Texture;

void main()
{
	vec3 lightDir = normalize(vec3(1, 1, 0));


	float intensity = max(dot(in_Normal, lightDir), 0.1);

	vec4 textureColor = texture(u_Texture, in_UV);

	out_color = vec4(vec3(intensity) * textureColor.rgb, 1);
	// out_color = vec4(in_UV, 1, 1);
}