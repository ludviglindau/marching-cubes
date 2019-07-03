#version 460

in vec4 vertexPos;
in vec4 vertexNormal;

out vec4 worldPos;
out flat vec4 worldNormal;

layout(location = 0) uniform mat4 world;
layout(location = 2) uniform mat4 projection;

void main()
{
    gl_Position = projection * world * vec4(vertexPos.xyz, 1.0);
	worldPos = world * vertexPos;
	worldNormal = world * vertexNormal;
}