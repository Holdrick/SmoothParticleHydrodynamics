#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

#define VERTEX_DATA 0
#define VERTEX_NORMAL 1

class ShaderObject{
public:

	ShaderObject();
	~ShaderObject();

	GLuint getID(){ return shaderID; }
	GLuint getMVPtr(){ return mv_ptr; }
	GLuint getViewPtr(){ return view_ptr; }
	GLuint getProjPtr(){ return proj_ptr; }
	GLuint getLightPos(){ return light_pos; }
	GLuint getAmbientPtr(){ return amb_ptr; }
	GLuint getDiffusePtr(){ return diff_ptr; }
	GLuint getSpecPowPtr(){ return spec_pow_ptr; }
	GLuint getSpecAlbPtr(){ return spec_alb_ptr; }
	GLuint getUniformLocation(const char * uniformID);

	void setupShaders(string vsFilename, string fsFilename);

private:

	bool loadShaderFile(const char *filename, GLuint shader);

	GLuint shaderID;
	GLuint mv_ptr, view_ptr, proj_ptr, light_pos;
	GLuint amb_ptr, diff_ptr, spec_pow_ptr, spec_alb_ptr;
};