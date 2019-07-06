#version 450
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba16f, binding = 0) uniform image3D outTexture;

layout(location = 0) uniform float seed;
layout(location = 1) uniform float time;
layout(location = 2) uniform float texels;
layout(location = 3) uniform ivec3 chunk;

vec3 random3(vec3 p)
{
    p = vec3(dot(p, vec3(905.0, 311.365, 320.79)),
              dot(p, vec3(684.63, 866.28, 95.22144554)),
              dot(p, vec3(174.62, 544.25, 151.8542)));
    return -1.0 + 2.0 * fract(sin(p) * 12583.3602648 + (seed * 155.155));
}
vec4 noise(vec3 p)
{
    vec3 v = floor(p);
    vec3 w = fract(p);
    vec3 u = 6.*w*w*w*w*w - 15.*w*w*w*w + 10.*w*w*w;
	vec3 du = 30.0*w*w*(w*(w-2.0)+1.0);

    vec3 a = vec3(0.0, 0.0, 0.0);
    vec3 b = vec3(1.0, 0.0, 0.0);
    vec3 c = vec3(0.0, 1.0, 0.0);
    vec3 d = vec3(1.0, 1.0, 0.0);
	vec3 e = vec3(0.0, 0.0, 1.0);
    vec3 f = vec3(1.0, 0.0, 1.0);
    vec3 g = vec3(0.0, 1.0, 1.0);
    vec3 h = vec3(1.0, 1.0, 1.0);

	vec3 ga = random3(v + a);
	vec3 gb = random3(v + b);
	vec3 gc = random3(v + c);
	vec3 gd = random3(v + d);
	vec3 ge = random3(v + e);
	vec3 gf = random3(v + f);
	vec3 gg = random3(v + g);
	vec3 gh = random3(v + h);

	float va = dot(ga, w - a);
	float vb = dot(gb, w - b);
	float vc = dot(gc, w - c);
	float vd = dot(gd, w - d);
	float ve = dot(ge, w - e);
	float vf = dot(gf, w - f);
	float vg = dot(gg, w - g);
	float vh = dot(gh, w - h);

	float noiseVal = va + 
                     u.x*(vb-va) + 
                     u.y*(vc-va) + 
                     u.z*(ve-va) + 
                     u.x*u.y*(va-vb-vc+vd) + 
                     u.y*u.z*(va-vc-ve+vg) + 
                     u.z*u.x*(va-vb-ve+vf) + 
                     u.x*u.y*u.z*(-va+vb+vc-vd+ve-vf-vg+vh);

	vec3 derivative = ga + 
                      u.x*(gb-ga) + 
                      u.y*(gc-ga) + 
                      u.z*(ge-ga) + 
                      u.x*u.y*(ga-gb-gc+gd) + 
                      u.y*u.z*(ga-gc-ge+gg) + 
                      u.z*u.x*(ga-gb-ge+gf) + 
                      u.x*u.y*u.z*(-ga+gb+gc-gd+ge-gf-gg+gh) +   
                      
                      du * (vec3(vb-va,vc-va,ve-va) + 
                            u.yzx*vec3(va-vb-vc+vd,va-vc-ve+vg,va-vb-ve+vf) + 
                            u.zxy*vec3(va-vb-ve+vf,va-vb-vc+vd,va-vc-ve+vg) + 
                            u.yzx*u.zxy*(-va+vb+vc-vd+ve-vf-vg+vh));

	return vec4(noiseVal, derivative);
}

vec4 fbm(vec3 p)
{
	p *= 0.5;
	vec4 value = vec4(p.y * 0.5, 0.0, 0.00001, 0.0);
	mat3 rotate = mat3(cos(0.0), 0.0, sin(0.0),
					   0.0     , 1.0, 0.0,
					  -sin(0.0), 0.0, cos(0.0));

    value += noise(p) * 0.5;
    value += noise(p*rotate*2.0) * 0.25 * vec4(1.0, 2.0, 2.0, 2.0);
    value += noise(p*rotate*rotate*4.0) * 0.126 * vec4(1.0, 4.0, 4.0, 4.0);
	value += noise(p*rotate*-8.0) * 0.06123 * vec4(1.0, 8.0, 8.0, 8.0);

	float amp = 0.03125f;
	float freq = 16.f;

	for (int i = 0; i < 6; ++i) {
		value += noise(p * freq) * amp * vec4(1.0, freq, freq, freq);
		freq *= 2.0;
		amp *= 0.5;
	}

	return value;
}

void main()
{
	vec3 p = gl_GlobalInvocationID.xyz;
	vec3 noiseInput = (p + (chunk * (texels - 1))) / (texels * 2.0);
	vec4 density = fbm(noiseInput);
	//vec4 density = vec4(length(p - vec3(16.f)) - 16.f, 0.f, 0.f, 0.f);
	//density.yzw = vec3(p - vec3(-16.f));
	imageStore(outTexture, ivec3(p), density);
}
