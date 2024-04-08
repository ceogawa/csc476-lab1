// Toon Shading - Fragment Shader
#version 330 core
out vec4 FragColor;

in vec3 fragNor;
in vec3 lightDir;

uniform vec3 lightColor;
uniform int increments = 4;
uniform vec3 objectColor;

void main()
{
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);

	float diffuse = dot(normal, light);
	float diffuseToon = max(ceil(diffuse * float(increments)) / float(increments), 0.0);

	vec3 toonColor = diffuseToon * lightColor * objectColor;

	FragColor = vec4(toonColor, 1.0);
}