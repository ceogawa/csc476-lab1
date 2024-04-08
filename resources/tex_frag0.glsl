#version 330 core

uniform sampler2D Texture0;
uniform float MatShine;

uniform int flip;

in vec2 vTexCoord;

out vec4 Outcolor;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 EPos;

void main() {
  vec4 texColor0 = texture(Texture0, vTexCoord);

  vec3 normal = normalize(fragNor);
  if (flip < 1){
  	normal *= -1.0f;
  }
  vec3 light = normalize(lightDir);
  float dC = max(0, dot(normal, light));
  Outcolor = vec4(dC*texColor0.xyz, 1.0);

  //to confirm texture coordinates
  //Outcolor = vec4(vTexCoord.x, vTexCoord.y, 0, 0);
}



// #version 330 core
// uniform sampler2D Texture0;
// in vec2 vTexCoord;
// uniform int flip;

// in vec3 fragNor;
// in vec3 lightDir;
// in vec3 EPos;

// out vec4 Outcolor;

// void main() {
// 	vec4 texColor0 = texture(Texture0, vTexCoord);
// 	//uncomment later on: 
// 	Outcolor = texColor0;
// 	if(flip != 0){
		
// 		//you will need to work with these for lighting
// 		vec3 normal = normalize(fragNor);
// 		vec3 light = normalize(lightDir);
		
// 		float dC = max(dot(normal, light), 0);		

// 		Outcolor = dC * vec4(vec4(vTexCoord.s, vTexCoord.t, 0, 1));

// 		if(Outcolor.b > Outcolor.g && Outcolor.b > Outcolor.r || dC == 0 ){
// 			discard;
// 		}
// 	}
	
// }
// #version 330 core
// uniform sampler2D Texture0;

// in vec2 vTexCoord;
// //interpolated normal and light vector in camera space
// in vec3 fragNor;
// in vec3 lightDir;
// //position of the vertex in camera space
// in vec3 EPos;
// uniform int flip;


/////////////////////// #version 330 core
// uniform sampler2D Texture0;

// in vec2 vTexCoord;
// out vec4 Outcolor;

// void main() {
//   vec4 texColor0 = texture(Texture0, vTexCoord);

//   	//to set the out color as the texture color 
//   	Outcolor = texColor0;
  
//   	//to set the outcolor as the texture coordinate (for debugging)
// 	//Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
// }

// #version 330 core

// uniform sampler2D Texture0;
// uniform float MatShine;

// uniform int flip;

// in vec2 vTexCoord;

// out vec4 Outcolor;

// //interpolated normal and light vector in camera space
// in vec3 fragNor;
// in vec3 lightDir;
// //position of the vertex in camera space
// in vec3 EPos;

// void main() {
//   vec4 texColor0 = texture(Texture0, vTexCoord);

//   vec3 normal = normalize(fragNor);
//   if (flip < 1)
//   	normal *= -1.0f;
//   vec3 light = normalize(lightDir);
//   float dC = max(0, dot(normal, light));
//   Outcolor = vec4(dC*texColor0.xyz, 1.0);

//   //to confirm texture coordinates
//   //Outcolor = vec4(vTexCoord.x, vTexCoord.y, 0, 0);
// }

