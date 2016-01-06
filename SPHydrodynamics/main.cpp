#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "SPH.h"
#include "GOBJ.h"
#include "ShaderObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

SPH * sph;
GOBJ * plane;
ShaderObject * shader;

string vsFilename = "vert-phong.vs.glsl";
string fsFilename = "frag-phong.fs.glsl";

// Transformation matrices
mat4 rotMatrix = mat4(1.0f);
mat4 scaleMatrix = mat4(1.0f);
mat4 transMatrix = mat4(1.0f);
float scalar = 1.0f;

int width = 1200;
int height = 900;

GLboolean isFunnel = false;

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader->getID());

	scaleMatrix = scale(mat4(1.0f), vec3(scalar)) * scaleMatrix;

	//Model View Matrix setup
	mat4 modelView = lookAt(vec3(0.0f, 7.0f, 20.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	if (isFunnel)
		modelView = lookAt(vec3(-0.7f, 8.0f, 1.0f), vec3(-0.7f, -7.0f, 0.9f), vec3(0.0f, 1.0f, 0.0f));

	modelView *= rotMatrix * scaleMatrix * transMatrix;
	glUniformMatrix4fv(shader->getViewPtr(), 1, GL_FALSE, value_ptr(modelView));
	glUniformMatrix4fv(shader->getMVPtr(), 1, GL_FALSE, value_ptr(mat4(1.0f)));


	//Projection Matrix setup
	mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	glUniformMatrix4fv(shader->getProjPtr(), 1, GL_FALSE, value_ptr(proj));

	glUniform3f(shader->getLightPos(), 0.0f, 20.0f, 10.0f);
	
	glUniform1f(shader->getSpecPowPtr(), 30.0f);
	glUniform3f(shader->getSpecAlbPtr(), 1.0f, 1.0f, 1.0f);
	glUniform3f(shader->getDiffusePtr(), 1.0f, 1.0f, 1.0f);
	glUniform3f(shader->getAmbientPtr(), 0.001f, 0.001f, 0.001f);
	//plane->draw(modelView, mv_ptr, view_ptr);

	glUniform3f(shader->getAmbientPtr(), 0.0f, 1.0f, 1.0f);
	sph->renderSPH(modelView, shader->getMVPtr(), shader->getViewPtr());

	glUniform1f(shader->getSpecPowPtr(), 30.0f);
	glUniform3f(shader->getSpecAlbPtr(), 0.1f, 0.1f, 0.1f);
	glUniform3f(shader->getDiffusePtr(), 0.3f, 0.3f, 0.3f);
	glUniform3f(shader->getAmbientPtr(), 0.001f, 0.001f, 0.001f);
	sph->drawSpheres(modelView, shader->getMVPtr(), shader->getViewPtr());
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(1.0f, 0.0f, 0.0f)) * rotMatrix;
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(-1.0f, 0.0f, 0.0f)) * rotMatrix;

	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(0.0f, 1.0f, 0.0f)) * rotMatrix;
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(0.0f, -1.0f, 0.0f)) * rotMatrix;
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		scalar = 1.1f;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		scalar = 1.0f;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		scalar = 1.0f / 1.1f;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		scalar = 1.0f;
}

int main(int argc, char **argv)
{
	GLFWwindow* window;

	if (argc != 2){
		cout << "Please enter a simulation type (waterfall or funnel)\n";
		return -1;
	}

	if (!glfwInit()){
		cout << "Couldn't initialize GLFW. \n";
		return 1;
	}

	shader = new ShaderObject();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Mass-Spring Demo", NULL, NULL);
	if (!window)
	{
		if (shader->getID()) {
			delete shader;
		}
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		printf("Couldn't initialize GLEW.\n");
		return -1;
	}

	plane = new GOBJ("floor.obj", vec3(0.0f, -5.0f, 0.0f), 1.0f);
	
	if (strcmp(argv[1], "waterfall") == 0){
		sph = new SPH("sphere.obj", "waterfall");
	}
	else if (strcmp(argv[1], "funnel") == 0){
		sph = new SPH("sphere.obj", "funnel");
		isFunnel = true;
	}
	else{
		cout << "Invalid simulation type\n";
		return -2;
	}
	
	shader->setupShaders(vsFilename, fsFilename);
	if (shader->getID()) {
		while (!glfwWindowShouldClose(window))
		{
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);

			render();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	delete sph;
	delete shader;
	delete plane;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
