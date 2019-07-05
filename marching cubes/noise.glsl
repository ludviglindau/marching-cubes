#version 460
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image3D outTexture;

layout(location = 0) uniform float seed;
layout(location = 1) uniform float time;
layout(location = 2) uniform float texels;
layout(location = 3) uniform ivec3 chunk;

float hash( float n ) { return fract(sin(n)*753.5453123); }

vec3 random3(vec3 p)
{
    p = vec3(dot(p, vec3(905.0, 311.365, 320.79)),
              dot(p, vec3(684.63, 866.28, 95.22144554)),
              dot(p, vec3(174.62, 544.25, 151.8542)));
    return -1.0 + 2.0 * fract(sin(p) * 12583.3602648 + (seed * 155.155));
}

vec4 noised( in vec3 x )
{
    vec3 p = floor(x);
    vec3 w = fract(x);
	vec3 u = w*w*(3.0-2.0*w);
    vec3 du = 6.0*w*(1.0-w);
    
    float n = p.x + p.y*157.0 + 113.0*p.z;
    
    float a = hash(n+  0.0);
    float b = hash(n+  1.0);
    float c = hash(n+157.0);
    float d = hash(n+158.0);
    float e = hash(n+113.0);
	float f = hash(n+114.0);
    float g = hash(n+270.0);
    float h = hash(n+271.0);
	
    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

    return vec4( k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z, 
                 du * (vec3(k1,k2,k3) + u.yzx*vec3(k4,k5,k6) + u.zxy*vec3(k6,k4,k5) + k7*u.yzx*u.zxy ));
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
	p *= 1.5;
	vec4 value = vec4(p.y * 0.0, 0.0, 2.0 * 0.0, 0.0);

    value += noise(p) * 0.5;
    value += noise(p*2.0) * 0.25 * vec4(1.0, 2.1, 1.9, 2.0);
    value += noise(p*4.0) * 0.126 * vec4(1.0, 4.0, 4.1, 3.9);
	value += noise(p*8.0) * 0.123 * 0.5 * vec4(1.0, 7.8, 8.2, 8.0);
	value += noise(p*16.0) * 0.128 * 0.25 * vec4(1.0, 16.0, 16.0, 16.0);

	return value;
}

vec4 fbmd( in vec3 x )
{
    const float scale  = 1.5;

    float a = 0.0;
    float b = 0.5;
	float f = 1.0;
    vec3  d = vec3(0.0);
    for( int i=0; i<8; i++ )
    {
        vec4 n = noised(f*x*scale);
        a += b*n.x;           // accumulate values		
        d += b*n.yzw*f*scale; // accumulate derivatives
        b *= 0.5;             // amplitude decrease
        f *= 1.8;             // frequency increase
    }

	return vec4( a, d );
}

void main()
{
	vec3 p = gl_GlobalInvocationID.xyz;
	vec3 noiseInput = (p + (chunk * (texels - 1))) / (texels * 1.0);
	vec4 density = fbm(noiseInput);
	//density.x = (density.x - 0.4) * 0.7;
	density.yzw = normalize(density.yzw);
	imageStore(outTexture, ivec3(p), density);
}
