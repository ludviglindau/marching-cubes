#version 460

in vec4 worldPos;
in vec4 worldNormal;

out vec4 fragColor;

void main()
{
	vec3 lightPos = vec3(0.0, -10.0, 0.0);
	vec3 normal = normalize(worldNormal.xyz);

	fragColor.rgb = vec3(0.0, 1.0, 0.0) * 0.25;
	fragColor.rgb += vec3(0.0, 1.0, 0.0) * max(dot(normalize(lightPos - worldPos.xyz), normalize(normal)), 0.0) * 1.0;

	//fragColor.rgb = normalize(worldNormal.xyz);
	//fragColor.rgb = abs(worldPos.xyz / 20.0);
	
	fragColor.a = 0.0;
}
