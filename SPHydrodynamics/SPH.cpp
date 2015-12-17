#include "SPH.h"

SPH::SPH(string filename, string config){
	for(GLuint i = 0; i < ROWS; i++){
		for(GLuint j = 0; j < COLS; j++){
			for(GLuint k = 0; k < DEPTH; k++){
				vec3 pos = vec3(0.05f * i, 0.25f * j + 7.0f, 0.05f * k);
				particles.push_back(new Particle(pos));
				spawns.push_back(pos);
			}
		}
	}

	if (config == "waterfall"){
		centers[0] = vec3(2.0f, 0.0f, 0.0f);
		centers[1] = vec3(-4.0f, -2.0f, 0.0f);
		centers[2] = vec3(0.0f, 3.0f, -2.75f);
		centers[3] = vec3(-1.0f, -7.0f, 2.5f);
		isFunnel = false;
	}
	else if (config == "funnel"){
		centers[0] = vec3(2.2f, 3.0f, 0.5f);
		centers[1] = vec3(-3.8f, 3.0f, 1.5f);
		centers[2] = vec3(0.0f, 3.0f, -2.0f);
		centers[3] = vec3(0.0f, 3.0f, 5.0f);
		isFunnel = true;
		bowl = new GOBJ("bowl.obj", vec3(0.0f, -7.1f, 0.0f), 1.0f);
	}

	for (GLuint i = 0; i < 4; i++){
		radii[i] = 3.0f;
		spheres.push_back(new GOBJ(filename, centers[i], 1.0f));
	}

	for (GLuint i = 0; i < spheres.size(); i++){
		GLfloat scaleFactor = (radii[i] * 2) / spheres[i]->getDiameter();
		spheres[i]->setScaleFactor(scaleFactor);
	}
}
SPH::~SPH(){
	particles.clear();
}

GLint SPH::getRandInt(GLint max){
	return rand() % max;
}

GLuint SPH::bufferSize()
{
	return sizeof(GLfloat)* vertices.size();
}

GLfloat* SPH::getData(GLuint type)
{
	return vertices.data();
}

GLuint SPH::getSize(GLuint type)
{
	return sizeof(GLfloat)* vertices.size();
}
vector<GLfloat> SPH::makeVertices(vector<Particle*> parts){
	vector<GLfloat> verts;

	for (GLuint i = 0; i < parts.size(); i++){
		vec3 * position = &parts[i]->p;

		verts.push_back(position->x);
		verts.push_back(position->y);
		verts.push_back(position->z);
		verts.push_back(1.0f);
	}

	return verts;
}
void SPH::printVector(GLuint i, string tag){
	if (tag == "p")
		cout << particles[i]->p.x << " " << particles[i]->p.y << " " << particles[i]->p.z << endl;
	else if (tag == "v")
		cout << particles[i]->v.x << " " << particles[i]->v.y << " " << particles[i]->v.z << endl;
	else if (tag == "a")
		cout << particles[i]->a.x << " " << particles[i]->a.y << " " << particles[i]->a.z << endl;
}


GLfloat SPH::kernel(vec3 p){
	GLfloat nDist = (p.x*p.x + p.y*p.y + p.z*p.z) / (2.0f * SIGMA);
	if(nDist > 1)
		return 0;
	else
		return (1.0f / (TWOPI * SIGMA)) * exp(-nDist);
}

vec3 SPH::kernelGradient(vec3 p){
	vec3 temp = p;
	return (-temp / SIGMA) * kernel(p);
}

GLfloat SPH::density(GLint i){
	GLfloat result = 0;
	for(GLuint j = 0; j < SIZE; j++){
		if (length(particles[i]->p - particles[j]->p) < 0.01f)
			result += kernel(particles[i]->p - particles[j]->p) * 2.0f;
	}
	return result;
}

GLfloat SPH::pressure(GLint i){
	return 0.01 * pow(particles[i]->density - 0.05, 0.5);
}

vec3 SPH::aDueToPressure(GLint i){
	vec3 result(0.0f);
	for(GLuint j = 0; j < SIZE; j++){
		if (length(particles[i]->p - particles[j]->p) < 0.01f){
			vec3 kg = kernelGradient(particles[j]->p - particles[i]->p);
			result += kg * (particles[j]->pressure + particles[i]->pressure) / particles[j]->density;
		}
	}
	return result / particles[i]->density;
}

vec3 SPH::aDueToViscosity(GLint i){
	vec3 result(0.0f);
	for (GLuint j = 0; j < SIZE; j++){
		if (length(particles[i]->p - particles[j]->p) < 0.01f){
			GLfloat kg = kernel(particles[j]->p - particles[i]->p) * 0.1f;
			result += kg * (particles[j]->v - particles[i]->v) / particles[j]->density;
		}
	}
	return result / particles[i]->density;	
}

vec3 SPH::aDueToCollision(GLuint i, GLuint ci){
	vec3 a = particles[i]->p;
	vec3 v = particles[i]->v;
	vec3 b = a + v*dt;
	vec3 fColl = vec3(0.0f);
	GLfloat len = length(b - centers[ci]);

	if (len < radii[ci]){
		GLfloat l = len - radii[ci];
		vec3 n = (b - centers[ci]) / len;
		fColl = (-0.5f*l*n) - (0.1f*v);
	}

	return fColl;
}

void SPH::update(){
	for(GLuint i = 0; i < SIZE; i++){
		particles[i]->density = density(i);
		particles[i]->pressure = pressure(i);
	}

	for(GLuint i = 0; i < SIZE; i++){
		particles[i]->a = aDueToPressure(i) + aDueToViscosity(i) + GRAVITY;
		for (GLuint j = 0; j < 4; j++)
			particles[i]->a += aDueToCollision(i, j);
	}

	for(GLuint i = 0; i < SIZE; i++){
		particles[i]->v += particles[i]->a * dt;

		vec3 p = particles[i]->p;
		if (!isFunnel){
			if (p.y <= -15.0f){
				particles[i] = new Particle(spawns[getRandInt(spawns.size() - 1)]);
				particles[i]->v = vec3(0.0f);
			}
		}
		else{
			if (p.y <= -7.0f)
				particles[i]->v.y *= -0.5f;
			if (p.x <= -1.0f || p.x >= 1.0f)
				particles[i]->v.x *= -0.5f;
			if (p.z <= -1.0f || p.z >= 1.0f)
				particles[i]->v.z *= -0.5f;
		}
		particles[i]->p += particles[i]->v * dt;
	}
}

void SPH::draw(){
	vertices = makeVertices(particles);

	GLuint offset = 0;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize(), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		offset,
		getSize(VERTEX_DATA),
		getData(VERTEX_DATA));

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(VERTEX_DATA);
	glVertexAttribPointer(VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)offset);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, vertices.size() / 4);
}

void SPH::renderSPH(mat4 view, GLuint model_ptr, GLuint view_ptr){
	update();
	draw();
}

void SPH::drawSpheres(mat4 view, GLuint model_ptr, GLuint view_ptr){
	if (isFunnel)
		bowl->draw(view, model_ptr, view_ptr);
	else{
	  for (GLuint i = 0; i < spheres.size(); i++){
	    spheres[i]->draw(view, model_ptr, view_ptr);
	  }
	}

}
