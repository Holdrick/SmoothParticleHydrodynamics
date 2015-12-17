#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "math.h"

using namespace std;
using namespace glm;


class GOBJ{
public:
  GOBJ(string filename, vec3 position, GLfloat scaleFactor);

	void readObj(string filename);
	void setupObj(string filename);
	void rearrangeData();
	void buffer();
	GLuint bufferSize();
	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	vec3 findMax();
	vec3 findMin();
	void findCenter();
	void findDiameter();
	void draw(mat4 view, GLuint model_ptr, GLuint view_ptr);
	void draw(vec3 ambient, GLuint amb);


	vector<GLuint> indices, normIndices, texIndices;
	vector<GLfloat> vertices, normals, texVertices;
	vec3 center;

	void setPosition(vec3 position){ this->position = position; }
	vec3 getPosition(){ return position; }

	void setScaleFactor(GLfloat scaleFactor){ this->scaleFactor = scaleFactor; }
	GLfloat getScaleFactor(){ return scaleFactor; }

	void setDiameter(GLfloat diameter){ this->diameter = diameter; }
	GLfloat getDiameter(){ return diameter; }
private:
	GLuint VAO, VBO;
	vec3 position;
	GLfloat scaleFactor, diameter;
};
