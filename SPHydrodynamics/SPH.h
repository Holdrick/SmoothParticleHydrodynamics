#pragma once

#include "Particle.h"
#include "GOBJ.h"

const vec3 GRAVITY(0.0f, -0.005f, 0.0f);
const GLfloat SIGMA = 0.42f;
const GLuint ROWS = 12;
const GLuint COLS = 1;
const GLuint DEPTH = 12;
const GLuint SIZE = ROWS*COLS*DEPTH;
const GLfloat dt = 1.0f;

#define TWOPI 6.28318530717959

#define VERTEX_DATA 0

class SPH{
public:
	SPH(string filename, string config);
	~SPH();

	GLfloat kernel(vec3 p);
	vec3 kernelGradient(vec3 p);
	GLfloat density(GLint i);
	GLfloat pressure(GLint i);
	vec3 aDueToPressure(GLint i);
	vec3 aDueToViscosity(GLint i);
	vec3 aDueToCollision(GLuint i, GLuint circleIndex);

	void update();
	void draw();
	void renderSPH(mat4 view, GLuint model_ptr, GLuint view_ptr);
	void drawSpheres(mat4 view, GLuint model_ptr, GLuint view_ptr);

	void printVector(GLuint i, string tag);
	GLuint bufferSize();
	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	vector<GLfloat> makeVertices(vector<Particle*> parts);
	GLint getRandInt(GLint max);

private:
	vector<Particle*> particles;
	vector<GOBJ*> spheres;
	vector<GLfloat> vertices;
	GLuint VAO, VBO;
	vec3 centers[4];
	GLfloat radii[4];
	vector<vec3> spawns;
	GLboolean isFunnel;
	GOBJ * bowl;
};
