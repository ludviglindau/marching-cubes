#version 450
layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

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
layout(location = 0) uniform float voxelDim;
layout(location = 1) uniform ivec3 chunk;
layout(location = 2) uniform float scale;

vec3 interpolateVertex(vec3 p1, vec3 p2, float val1, float val2) {
    vec3 p;

    if (abs(val1) < 0.00001)
        return p1;
    if (abs(val2) < 0.00001)
        return p2;
    if (abs(val1 - val2) < 0.00001)
        return p1;
    float mu = -val1 / (val2 - val1);
    p = p1 + mu * (p2 - p1);
    return p;
}

void main()
{
	ivec3 p = ivec3(gl_GlobalInvocationID.xyz);
	float corners[8];
	const vec3 offsets[8] = vec3[8](
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

	vec3 vertexList[12];
	vec3 pos = vec3(p);

	if (bool(edgeTable[cubeIndex] & 1))
		vertexList[0] = interpolateVertex(pos + offsets[0], pos + offsets[1], corners[0], corners[1]);
	if (bool(edgeTable[cubeIndex] & 2))
		vertexList[1] = interpolateVertex(pos + offsets[1], pos + offsets[2], corners[1], corners[2]);
	if (bool(edgeTable[cubeIndex] & 4))
		vertexList[2] = interpolateVertex(pos + offsets[2], pos + offsets[3], corners[2], corners[3]);
	if (bool(edgeTable[cubeIndex] & 8))
		vertexList[3] = interpolateVertex(pos + offsets[3], pos + offsets[0], corners[3], corners[0]);
	if (bool(edgeTable[cubeIndex] & 16))
		vertexList[4] = interpolateVertex(pos + offsets[4], pos + offsets[5], corners[4], corners[5]);
	if (bool(edgeTable[cubeIndex] & 32))
		vertexList[5] = interpolateVertex(pos + offsets[5], pos + offsets[6], corners[5], corners[6]);
	if (bool(edgeTable[cubeIndex] & 64))
		vertexList[6] = interpolateVertex(pos + offsets[6], pos + offsets[7], corners[6], corners[7]);
	if (bool(edgeTable[cubeIndex] & 128))
		vertexList[7] = interpolateVertex(pos + offsets[7], pos + offsets[4], corners[7], corners[4]);
	if (bool(edgeTable[cubeIndex] & 256))
		vertexList[8] = interpolateVertex(pos + offsets[0], pos + offsets[4], corners[0], corners[4]);
	if (bool(edgeTable[cubeIndex] & 512))
		vertexList[9] = interpolateVertex(pos + offsets[1], pos + offsets[5], corners[1], corners[5]);
	if (bool(edgeTable[cubeIndex] & 1024))
		vertexList[10] = interpolateVertex(pos + offsets[2], pos + offsets[6], corners[2], corners[6]);
	if (bool(edgeTable[cubeIndex] & 2048))
		vertexList[11] = interpolateVertex(pos + offsets[3], pos + offsets[7], corners[3], corners[7]);

	Triangle triangleList[5];
	int triangleCounter = 0;
	for (int i = 0; triTable[cubeIndex][i] != -1; i += 3)
	{
        vec3 v1 = vertexList[triTable[cubeIndex][i]]   - (voxelDim * 0.5);
        vec3 v2 = vertexList[triTable[cubeIndex][i+1]] - (voxelDim * 0.5);
        vec3 v3 = vertexList[triTable[cubeIndex][i+2]] - (voxelDim * 0.5);

		triangleList[triangleCounter].v1 = vec4(v1 + (chunk * voxelDim), 1.0) * scale;
		triangleList[triangleCounter].v2 = vec4(v2 + (chunk * voxelDim), 1.0) * scale;
		triangleList[triangleCounter].v3 = vec4(v3 + (chunk * voxelDim), 1.0) * scale;

        v1 += voxelDim * 0.5;
        v2 += voxelDim * 0.5;
        v3 += voxelDim * 0.5;
            
        triangleList[triangleCounter].n1 = vec4(texture(noiseTexture, v1/voxelDim).gba, 0.0);
        triangleList[triangleCounter].n2 = vec4(texture(noiseTexture, v2/voxelDim).gba, 0.0);
        triangleList[triangleCounter].n3 = vec4(texture(noiseTexture, v3/voxelDim).gba, 0.0);

		triangleCounter++;
	}
   
    memoryBarrier();
	uint vboIndex = atomicAdd(counter[0], triangleCounter);
    
	for (int i = 0; i < triangleCounter; ++i)
	{
		triangles[vboIndex + i] = triangleList[i];
	}
}
