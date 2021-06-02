#pragma once
//#pragma comment(lib, "glew32s.lib")
//#pragma comment(lib, "glfw3.lib")
//#pragma comment(lib, "opengl32.lib")
#include <GL\glew.h>
#include <GL\gl.h>

#include <glm/glm.hpp> // vec3, vec4, ivec4, mat4, ...
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ...
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <iostream>
#include <vector>
#include "Materiais.h"



using namespace std;


class OBJ {
public:
	bool loadOBJ(const char* path);

	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normais;

	string matlink;
	Material material;

};






