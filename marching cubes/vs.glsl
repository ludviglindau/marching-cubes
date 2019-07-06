#version 450

in vec4 vertexPos;
in vec4 vertexNormal;

out vec4 worldPos;
out vec4 worldNormal;

layout(location = 0) uniform mat4 viewProjection;

void main()
{
    gl_Position = viewProjection * vec4(vertexPos.xyz, 1.0);
	worldPos = vertexPos;
	worldNormal = vertexNormal;
}