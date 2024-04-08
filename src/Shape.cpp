#include "Shape.h"
#include <iostream>
#include <assert.h>

#include "GLSL.h"
#include "Program.h"

using namespace std;
using namespace glm;

Shape::Shape(bool textured) :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0), 
   vaoID(0)
{
	min = glm::vec3(0);
	max = glm::vec3(0);
	texOff = !textured;
}

Shape::~Shape()
{
}

/* copy the data from the shape to this object */
void Shape::createShape(tinyobj::shape_t & shape)
{
		posBuf = shape.mesh.positions;
		norBuf = shape.mesh.normals;
		texBuf = shape.mesh.texcoords;
		eleBuf = shape.mesh.indices;
}

void Shape::measure() {
  float minX, minY, minZ;
   float maxX, maxY, maxZ;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
   max.x = maxX;
   max.y = maxY;
   max.z = maxZ;
}

void Shape::init(){

	// Initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Send the position array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &posBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW));

	// Send the normal array to the GPU
	if (norBuf.empty())
	{
		float x, y, z;
		int v0_index, v1_index, v2_index;
		vec3 n;

		norBuf.clear();
		norBuf.resize(posBuf.size(), 0.0);
		// make sure to "clear" any data that could've been in the norBuf/initialize each index to 0


		for(size_t i = 0; i < this->eleBuf.size(); i += 3){
			// these represent the indices of the vertices in posBuf that we need to retrieve the actual vertices
			v0_index = this->eleBuf[i];
			v1_index = this->eleBuf[i+1];
			v2_index = this->eleBuf[i+2];

			// indexes into position buffer (vertices) with the indices retrieved from element buffer (faces)
			vec3 v0 = vec3(this->posBuf[3*v0_index], this->posBuf[1 + 3*v0_index], this->posBuf[2 + 3*v0_index]);
			vec3 v1 = vec3(this->posBuf[3*v1_index], this->posBuf[1 + 3*v1_index], this->posBuf[2 + 3*v1_index]);
			vec3 v2 = vec3(this->posBuf[3*v2_index], this->posBuf[1 + 3*v2_index], this->posBuf[2 + 3*v2_index]);

			//helps us find e1 and e2 that have to be crossed
			vec3 e1 = v1 - v0;
			vec3 e2 = v2 - v1;

			vec3 normal = cross(e2, e1);
			// cross and normalize e2 x e1
			normal = (normalize(normal));

			// increment the normal value at the corresponding index of the vertex we are calculating the normal of
			this->norBuf[3*v0_index] += normal.x;
			this->norBuf[3*v0_index + 1] += normal.y;
			this->norBuf[3*v0_index + 2] += normal.z;
			//v1	
			this->norBuf[3*v1_index] += normal.x;
			this->norBuf[3*v1_index + 1] += normal.y;
			this->norBuf[3*v1_index + 2] += normal.z;

			//v2
			this->norBuf[3*v2_index] += normal.x;
			this->norBuf[3*v2_index + 1] += normal.y;
			this->norBuf[3*v2_index + 2] += normal.z;

		}

		// cannot just normalize each vertex, must normalize entire norBuf (normals)

		for(size_t i = 0; i < this->norBuf.size(); i += 3)
		{	
			x = this->norBuf[i];
			y = this->norBuf[i+1];
			z = this->norBuf[i+2];

			n = normalize(vec3(x, y, z));
			
			this->norBuf[i] = n.x;
			this->norBuf[i+1] = n.y;
			this->norBuf[i+2] = n.z;
		}
	}

	// must enter these gl calls regardless of if id
	
	CHECKED_GL_CALL(glGenBuffers(1, &norBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW));


	// Send the texture array to the GPU
	if (texBuf.empty())
	{
		texBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &texBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW));
	}

	// Send the element array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &eleBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW));

	// Unbind the arrays
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
// {
//    // Initialize the vertex array object
//    glGenVertexArrays(1, &vaoID);
//    glBindVertexArray(vaoID);

// 	// Send the position array to the GPU
// 	glGenBuffers(1, &posBufID);
// 	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
// 	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
// 	// Send the normal array to the GPU
// 	if(norBuf.empty()) {
// 		// norBufID = 0;
// 		int v0_i, v1_i, v2_i;
// 		float x, y, z;
// 		vec3 v0, v1, v2,e1, e2, e3;

// 		// replaced NORBUF ID = 0
// 		for(int i = 0; i < this->posBuf.size(); i++){
// 			this->norBuf.push_back(0.0);
// 			// add zero to each position of the normal buffer
// 		}

// 		// iterate through the array of FACES to extract the vertices we want to normalize
// 		for(int i = 0; i < eleBuf.size(); i+=3){
			
// 			v0_i = eleBuf[i];
// 			v1_i = eleBuf[i + 1];
// 			v2_i = eleBuf[i + 2];


// 			// then for each vertex index, vn_i, we extract the corresponding vertex from the vertex buffer
// 			v0 = vec3(posBuf[3*v0_i],
// 						posBuf[1 + 3*v0_i],
// 						posBuf[2 + 3*v0_i]);

// 			v1 = vec3(posBuf[3*v1_i],
// 						posBuf[1 + 3*v1_i],
// 						posBuf[2 + 3*v1_i]);

// 			v2 = vec3(posBuf[3*v2_i],
// 						posBuf[1 + 3*v2_i],
// 						posBuf[2 + 3*v2_i]);

// 			e1 = v1-v0;
// 			e2 = v2-v0;

// 			vec3 n = cross(e1, e2);
// 			n = normalize(n);

	
// 			this->norBuf[3*v0_i]    += n.x;
// 			this->norBuf[3*v0_i + 1] += n.y;
// 			this->norBuf[3*v0_i + 2] += n.z;

			
// 			this->norBuf[3*v1_i]    += n.x;
// 			this->norBuf[3*v1_i + 1] += n.y;
// 			this->norBuf[3*v1_i + 2] += n.z;

// 			this->norBuf[3*v2_i]    += n.x;
// 			this->norBuf[3*v2_i + 1] += n.y;
// 			this->norBuf[3*v2_i + 2] += n.z;

// 		}

// 		for (int j = 0; j < norBuf.size(); ++j){
// 			x = this->norBuf[j];
// 			y = this->norBuf[j+1];
// 			z = this->norBuf[j+2];

// 			vec3 n1 = normalize(vec3(x, y, z));
// 			// now we store the average normal into the normal buffer
// 			this->norBuf[j] = n1.x;
// 			this->norBuf[j+1] = n1.y;
// 			this->norBuf[j+2] = n1.z;
			
		
// 		}

// 		glGenBuffers(1, &norBufID);
// 		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
// 		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

// 	}
	
// 	// Send the texture array to the GPU - for now no textures
// 	if(texBuf.empty() || texOff) {
// 		texBufID = 0;
// 	} else {
// 		glGenBuffers(1, &texBufID);
// 		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
// 		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
// 	}
	
// 	// Send the element array to the GPU
// 	glGenBuffers(1, &eleBufID);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
	
// 	// Unbind the arrays
// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
// 	assert(glGetError() == GL_NO_ERROR);
// }

//always untextured for intro labs until texture mapping
void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

   glBindVertexArray(vaoID);
	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if(h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}

	if (texBufID != 0) {	
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");
		if(h_tex != -1 && texBufID != 0) {
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}
	
	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	
	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
	
	// Disable and unbind
	if(h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
