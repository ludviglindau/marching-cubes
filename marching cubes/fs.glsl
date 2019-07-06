#version 450

in vec4 worldPos;
in vec4 worldNormal;

out vec4 fragColor;

layout(location = 1) uniform vec3 cameraPos;

void main()
{
	vec3 lightPos = vec3(0.0, 100.0, 0.0);
	vec3 lightDir = normalize(lightPos - worldPos.xyz);
	vec3 normal = normalize(worldNormal.xyz);
	vec3 color = vec3(0.7, 0.35, 0.35);
	fragColor.rgb = color * 0.25;
	fragColor.rgb += color * max(dot(lightDir, normal), -.15) * 1.0;
	if (normal.y > -0.2) fragColor.rgb += vec3(0.25, 0.6, 0.6);
	vec3 halfway = mix(lightDir, normalize(cameraPos - worldPos.xyz), 0.5);
	fragColor.rgb += vec3(1.0, 1.0, 1.0) * pow(max(dot(halfway, normal), 0.0), 50);
	
	//fragColor.rgb += vec3(0.5, 1.0, 1.0) * max(normal.y, 0.0);
	
	//fragColor.rgb = normal;
	//fragColor.rgb = abs(worldPos.xyz / 20.0);

	clamp(fragColor.rgb, 0.0, 1.0);
	fragColor.a = 0.0;
}
