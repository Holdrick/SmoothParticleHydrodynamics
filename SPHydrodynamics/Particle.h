#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Particle
{
public:

	inline Particle(){
		a = v = vec3(0.0f);
	}
	inline Particle(vec3 position){
		a = v = vec3(0.0f);
		this->p = position;
	}

	vec3 p;
	vec3 v;
	vec3 a;
	GLfloat density, pressure;
};
