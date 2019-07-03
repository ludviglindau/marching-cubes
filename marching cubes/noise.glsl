#version 460
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image3D outTexture;

layout(location = 0) uniform float seed;
layout(location = 1) uniform float time;
layout(location = 2) uniform float texels;

vec3 random3(vec3 p)
{
    p = vec3(dot(p, vec3(905.0, 311.365, 320.79)),
              dot(p, vec3(684.63, 866.28, 95.22144554)),
              dot(p, vec3(174.62, 544.25, 151.8542)));
    return -1.0 + 2.0 * fract(sin(p) * 12583.3602648 + (seed * seed * seed));
}

float noise(vec3 p)
{
    vec3 v = floor(p);
    vec3 w = fract(p);
    vec3 u = 6.*w*w*w*w*w - 15.*w*w*w*w + 10.*w*w*w;

    vec3 a = vec3(0.0, 0.0, 0.0);
    vec3 b = vec3(1.0, 0.0, 0.0);
    vec3 c = vec3(0.0, 1.0, 0.0);
    vec3 d = vec3(1.0, 1.0, 0.0);
	vec3 e = vec3(0.0, 0.0, 1.0);
    vec3 f = vec3(1.0, 0.0, 1.0);
    vec3 g = vec3(0.0, 1.0, 1.0);
    vec3 h = vec3(1.0, 1.0, 1.0);

    float bot = mix(mix(dot(random3(v + a), w - a),
                    dot(random3(v + b), w - b), u.x),
                mix(dot(random3(v + c), w - c),
                    dot(random3(v + d), w - d), u.x), u.y);
	
    float top = mix(mix(dot(random3(v + e), w - e),
                    dot(random3(v + f), w - f), u.x),
                mix(dot(random3(v + g), w - g),
                    dot(random3(v + h), w - h), u.x), u.y);

	return mix(bot, top, u.z);
}

float fbm(vec3 p)
{
	float value = p.y - 0.05;
	//value = 0.05 - exp(-length(p - texels * 0.5));

    value += noise(p);
    value += noise(p*2.0) * 0.5;
    value += noise(p*4.0) * 0.25;
	value += noise(p*8.0) * 0.125;

	return value;
}


vec3 calcNormal(vec3 p)
{
    const float eps = 0.05;
    const vec2 h = vec2(eps, 0);

    return normalize(vec3(fbm(p + h.xyy) - fbm(p - h.xyy),
                          fbm(p + h.yxy) - fbm(p - h.yxy),
                          fbm(p + h.yyx) - fbm(p - h.yyx)));
}

void main()
{
	vec3 p = gl_GlobalInvocationID.xyz;
	vec3 noiseInput = (p) / (texels * 0.5);
	float density = fbm(noiseInput);
	//density = length(p - vec3(32.0)) - 5.5;
	vec3 normal = calcNormal(noiseInput);
	//normal = normalize(p-vec3(32.0));
	imageStore(outTexture, ivec3(p), vec4(density, normal.x, normal.y, normal.z));
}
