#version 450
#extension GL_ARB_Separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

vec3 lightVec = {0,0,-1};

void main()
{
	vec2 extentCoord = fragTexCoord * 2.0f;
	outColor = texture(texSampler, extentCoord);
	//outColor = vec4(1.0,0.0,0.0, 1.0);
}

