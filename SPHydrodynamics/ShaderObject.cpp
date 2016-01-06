#include "ShaderObject.h"

static GLubyte shaderText[8192];

ShaderObject::ShaderObject(){
}

ShaderObject::~ShaderObject(){
	glDeleteProgram(shaderID);
}

GLuint ShaderObject::getUniformLocation(const char* uniformID){
	return glGetUniformLocation(shaderID, uniformID);
}

bool ShaderObject::loadShaderFile(const char *filename, GLuint shader){
	GLint shaderLength = 0;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp != NULL) {
		while (fgetc(fp) != EOF)
			shaderLength++;

		rewind(fp);
		fread(shaderText, 1, shaderLength, fp);
		shaderText[shaderLength] = '\0';

		fclose(fp);
	}
	else {
		return false;
	}

	GLchar* fsStringPtr[1];
	fsStringPtr[0] = (GLchar *)((const char*)shaderText);
	glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);

	return true;
}

void ShaderObject::setupShaders(string vsFilename, string fsFilename){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderID = (GLuint)NULL;
	GLint testVal;

	if (!loadShaderFile(vsFilename.c_str(), hVertexShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << vsFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(fsFilename.c_str(), hFragmentShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << fsFilename << " could not be found." << endl;
	}

	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(hVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(hFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	shaderID = glCreateProgram();
	glAttachShader(shaderID, hVertexShader);
	glAttachShader(shaderID, hFragmentShader);

	glBindAttribLocation(shaderID, VERTEX_DATA, "position");
	glBindAttribLocation(shaderID, VERTEX_NORMAL, "normal");

	glLinkProgram(shaderID);
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(shaderID, 1024, NULL, infoLog);
		cout << "The shader program (" << vsFilename << " + " << fsFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(shaderID);
		shaderID = (GLuint)NULL;
	}

	mv_ptr = getUniformLocation("mv_matrix");
	view_ptr = getUniformLocation("view_matrix");
	proj_ptr = getUniformLocation("proj_matrix");
	light_pos = getUniformLocation("light_pos");
	amb_ptr = getUniformLocation("ambient");
	diff_ptr = getUniformLocation("diffuse_albedo");
	spec_pow_ptr = getUniformLocation("specular_power");
	spec_alb_ptr = getUniformLocation("specular_albedo");
}