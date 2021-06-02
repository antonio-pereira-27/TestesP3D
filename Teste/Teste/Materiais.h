#pragma once

#include <GL\glew.h>
#include <GL\gl.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <iostream>
#include <vector>

using namespace std;

class Material {
public:
	bool loadMaterial(const char* f);

	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	GLfloat ns;
	GLfloat ni;

	string textpath;

};