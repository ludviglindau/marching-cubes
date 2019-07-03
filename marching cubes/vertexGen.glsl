#version 460
layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

struct Triangle {
	vec4 v1;
    vec4 n1;
	vec4 v2;
    vec4 n2;
	vec4 v3;
    vec4 n3;
};

layout(std430, binding = 0) buffer TriangleBuffer
{
	Triangle triangles[];
};

layout(std430, binding = 1) buffer Counter
{
	uint counter[];
};

layout(std430, binding = 2) readonly buffer EdgeTable
{
	int edgeTable[256];
};

layout(std430, binding = 3) readonly buffer TriangleTable
{
	int triTable[256][16];
};

layout(binding = 0) uniform sampler3D noiseTexture;

void main()
{
	ivec3 p = ivec3(gl_GlobalInvocationID.xyz);
	float corners[8];
	vec3 offsets[8] = vec3[8](
		vec3(0.0, 0.0, 1.0),
		vec3(1.0, 0.0, 1.0),
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 0.0, 0.0),
		vec3(0.0, 1.0, 1.0),
		vec3(1.0, 1.0, 1.0),
		vec3(1.0, 1.0, 0.0),
		vec3(0.0, 1.0, 0.0)
	);

	corners[0] = texelFetch(noiseTexture, p + ivec3(offsets[0]), 0).x;
	corners[1] = texelFetch(noiseTexture, p + ivec3(offsets[1]), 0).x;
	corners[2] = texelFetch(noiseTexture, p + ivec3(offsets[2]), 0).x;
	corners[3] = texelFetch(noiseTexture, p + ivec3(offsets[3]), 0).x;
	corners[4] = texelFetch(noiseTexture, p + ivec3(offsets[4]), 0).x;
	corners[5] = texelFetch(noiseTexture, p + ivec3(offsets[5]), 0).x;
	corners[6] = texelFetch(noiseTexture, p + ivec3(offsets[6]), 0).x;
	corners[7] = texelFetch(noiseTexture, p + ivec3(offsets[7]), 0).x;
	
    int cubeIndex = 0;
	if (corners[0] < 0.0f) cubeIndex |= 1;
	if (corners[1] < 0.0f) cubeIndex |= 2;
	if (corners[2] < 0.0f) cubeIndex |= 4;
	if (corners[3] < 0.0f) cubeIndex |= 8;
	if (corners[4] < 0.0f) cubeIndex |= 16;
	if (corners[5] < 0.0f) cubeIndex |= 32;
	if (corners[6] < 0.0f) cubeIndex |= 64;
	if (corners[7] < 0.0f) cubeIndex |= 128;
	if (edgeTable[cubeIndex] == 0) {
        return;
    }

	vec3 vertexList[12] = vec3[12](0.0);
	vec3 pos = vec3(p);
	if (bool(edgeTable[cubeIndex] & 1))
		vertexList[0] = mix(pos + offsets[0], pos + offsets[1], 0.5);
	if (bool(edgeTable[cubeIndex] & 2))
		vertexList[1] = mix(pos + offsets[1], pos + offsets[2], 0.5);
	if (bool(edgeTable[cubeIndex] & 4))
		vertexList[2] = mix(pos + offsets[2], pos + offsets[3], 0.5);
	if (bool(edgeTable[cubeIndex] & 8))
		vertexList[3] = mix(pos + offsets[3], pos + offsets[0], 0.5);
	if (bool(edgeTable[cubeIndex] & 16))
		vertexList[4] = mix(pos + offsets[4], pos + offsets[5], 0.5);
	if (bool(edgeTable[cubeIndex] & 32))
		vertexList[5] = mix(pos + offsets[5], pos + offsets[6], 0.5);
	if (bool(edgeTable[cubeIndex] & 64))
		vertexList[6] = mix(pos + offsets[6], pos + offsets[7], 0.5);
	if (bool(edgeTable[cubeIndex] & 128))
		vertexList[7] = mix(pos + offsets[7], pos + offsets[4], 0.5);
	if (bool(edgeTable[cubeIndex] & 256))
		vertexList[8] = mix(pos + offsets[0], pos + offsets[4], 0.5);
	if (bool(edgeTable[cubeIndex] & 512))
		vertexList[9] = mix(pos + offsets[1], pos + offsets[5], 0.5);
	if (bool(edgeTable[cubeIndex] & 1024))
		vertexList[10] = mix(pos + offsets[2], pos + offsets[6], 0.5);
	if (bool(edgeTable[cubeIndex] & 2048))
		vertexList[11] = mix(pos + offsets[3], pos + offsets[7], 0.5);

	Triangle triangleList[5];
	int triangleCounter = 0;
	for (int i = 0; triTable[cubeIndex][i] != -1; i += 3)
	{
        vec3 v1 = vertexList[triTable[cubeIndex][i]];
        vec3 v2 = vertexList[triTable[cubeIndex][i+1]];
        vec3 v3 = vertexList[triTable[cubeIndex][i+2]];

		triangleList[triangleCounter].v1 = vec4(v1, 1.0);
		triangleList[triangleCounter].v2 = vec4(v2, 1.0);
		triangleList[triangleCounter].v3 = vec4(v3, 1.0);

        //triangleList[triangleCounter].n1 = vec4(texture(noiseTexture, triangleList[triangleCounter].v1.xyz).yzw, 0.0);
        //triangleList[triangleCounter].n2 = vec4(texture(noiseTexture, triangleList[triangleCounter].v2.xyz).yzw, 0.0);
        //triangleList[triangleCounter].n3 = vec4(texture(noiseTexture, triangleList[triangleCounter].v3.xyz).yzw, 0.0);

        triangleList[triangleCounter].n1 = vec4(cross(v3.xyz - v1.xyz, v2.xyz - v1.xyz), 0.0);
        triangleList[triangleCounter].n2 = vec4(cross(v3.xyz - v1.xyz, v2.xyz - v1.xyz), 0.0);
        triangleList[triangleCounter].n3 = vec4(cross(v3.xyz - v1.xyz, v2.xyz - v1.xyz), 0.0);

        //triangleList[triangleCounter].n1 = vec4(1.0, 0.0, 0.0, 0.0);
        //triangleList[triangleCounter].n2 = vec4(0.0, 1.0, 0.0, 0.0);
        //triangleList[triangleCounter].n3 = vec4(0.0, 0.0, 1.0, 0.0);

		triangleCounter++;
	}
   
    //memoryBarrier();
	uint vboIndex = atomicAdd(counter[0], triangleCounter);
    
	for (int i = 0; i < triangleCounter; ++i)
	{
		triangles[vboIndex + i] = triangleList[i];
	}
}
