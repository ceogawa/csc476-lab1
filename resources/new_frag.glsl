#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif; // kd
uniform vec3 MatSpec; // ks
uniform float MatShine;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 EPos;

void main()
{
	
	//you will need to work with these for lighting
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	float dC = dot(normal, light); //represents N dot L, scalar to be multiplied by Kd

	vec3 L = normalize(light*-1.0); // because opposite direction of where light is pointing
	vec3 V = normalize(EPos);
	vec3 H = normalize(V + L); // this represents V + L / Magnitude(V + L)

	float x = dot(normal, H); // scalar to be multiplied by Ks

	color = vec4(MatAmb + dC*MatDif, 1.0); // adding in the ambient and diffusive light component
	color += vec4((pow(x, MatShine))*MatSpec, 1.0); // adding in the specular light component
}
