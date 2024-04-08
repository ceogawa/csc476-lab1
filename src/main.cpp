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

	//the particle system
	particleSys *thePartSystem;

	//some particle variables
	float t = 0.0f; //reset in init
	float h = 0.01f;
	glm::vec3 g = glm::vec3(0.0f, -0.01f, 0.0f);


	//our geometry
	shared_ptr<Shape> sphere;

	shared_ptr<Shape> cube;

	shared_ptr<Shape> orange;

	shared_ptr<Shape> cheese;

	shared_ptr<Shape> banana;

	shared_ptr<Shape> apple;

	vector<shared_ptr<Shape>> kitchen;
	
	vector<shared_ptr<Shape>> knife;

	vector<shared_ptr<Shape>> rat;

	vector<shared_ptr<Shape>> chef;

	vector<shared_ptr<Shape>> platter;

	vector<shared_ptr<Shape>> noNorms;

	vector<shared_ptr<Shape>> table;

	// allows us to "instantiate" each keyframe
	using keyframe = vector<float>;

	// EACH INDEX represents a KEYFRAME
	vector<keyframe> frames;

	// vector<shared_ptr<Shape>> table1;

	vector<shared_ptr<Shape>> dummy;

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

	float interpFactor = 0.0f;
	// Define a variable to control the speed of interpolation
	float interpSpeed = 0.01f;
	// Define a flag to indicate whether interpolation is increasing or decreasing
	bool increasing = true;

	int curFrame = 0; 

	keyframe kf0;
	keyframe kf1;
	keyframe kf2;
	keyframe kf3;
	keyframe kf4;
	keyframe kf5;
	keyframe kf6;
	keyframe kf7;
	keyframe kf8;
	keyframe kf9;
	keyframe kf10;
	keyframe kf11;
	keyframe kf12;
	keyframe kf13;
	keyframe kf14;
	keyframe kf15;
	keyframe kf16;
	keyframe kf17;
	keyframe kf18;
	keyframe kf19;

	// Create shared pointers to keyframe objects
	shared_ptr<keyframe> ptr_kf0 = make_shared<keyframe>(kf0);
	shared_ptr<keyframe> ptr_kf1 = make_shared<keyframe>(kf1);
	shared_ptr<keyframe> ptr_kf2 = make_shared<keyframe>(kf2);
	shared_ptr<keyframe> ptr_kf3 = make_shared<keyframe>(kf3);
	shared_ptr<keyframe> ptr_kf4 = make_shared<keyframe>(kf4);
	shared_ptr<keyframe> ptr_kf5 = make_shared<keyframe>(kf5);

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
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			g_lookAt += 0.3f * normalize(cross(view, vec3(0, 1, 0)));
            g_eye += 0.3f * normalize(cross(view, vec3(0, 1, 0)));
           
			view = normalize(g_lookAt - g_eye);
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			g_lookAt -= 0.3f * normalize(cross(view, vec3(0, 1, 0)));
            g_eye -= 0.3f * normalize(cross(view, vec3(0, 1, 0)));
			view = normalize(g_lookAt - g_eye);
        }
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			g_lookAt += 0.3f * view;
            g_eye += 0.3f * view;
           
		   	view = normalize(g_lookAt - g_eye);
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
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

		thePartSystem = new particleSys(vec3(0, 0, 0));
		thePartSystem->gpuSetup();
	   
		// Add transforms for each component of the object to the keyframe
		// For example, adding a translation and rotation to the first component

		
		float t1head = 0.01;
		float t2head = -0.01;

		float t1hat = -.05;
		float t2hat = -.08;
		float t3hat = -.06;
		

		// head, hat
		
		kf0.push_back(t1head);
		kf0.push_back(0.0);

		kf1.push_back(t2head);
		kf1.push_back(0.0);

		kf2.push_back(t1head);
		kf2.push_back(0.0);

		kf3.push_back(t2head);
		kf3.push_back(0.0);

		kf4.push_back(t1head);
		kf4.push_back(0.0);

		kf5.push_back(t2head);
		kf5.push_back(0.0);

		kf6.push_back(t1head);
		kf6.push_back(0.0);

		kf7.push_back(t1head);
		kf7.push_back(0.0);

		kf8.push_back(t1head);
		kf8.push_back(0.0);

		kf9.push_back(t1head);
		kf9.push_back(t1hat);

		kf10.push_back(t1head);
		kf10.push_back(t2hat);

		kf11.push_back(t1head);
		kf11.push_back(t2hat);

		kf12.push_back(t1head);
		kf12.push_back(t2hat);

		kf13.push_back(t1head);
		kf13.push_back(t2hat);
		
		kf14.push_back(t1head);
		kf14.push_back(0.0);

		kf15.push_back(t1head);
		kf15.push_back(0.0);

		kf16.push_back(t1head);
		kf16.push_back(0.0);

		kf17.push_back(t2head);
		kf17.push_back(0.0);

		kf18.push_back(t1head);
		kf18.push_back(0.0);

		kf19.push_back(t1head);
		kf19.push_back(0.0);



		

		// body, hips, left leg, right leg

		for(int i = 1; i < 5; i ++){
			// Assuming keyframe is a vector of shared pointers to mat4
			// keyframe0.push_back(body);
			kf0.push_back(0.0);
			kf1.push_back(0.0);
			kf2.push_back(0.0);
			kf3.push_back(0.0);
			kf4.push_back(0.0);
			kf5.push_back(0.0);
			kf6.push_back(0.0);
			kf7.push_back(0.0);
			kf8.push_back(0.0);
			kf9.push_back(0.0);
			kf10.push_back(0.0);
			kf11.push_back(0.0);
			kf12.push_back(0.0);
			kf13.push_back(0.0);
			kf14.push_back(0.0);
			kf15.push_back(0.0);
			kf16.push_back(0.0);
			kf17.push_back(0.0);
			kf18.push_back(0.0);
			kf19.push_back(0.0);
		}

		// left upper arm, left lower arm, left hand
		// frames[frame][6], frames[frame][7], frames[frame][8]
		float t1Lupper = -.72;
		float t2Lupper = -.1;
		float t3Lupper = 1;
		float t4Lupper = 1;
		float t5Lupper = .9;
		float t6Lupper = .1;
		// float t2Lupper = 
		kf0.push_back(t1Lupper);
		kf1.push_back(t1Lupper);
		kf2.push_back(t1Lupper);
		kf3.push_back(t1Lupper);
		kf4.push_back(t1Lupper);
		kf5.push_back(t1Lupper);
		kf6.push_back(t2Lupper);
		kf7.push_back(t3Lupper);
		kf8.push_back(t3Lupper);
		kf9.push_back(t4Lupper);
		kf10.push_back(t4Lupper);
		kf11.push_back(t5Lupper);
		kf12.push_back(t5Lupper);
		kf13.push_back(t5Lupper);
		kf14.push_back(t4Lupper);
		kf15.push_back(t4Lupper);
		kf16.push_back(t5Lupper);
		kf17.push_back(t2Lupper);
		kf18.push_back(t1Lupper);
		kf19.push_back(t1Lupper);



		float t1Llower = 0;
		float t2Llower = .72;
		float t3Llower = 1.2;
		float t4Llower = 1.9;

		float t5Llower = 1.7;
		float t6Llower = 1.2;
		float t7Llower = 1.5;
		

		for(int i = 1; i<3; i++){
			kf0.push_back(t1Llower);
			kf1.push_back(t1Llower);
			kf2.push_back(t1Llower);
			kf3.push_back(t1Llower);
			kf4.push_back(t1Llower);
			kf5.push_back(t1Llower);
			kf6.push_back(t2Llower);
			kf7.push_back(t3Llower);
			kf8.push_back(t4Llower);
			kf9.push_back(t5Llower);
			kf10.push_back(t6Llower);
			kf11.push_back(t6Llower);
			kf12.push_back(t6Llower);
			kf13.push_back(t6Llower);
			kf14.push_back(t5Llower);
			kf15.push_back(t5Llower);
			kf16.push_back(t1Llower);
			kf17.push_back(t1Llower);
			kf18.push_back(t1Llower);
			kf19.push_back(t1Llower);

		}



		float t1Rupper = 1.3;
		// right upper arm

		kf0.push_back(t1Rupper);
		kf1.push_back(t1Rupper);
		kf2.push_back(t1Rupper);
		kf3.push_back(t1Rupper);
		kf4.push_back(t1Rupper);
		kf5.push_back(t1Rupper);
		kf6.push_back(t1Rupper);
		kf7.push_back(t1Rupper);
		kf8.push_back(t1Rupper);
		kf9.push_back(t1Rupper);
		kf10.push_back(t1Rupper);
		kf11.push_back(t1Rupper);
		kf12.push_back(t1Rupper);
		kf13.push_back(t1Rupper);
		kf14.push_back(t1Rupper);
		kf15.push_back(t1Rupper);
		kf16.push_back(t1Rupper);
		kf17.push_back(t1Rupper);
		kf18.push_back(t1Rupper);
		kf19.push_back(t1Rupper);

		float t1Rlower = 1.6;
		float t2Rlower = 2.2;

		
		// head, hat, body, hips, left leg, right leg, left upper arm, left lower arm, left hand, right upper arm, right lower+hand

		kf0.push_back(t1Rlower);
		kf1.push_back(t2Rlower);
		kf2.push_back(t1Rlower);
		kf3.push_back(t2Rlower);
		kf4.push_back(t1Rlower);
		kf5.push_back(t2Rlower);
		kf6.push_back(t2Rlower);
		kf7.push_back(t2Rlower);
		kf8.push_back(t2Rlower);
		kf9.push_back(t2Rlower);
		kf10.push_back(t2Rlower);
		kf11.push_back(t2Rlower);
		kf12.push_back(t2Rlower);
		kf13.push_back(t2Rlower);
		kf14.push_back(t1Rlower);
		kf15.push_back(t2Rlower);
		kf16.push_back(t1Rlower);
		kf17.push_back(t2Rlower);
		kf18.push_back(t1Rlower);
		kf19.push_back(t2Rlower);

		// RAT ROTATION	
		kf0.push_back(0.0);
		kf1.push_back(0.0);
		kf2.push_back(0.0);
		kf3.push_back(0.0);
		kf4.push_back(0.0);
		kf5.push_back(0.0);
		kf6.push_back(0.0);
		kf7.push_back(0.0);
		kf8.push_back(0.0);
		kf9.push_back(-0.3);
		kf10.push_back(0.3);
		kf11.push_back(-0.3);
		kf12.push_back(0.3);
		kf13.push_back(-0.3);
		kf14.push_back(0.3);
		kf15.push_back(0.0);
		kf16.push_back(0.0);
		kf17.push_back(0.0);
		kf18.push_back(0.0);
		kf19.push_back(0.0);

		// LEFT ARM
		float armDown = 1.2;
		float armTurning = .5;
		float armUp = 0.0;
		kf0.push_back(armDown);
		kf1.push_back(armDown);
		kf2.push_back(armDown);
		kf3.push_back(armDown);
		kf4.push_back(armDown);
		kf5.push_back(armDown);
		kf6.push_back(armTurning);
		kf7.push_back(armUp);
		kf8.push_back(armUp);
		kf9.push_back(armUp);
		kf10.push_back(armUp);
		kf11.push_back(armUp);
		kf12.push_back(armUp);
		kf13.push_back(armUp);
		kf14.push_back(armUp);
		kf15.push_back(armUp);
		kf16.push_back(armUp);
		kf17.push_back(armTurning);
		kf18.push_back(armDown);
		kf19.push_back(armDown);

		// LEFT ARM frames[cur][13]
		float still = 0.0;
		float chopping1 = 0.2;
		float chopping2 = 0.1;
		kf0.push_back(chopping1);
		kf1.push_back(chopping2);
		kf2.push_back(chopping1);
		kf3.push_back(chopping2);
		kf4.push_back(chopping1);
		kf5.push_back(chopping2);
		kf6.push_back(still);
		kf7.push_back(still);
		kf8.push_back(still);
		kf9.push_back(still);
		kf10.push_back(still);
		kf11.push_back(still);
		kf12.push_back(still);
		kf13.push_back(still);
		kf14.push_back(chopping1);
		kf15.push_back(chopping2);
		kf16.push_back(chopping1);
		kf17.push_back(chopping2);
		kf18.push_back(chopping1);
		kf19.push_back(chopping2);



		// Push frames into the vector
		frames.push_back(kf0);
		frames.push_back(kf1);
		frames.push_back(kf2);
		frames.push_back(kf3);
		frames.push_back(kf4);
		frames.push_back(kf5);
		frames.push_back(kf6);
		frames.push_back(kf7);
		frames.push_back(kf8);
		frames.push_back(kf9);
		frames.push_back(kf10);
		frames.push_back(kf11);
		frames.push_back(kf12);
		frames.push_back(kf13);
		frames.push_back(kf14);
		frames.push_back(kf15);
		frames.push_back(kf16);
		frames.push_back(kf17);
		frames.push_back(kf18);
		frames.push_back(kf19);
		
    
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

		vector<tinyobj::shape_t> orangeShapes;
		vector<tinyobj::material_t> orMaterials;
 		rc = tinyobj::LoadObj(orangeShapes, orMaterials, errStr, (resourceDirectory + "/Orange.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			orange = make_shared<Shape>(false);
			orange->createShape(orangeShapes[0]);
			orange->measure();
			orange->init();
		}

		
		vector<tinyobj::shape_t> cheeseShapes;
 		rc = tinyobj::LoadObj(cheeseShapes, objMaterials, errStr, (resourceDirectory + "/cheesetriangle.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			cheese = make_shared<Shape>(false);
			cheese->createShape(cheeseShapes[0]);
			cheese->measure();
			cheese->init();
		}

		vector<tinyobj::shape_t> bananaShapes;
 		rc = tinyobj::LoadObj(bananaShapes, objMaterials, errStr, (resourceDirectory + "/Banana.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			banana = make_shared<Shape>(false);
			banana->createShape(bananaShapes[0]);
			banana->measure();
			banana->init();
		}

		// Load in APPLE 
		vector<tinyobj::shape_t> appleShapes;
 		rc = tinyobj::LoadObj(appleShapes, objMaterials, errStr, (resourceDirectory + "/Apple.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			apple = make_shared<Shape>(false);
			apple->createShape(appleShapes[0]);
			apple->measure();
			apple->init();
		}


		vector<tinyobj::shape_t> cubeShapes;
 		rc = tinyobj::LoadObj(cubeShapes, objMaterials, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			cube = make_shared<Shape>(false);
			cube->createShape(cubeShapes[0]);
			cube->measure();
			cube->init();
		}


		//---------------------------------------------------------------------------
		vector<tinyobj::shape_t> knifeShapes;
 		rc = tinyobj::LoadObj(knifeShapes, objMaterials, errStr, (resourceDirectory + "/knife_obj.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> knifePart;
		
			for(int i = 0; i < knifeShapes.size(); i ++){
			
				knifePart = make_shared<Shape>(false);
				knifePart->createShape(knifeShapes[i]);
				knifePart->measure();
				knifePart->init();
				knife.push_back(knifePart);

			}
		}

		//---------------------------------------------------------------------------
		vector<tinyobj::shape_t> ratShapes;
 		rc = tinyobj::LoadObj(ratShapes, objMaterials, errStr, (resourceDirectory + "/rat_model.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> ratPart;
		
			for(int i = 0; i < ratShapes.size(); i ++){
			
				ratPart = make_shared<Shape>(false);
				ratPart->createShape(ratShapes[i]);
				ratPart->measure();
				ratPart->init();
				rat.push_back(ratPart);

			}
		}


		//---------------------------------------------------------------------------
		vector<tinyobj::shape_t> chefS;
 		rc = tinyobj::LoadObj(chefS, objMaterials, errStr, (resourceDirectory + "/chef_rotated.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> chefP;
		
			for(int i = 0; i < chefS.size(); i ++){
			
				chefP = make_shared<Shape>(false);
				chefP->createShape(chefS[i]);
				chefP->measure();
				chefP->init();
				chef.push_back(chefP);

			}
		}

		
		// //TODO use bounding boxes for hierarchical modeling
		// BoundingBox upperArmL = chef[8].calculateBoundingBox();
		upperArmLMin = chef[5]->min;
		upperArmLMax = chef[5]->max;
		upperArmLCenter = (upperArmLMin + upperArmLMax)/2.0f;

		lowerArmLMin = chef[6]->min;
		lowerArmLMax = chef[6]->max;
		lowerArmLCenter = (lowerArmLMin + lowerArmLMax)/2.0f;

		upperArmRMin = chef[10]->min;
		upperArmRMax = chef[10]->max;
		upperArmRCenter = (upperArmRMin + upperArmRMax)/2.0f;

		
		lowerArmRMin = chef[9]->min;
		lowerArmRMax = chef[9]->max;
		lowerArmRCenter = (lowerArmRMin + lowerArmRMax)/2.0f;

				


		//-------------------------------------------------------------------------------
		//load IN PLATTER
		vector<tinyobj::shape_t> platterShapes;
 		rc = tinyobj::LoadObj(platterShapes, objMaterials, errStr, (resourceDirectory + "/Grapes_and_Orange_Slices_OBJ.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> platterPart;
		
			for(int i = 0; i < platterShapes.size(); i ++){
			
				platterPart = make_shared<Shape>(false);
				platterPart->createShape(platterShapes[i]);
				platterPart->measure();
				platterPart->init();
				platter.push_back(platterPart);

			}
		}

		//-------------------------------------------------------------------------------
		//load IN KITCHEN
		vector<tinyobj::shape_t> kitchenShapes;
 		rc = tinyobj::LoadObj(kitchenShapes, objMaterials, errStr, (resourceDirectory + "/Kitchen.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> kitchenPart;
		
			for(int i = 0; i < kitchenShapes.size(); i ++){
			
				kitchenPart = make_shared<Shape>(false);
				kitchenPart->createShape(kitchenShapes[i]);
				kitchenPart->measure();
				kitchenPart->init();
				kitchen.push_back(kitchenPart);

			}
		}
		//-------------------------------------------------------------------------------
		//load IN NO NORMALS
		vector<tinyobj::shape_t> nS;
 		rc = tinyobj::LoadObj(nS, objMaterials, errStr, (resourceDirectory + "/icoNoNormals.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> nP;
		
			for(int i = 0; i < nS.size(); i ++){
			
				nP = make_shared<Shape>(false);
				nP->createShape(nS[i]);
				nP->measure();
				nP->init();
				noNorms.push_back(nP);

			}
		}
		//-------------------------------------------------------------------------------
		//load IN TABLE
		vector<tinyobj::shape_t> tableShapes;
 		rc = tinyobj::LoadObj(tableShapes, objMaterials, errStr, (resourceDirectory + "/tableobj.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> tablePart;
			//for now all our shapes will not have textures - change in later labs
			for(int i = 0; i < tableShapes.size(); i ++){
			
				tablePart = make_shared<Shape>(false);
				tablePart->createShape(tableShapes[i]);
				tablePart->measure();
				tablePart->init();
				table.push_back(tablePart);

			}
		}

		//-------------------------------------------------------------------------------
		//load IN DUMMY
		vector<tinyobj::shape_t> dummyShapes;
 		rc = tinyobj::LoadObj(dummyShapes, objMaterials, errStr, (resourceDirectory + "/dummy.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			shared_ptr<Shape> dummyPart;
			//for now all our shapes will not have textures - change in later labs
			for(int i = 0; i < dummyShapes.size(); i ++){
			
				dummyPart = make_shared<Shape>(false);
				dummyPart->createShape(dummyShapes[i]);
				dummyPart->measure();
				dummyPart->init();
				dummy.push_back(dummyPart);

			}
		}


		//read out information stored in the shape about its size - something like this...
		//then do something with that information....
		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 20;
		float g_groundY = -0.25;

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

	float linearInterp(float x, float y, float t) {
		return x + t * (y - x);
	}

	void drawChef(shared_ptr<MatrixStack> Model, float interpFactor){
	
		// hierarchical chef
		toonShader->bind();

		if (curFrame > frames.size() - 1) {
			curFrame = 0; // Loop back to the beginning
		}

		int nextFrame = curFrame + 1;
		if(nextFrame > frames.size()-1){
			nextFrame = 0;
		}
		cout << "cur frame" << curFrame << endl;
		cout << "next frame" << nextFrame << endl;


	
		float interpHead = mix(frames[curFrame][0], frames[nextFrame][0], interpFactor);
		float interpHat = mix(frames[curFrame][1], frames[nextFrame][1], interpFactor);
		// float interpHat = mix(kf0[0], kf1[0], interpFactor);
		// float interpChop = mix(kf0[10], kf1[10], interpFactor*1.2);
		float interpChop = mix(frames[curFrame][10], frames[nextFrame][10], interpFactor);
		cout << "interp factor " << interpFactor << endl;
		cout << "interp hat " << interpHead << endl;

		cout << "num frames" << frames.size() << endl;

		Model->pushMatrix();
		// head, hat, body, hips, left leg, right leg, left upper arm, left lower arm, left hand
			// Model->loadIdentity();
			// apply transforms to all components
			Model->translate(vec3(0, -3, -3));
			Model->scale(vec3(1, 1, 1));

			// draw chef head and hat hat, WHITE
			Model->pushMatrix();
				Model->rotate(interpHead, vec3(0, 0, 1));
				setModel(toonShader, Model);
				SetMaterial(4);
				chef[0]->draw(toonShader);

				Model->pushMatrix();
					float ratRot = mix(frames[curFrame][11], frames[nextFrame][11], interpFactor);
					Model->translate(vec3(.05, 6, 0.2));
					Model->rotate(ratRot, vec3(0, 0, 1));
					Model->scale(vec3(.06, .06, .06));
					SetMaterial(5);
					setModel(toonShader, Model);
					for(int i = 0; i < rat.size(); i ++){
						rat[i]->draw(toonShader);
					}
				Model->popMatrix();

				Model->pushMatrix();
					
					Model->translate(vec3(7, 0, 0));
					Model->rotate(interpHat, vec3(0, 0, 1));
					// Model->translate(vec3(0, interpHeight, 0));
					Model->translate(vec3(-7, 0, 0));
				
					setModel(toonShader, Model);
					SetMaterial(5);
					chef[11]->draw(toonShader);	
			
				Model->popMatrix();

			Model->popMatrix();



			// draw body, head, legs, hips
			Model->pushMatrix();	
				Model->rotate(interpHead, vec3(0, 0, 1));
				SetMaterial(4);
				setModel(toonShader, Model);
				for(int i = 1; i < 5; i ++){
					chef[i]->draw(toonShader);
				}
			Model->popMatrix();
		
			// arms
			// Model->pushMatrix();
	
			SetMaterial(4);
			// draw left arm at side of body
			Model->pushMatrix();

				float LarmRot = mix(frames[curFrame][12], frames[nextFrame][12], interpFactor);

				Model->translate(vec3(upperArmLCenter.x-.3, upperArmLCenter.y - .45, upperArmLCenter.z));
				// 4. translate back to center for reference
				Model->translate(vec3(0, 0, -.2));

				float interpLUpperArm = mix(frames[curFrame][6], frames[nextFrame][6], interpFactor);
				Model->translate(vec3(0, (interpLUpperArm+.6)*.7, 0));
				
				Model->rotate(interpLUpperArm, vec3(0, 0, 1));
				Model->rotate(interpHead, vec3(0, 0, 1));
				Model->rotate(LarmRot, vec3(1, 0, 0));
				// 2. trans to pivot point (shoulder)
				Model->translate(vec3(0, 0, .2));
				Model->translate(vec3(-1.0f * upperArmLCenter.x, -1.0f * upperArmLCenter.y, -1.0f * upperArmLCenter.z));
				setModel(toonShader, Model);
				
				chef[5]->draw(toonShader);

				Model->pushMatrix();
					float interpLLowerArm = mix(frames[curFrame][7], frames[nextFrame][7], interpFactor);
					Model->translate(vec3(lowerArmLCenter.x, lowerArmLCenter.y, lowerArmLCenter.z));
					Model->translate(vec3(-0.2, 0, 0));
					Model->rotate(interpLLowerArm, vec3(0, 0, 1));
					Model->translate(vec3(0.2, 0, 0));
					Model->translate(vec3(-1.0f * lowerArmLCenter.x, -1.0f * lowerArmLCenter.y, -1.0f * lowerArmLCenter.z));
					setModel(toonShader, Model);
					chef[6]->draw(toonShader);
					chef[7]->draw(toonShader);

				Model->popMatrix();
		

			Model->popMatrix();

			// draw right arm 
			Model->pushMatrix();

				// draw right upper arm
				// 5. translate back to original position
				Model->translate(vec3(upperArmRCenter.x, upperArmRCenter.y, upperArmRCenter.z));
				// 4. translate back to center for reference
				Model->translate(vec3(.26, -.4, 0));
				// 3. rotate <30 degrees ccw
				Model->rotate(.9, vec3(0, 0, 1));
				// 2. trans to pivot point (shoulder)
				Model->translate(vec3(-.2, 0, 0));
				// 1. trans to center
				Model->translate(vec3(-1.0*upperArmRCenter.x, -1.0*upperArmRCenter.y, -1.0*upperArmRCenter.z));

				setModel(toonShader, Model);

				chef[10]->draw(toonShader);


				// float ratio = float(t-t1)/float(t2 - t1);

				Model->pushMatrix();
					Model->translate(vec3(lowerArmRCenter.x+0.5, lowerArmRCenter.y, lowerArmRCenter.z-.25));
					// pose 1

					Model->rotate(interpChop, vec3(0, 1, 0));
			
					Model->translate(vec3(-1.0*lowerArmRCenter.x -.5, -1.0*lowerArmRCenter.y, -1.0*lowerArmRCenter.z));

					// pose 1	
					Model->pushMatrix();
						SetMaterial(0);
						Model->translate(vec3(-3.65, 3.89, .08));
						Model->scale(vec3(0.09, 0.09, 0.09));
						// Model->rotate(-1.42, vec3(0, 1, 0));
						Model->rotate(3.14, vec3(1, 0, 0));

						SetMaterial(0);
						setModel(toonShader, Model);
						for(int i = 0; i < knife.size(); i ++){
							knife[i]->draw(toonShader);
						}
					Model->popMatrix();


					SetMaterial(4);
					
					setModel(toonShader, Model);
					chef[8]->draw(toonShader);
					chef[9]->draw(toonShader);
	
				Model->popMatrix();
		
			Model->popMatrix();

		Model->popMatrix();

		// draw cheese

		Model->pushMatrix();
			float interpCheese = mix(frames[curFrame][13], frames[nextFrame][13], interpFactor);
			Model->translate(vec3(-1.1, 0, 0));
			Model->rotate(interpCheese, vec3(0, 1, 0));
			Model->rotate(interpCheese, vec3(1, 0, 0));
			Model->scale(vec3(0.05, 0.05, 0.05));
			SetMaterial(2);
			setModel(toonShader, Model);
			platter[1]->draw(toonShader);
		Model->popMatrix();


		toonShader->unbind();

		// curFrame ++;

		// if (interpFactor >= 1.0f) {
		// 		// Move to the next frame
		// 	curFrame++;
		// 	// Reset interpolation factor to start interpolating the next frame
		// 	interpFactor = 0.0f;


		// }


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
	
		// Draw the doggos
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(texProg);
		glUniform3f(texProg->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		glUniform1f(texProg->getUniform("MatShine"), 27.9);
		glUniform1i(texProg->getUniform("flip"), 1);
		texture2->bind(texProg->getUniform("Texture0"));
		Model->pushMatrix();

		//draw SKYBOX 
		glUniform1i(texProg->getUniform("flip"), 0);
		texture1->bind(texProg->getUniform("Texture0"));
		Model->pushMatrix();
			Model->loadIdentity();
			Model->translate(vec3(1, 0, -1));
			Model->scale(vec3(12, 17, 12));
			setModel(texProg, Model);
			sphere->draw(texProg);
		Model->popMatrix();

		glUniform1i(texProg->getUniform("flip"), 1);
		drawGround(texProg);

		texProg->unbind();

		// drawing orange//////////////////////////////////////////////////////////////////////////
		toonShader->bind();
		//send the projetion and view for solid shader
		glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(toonShader);
		glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);

		//orange
		SetMaterial(1);
		//use helper function that uses glm to create some transform matrices
		SetModel(toonShader, vec3(1.8, -.1, 1.6), 0, 0, 0.008, 0.008, 0.008);

		orange->draw(toonShader);

		// drawing banana/////////////////////////////////////////////////////////////////////////////
		//send the projection and view for solid shader
		glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(toonShader);
		glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		
		// yellow
		SetMaterial(2);
		//use helper function that uses glm to create some transform matrices
		SetModel(toonShader, vec3(2.4, 0.05, 2), 0.2, 0, 0.009, 0.009, 0.009);
		banana->draw(toonShader);

		//draw  table////////////////////////////////////////
		//send the projection and view for solid shader
		glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(toonShader);
		glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		

		if(materialKey % 2 == 0){
			SetMaterial(3);
			glUniform3f(toonShader->getUniform("lightColor"), 1, 1, 1);
    		glUniform3f(toonShader->getUniform("objectColor"), 0.55, 0.27, 0.08);
			
		}
		else{
			SetMaterial(0);
		}

		SetModel(toonShader, vec3(0, -0.4, -1), 0, 0, 0.1, 0.2, 0.1);
		for(int i = 0; i < 25; i ++){
			table[i]->draw(toonShader);
		}


		// 	// ........................................................................
	 	// DRAWING KNIFE
		//send the projection and view for solid shader
		
		Model->pushMatrix();
			Model->translate(vec3(-.5, 1, 0));
			Model->translate( vec3(0,  -1.0 * abs(1.7*sin(sTheta*.9)), abs(1.2*sin(sTheta)) ));	
			Model->rotate(.3, vec3(1, 0, 0));
			Model->rotate(1.6, vec3(0, 1, 0));
			Model->scale(vec3(0.08, 0.09, 0.09));


		SetMaterial(0);
		setModel(toonShader, Model);

		for(int i = 0; i < knife.size(); i ++){
			// knife[i]->draw(toonShader);
		}

		Model->popMatrix();

		Model->pushMatrix();

			Model->translate(vec3(-2, -4.5, -2));
			// Model->translate( vec3(0,  -1.0 * abs(1.7*sin(sTheta*.9)), abs(1.2*sin(sTheta)) ));	
			Model->rotate(3.14, vec3(0, 1, 0));
			// Model->rotate(1.6, vec3(0, 1, 0));
			Model->scale(vec3(2, 2, 2));
		
		SetMaterial(0);
		setModel(toonShader, Model);
		for (int i = 0; i < kitchen.size(); i++) {
			if((i < 30 && i != 23)|| (i > 43)){

				if(i == 2 || i == 8 || i == 12 || i == 21 || i == 22 || i == 17 || i == 25 || i ==53 || i == 58 || i == 61){
					SetMaterial(5);
					// white
				}
				else if( i == 6 || i == 7 || i == 9 || i == 10 || i == 15 || i == 16 || i == 19 || i == 20 || i == 24){
					SetMaterial(3);
					// brown
				}
				else if(i == 26 || i == 29 || i == 46 || i == 48){
					SetMaterial(2);
				}
				else if(i == 27 || i == 54 || i == 56){
					SetMaterial(6);
					// purple
				}
				else if(i == 45 || i == 47 ){
					SetMaterial(1);
				}
				else if(i == 50){
					SetMaterial(4);
				}
				else if(i == 51 || i == 52){
					SetMaterial(8);
				}
				else if( i == 0 || i == 1 || i == 4 || i == 11 || i == 14 || i == 18|| i == 17 || i == 21){
					SetMaterial(9);
					// blue
				}
				else if(i == 28){
					SetMaterial(7);
					// black
				}
				else{
					SetMaterial(0);
					
				}

				if(i != 24){
					kitchen[i]->draw(toonShader);
				}
			}
		}

			Model->pushMatrix();
				Model->translate(vec3(-3, -.41, 0));
				setModel(toonShader, Model);
				kitchen[24]->draw(toonShader);
			Model->popMatrix();
	
		Model->popMatrix();

		// drawing CHEESE //////////////////////////////////////////////////////////////////////////

		//send the projetion and view for solid shader
		Model->pushMatrix();
			glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			SetView(toonShader);
			glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
			SetMaterial(2);
			//use helper function that uses glm to create some transform matrices
			Model->translate(vec3(-.8, 0, 0));
			Model->scale(vec3(0.05, 0.05, 0.05));

			setModel(toonShader, Model);

			cheese->draw(toonShader);
		Model->popMatrix();


		
		//........................................................................
		// DRAWING PLATTER
				//send the projection and view for solid shader
		glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(toonShader);
		glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		
	
		Model->pushMatrix();
			Model->translate(vec3(1.5, -.35, -.5));
			Model->rotate(1.6, vec3(0, 1, 0));

			Model->scale(vec3(0.06, 0.06, 0.06));
		// Model->rotate(.72, vec3(0, 1, 0));
			// SetModel(prog, vec3(-1, -.3, -.5), 1.6, 0, .06, .06, .06);
			setModel(toonShader, Model);
			SetMaterial(0);
			platter[0]->draw(toonShader);
			SetMaterial(2);
			platter[1]->draw(toonShader);
			SetMaterial(6);
			platter[2]->draw(toonShader);

		Model->popMatrix();

		
		//........................................................................
		// DRAWING NO NORMS
		//send the projection and view for solid shader
				//send the projection and view for solid shader
		glUniformMatrix4fv(toonShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(toonShader);
		glUniform3f(toonShader->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		
		Model->pushMatrix();
			Model->translate(vec3(0, 4, 0));
			Model->scale(vec3(.8, .3, .8));

			SetMaterial(5);
			setModel(toonShader, Model);

			noNorms[0];

			for(int i = 0; i < noNorms.size(); i ++){	
				noNorms[i]->draw(toonShader);
			}

		Model->popMatrix();

		toonShader->unbind();

		interpFactor += 0.038;
		if(interpFactor >= 1.0f){
			interpFactor = 0.0;
			curFrame ++;
		}
		drawChef(Model, interpFactor);

		// partProg->bind();
		
		// texturePart->bind(partProg->getUniform("alphaTexture"));
		// glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		// SetView(partProg); 
		// CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.9, 0.7, 0.7));
		
		// thePartSystem->drawMe(partProg);
		// thePartSystem->update();

		// partProg->unbind();
		


		sTheta = sin(glfwGetTime());

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
