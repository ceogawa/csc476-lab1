/*
 * Program 4 example with diffuse and spline camera PRESS 'g'
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn (spline D. McGirr)
 */

#include <iostream>
#include <glad/glad.h>
#include <chrono>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Bezier.h"
#include "Spline.h"
#include "particleSys.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	
	// Our shader program for particles
	std::shared_ptr<Program> partProg;

	// Our shader program - use this one for Blinn-Phong has diffuse
	std::shared_ptr<Program> prog;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	std::shared_ptr<Program> toonShader;

	//our geometry
	shared_ptr<Shape> sphere;
	
	shared_ptr<Shape> bunny;

	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;

	//the image to use as a texture (ground)
	shared_ptr<Texture> texture0;
	shared_ptr<Texture> texture1;	
	shared_ptr<Texture> texture2;
	// OpenGL handle to texture data used in particle
	shared_ptr<Texture> texturePart;



	//animation data
	float lightTrans = 0;
	
	vec3 gMin;
	int materialKey = 0;
	//camera
	double g_phi, g_theta;
	vec3 view = vec3(0, 0, 0);
	vec3 strafe = vec3(1, 0, 0);
	vec3 g_eye = vec3(0, 2, 4);
	vec3 g_lookAt = vec3(0, 1.8, 3);

	// vec3 g_lookAt = normalizeView(view,  vec3(0, 1, 0));
    // vec3 g_eye = normalizeView(view, vec3(0, 0, 1));
           

	float a = 0.0;

	float gRot = 0;
	float gCamH = -1.5;

	float f_b = 6;
	float l_r = 0;

	float sTheta = 0;

	float lastTime = 0;
	float curTime = 0;
	// float t = 0;


	vec3 upperArmLMin;
	vec3 upperArmLMax;
	vec3 upperArmLCenter;

	vec3 lowerArmLMin;
	vec3 lowerArmLMax;
	vec3 lowerArmLCenter;

	
	vec3 upperArmRMin;
	vec3 upperArmRMax;
	vec3 upperArmRCenter;

		
	vec3 lowerArmRMin;
	vec3 lowerArmRMax;
	vec3 lowerArmRCenter;


	// Repeat for other keyframes...


	vec3 lookAtPoint = vec3(0, 0, 0);
	vec3 cameraPosition = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

	


	Spline splinepath[2];
	bool goCamera = false;

	vec3 normalizeView(vec3 view, vec3 vec){
		return normalize(cross(view, vec));
	}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS){
			lightTrans += 0.5;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS){
			lightTrans -= 0.5;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
			goCamera = !goCamera;
			// updateUsingCameraPath(3);
		}
		if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
			materialKey ++;
		}
		if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
			g_lookAt += 0.3f * normalize(cross(view, vec3(0, 1, 0)));
            g_eye += 0.3f * normalize(cross(view, vec3(0, 1, 0)));
           
			view = normalize(g_lookAt - g_eye);
        }
        if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
			g_lookAt -= 0.3f * normalize(cross(view, vec3(0, 1, 0)));
            g_eye -= 0.3f * normalize(cross(view, vec3(0, 1, 0)));
			view = normalize(g_lookAt - g_eye);
        }
        if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
			g_lookAt += 0.3f * view;
            g_eye += 0.3f * view;
           
		   	view = normalize(g_lookAt - g_eye);
        }
        if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
			g_lookAt -= 0.3f * view;
            g_eye -= 0.3f * view;
        
			view = normalize(g_lookAt - g_eye);
        }
		

	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}


	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {
		// Adjust polar and azimuthal angles (phi and theta) based on mouse scroll input
		g_phi -= 0.3 * deltaY;
		g_theta += 0.3 * deltaX;

		// Clamp phi to avoid flipping the camera
		if (g_phi > 1.38) {
			g_phi = 1.38;
		} else if (g_phi < -1.38) {
			g_phi = -1.38;
		}

		// Calculate new camera look-at point based on phi, theta, and spherical radius
		float radius = 6.0f;  // Adjust the radius as needed
		float x = radius * cos(g_phi) * cos(g_theta);
		float y = radius * sin(g_phi);
		float z = radius * cos(g_phi) * sin(g_theta);

		g_lookAt = g_eye + vec3(x, y, z);
		view = normalize(g_lookAt - g_eye);

	}


	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		g_theta = -PI/2.0;

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/new_vert.glsl", resourceDirectory + "/new_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");

		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");

		prog->addUniform("lightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		// Initialize the GLSL program for TOON SHADING.
		toonShader = make_shared<Program>();
		toonShader->setVerbose(true);
		toonShader->setShaderNames(resourceDirectory + "/new_vert.glsl", resourceDirectory + "/toon_frag.glsl");
		toonShader->init();
		toonShader->addUniform("P");
		toonShader->addUniform("V");
		toonShader->addUniform("M");

		toonShader->addUniform("lightPos");
		toonShader->addUniform("lightColor");
		toonShader->addUniform("objectColor");
		
		toonShader->addAttribute("vertPos");
		toonShader->addAttribute("vertNor");
		toonShader->addAttribute("vertTex");

		// Initialize the GLSL program.
		partProg = make_shared<Program>();
		partProg->setVerbose(true);
		partProg->setShaderNames(
			resourceDirectory + "/lab10_vert.glsl",
			resourceDirectory + "/lab10_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("M");
		partProg->addUniform("V");
		partProg->addUniform("pColor");
		partProg->addUniform("alphaTexture");
		partProg->addAttribute("vertPos");


		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		texProg->addUniform("MatShine");
		texProg->addUniform("lightPos");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/cartoonWood.jpg");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/grey.jpg");
  		texture1->init();
  		texture1->setUnit(1);
 		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		 
  		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/cartoonWood.jpg");
  		texture2->init();
  		texture2->setUnit(2);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texturePart = make_shared<Texture>();
		texturePart->setFilename(resourceDirectory + "/alpha.bmp");
		texturePart->init();
		texturePart->setUnit(0);
		texturePart->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);


  		// init splines up and down
       splinepath[0] = Spline(glm::vec3(-6,0,5), glm::vec3(-1,-5,5), glm::vec3(1, 5, 5), glm::vec3(2,0,5), 5);
       splinepath[1] = Spline(glm::vec3(2,0,5), glm::vec3(3,-2,5), glm::vec3(-0.25, 0.25, 5), glm::vec3(0,0,5), 5);

    
	}

	void initGeom(const std::string& resourceDirectory)
	{
		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cube1.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			sphere = make_shared<Shape>(false);
			sphere->createShape(TOshapes[0]);
			sphere->measure();
			sphere->init();
		}

		vector<tinyobj::shape_t> bunnyShapes;
 		vector<tinyobj::material_t> bunnyMaterials;

		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(bunnyShapes, bunnyMaterials, errStr, (resourceDirectory + "/bunny.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			bunny = make_shared<Shape>(false);
			bunny->createShape(bunnyShapes[0]);
			bunny->measure();
			bunny->init();
		}

		initGround();

	}	

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 20;
		float g_groundY = -1;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }

      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
     	texture0->bind(curS->getUniform("Texture0"));
		//draw the ground plane 
  		SetModel(curS, vec3(0, -4, 0), 0, 0, 2, 2, 2);

  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

     //helper function to pass material data to the GPU
	void SetMaterial(int i) {

		glUniform3f(toonShader->getUniform("lightColor"), 1, 1, 1);

    	switch (i) {
    		case 0: // SILVER
    		    glUniform3f(toonShader->getUniform("objectColor"), 	0.75, 0.75, 0.75);
    		break;

			case 1: // ORANGE
				glUniform3f(toonShader->getUniform("objectColor"), 1.0, 0.65, 0.0);
			break;

			case 2: // YELLOW
    			glUniform3f(toonShader->getUniform("objectColor"), 1.00, 0.92, 0.59);
			break;

			case 3: // BROWN
				glUniform3f(toonShader->getUniform("objectColor"), 0.55, 0.27, 0.08);
			break;

			case 4: // PINK
				glUniform3f(toonShader->getUniform("objectColor"), 	1.000, 0.800, 0.898);
			break;
			
			case 5: // WHITE
    			glUniform3f(toonShader->getUniform("objectColor"), 	0.95, 0.95, 0.95);
			break;

			case 6: // PURPLE
    			glUniform3f(toonShader->getUniform("objectColor"), 0.33, 0.15, 0.5);
			break;

			case 7: // BLACK
    			glUniform3f(toonShader->getUniform("objectColor"), 0.0, 0.0, 0.0);
			break;
			
			case 8: // GREEN 
    			glUniform3f(toonShader->getUniform("objectColor"), 0.18, 0.55, 0.34);
			break;

			case 9: // BLUE 
    			glUniform3f(toonShader->getUniform("objectColor"), 0.529, 0.655, 0.667);
			break;
  		}
	}

  	void SetModel(shared_ptr<Program> curS, vec3 trans, float rotY, float rotX, float sx, float sy, float sz) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sx, sy, sz));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}
	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}

   	/* camera controls - do not change */
	void SetView(shared_ptr<Program>  shader) {
  		glm::mat4 Cam = glm::lookAt(g_eye, g_lookAt, vec3(0, 1, 0));
  		glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
	}


   	void updateUsingCameraPath(float frametime)  {

   	  if (goCamera) {
       if(!splinepath[0].isDone()){
       		splinepath[0].update(frametime);
            g_eye = splinepath[0].getPosition();
        } else {
            splinepath[1].update(frametime);
            g_eye = splinepath[1].getPosition();
        }
      }
   	}


	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		//update the camera position
		updateUsingCameraPath(frametime);

		lastTime = curTime;
		curTime = glfwGetTime();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// drawing orange//////////////////////////////////////////////////////////////////////////
		toonShader->bind();
		//send the projetion and view for solid shader
		glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(toonShader);
		glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);

		//orange
		SetMaterial(1);
		//use helper function that uses glm to create some transform matrices
		SetModel(toonShader, vec3(1.8, -.1, 1.6), 0, 0, 2, 2, 2);

		bunny->draw(toonShader);
	
		// Draw the doggos
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(texProg);
		glUniform3f(texProg->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		glUniform1f(texProg->getUniform("MatShine"), 27.9);
		//draw SKYBOX 
		glUniform1i(texProg->getUniform("flip"), 0);

		texture1->bind(texProg->getUniform("Texture0"));
		Model->pushMatrix();
			Model->loadIdentity();
			Model->translate(vec3(1, 0, -1));
			Model->scale(vec3(50, 18, 50));
			setModel(texProg, Model);
			sphere->draw(texProg);
		Model->popMatrix();

		glUniform1i(texProg->getUniform("flip"), 1);
		drawGround(texProg);

		Projection->popMatrix();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	auto lastTime = chrono::high_resolution_clock::now();
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();
		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

		// Render scene.
		application->render(deltaTime);
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
