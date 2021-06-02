//Ka - coeficiente ambiente
//Kd - coeficiente difuso
//Ks - coeficiente especular
//Ns - brilho
//Ni - densidade

#define _CRT_SECURE_NO_DEPRECATE

#include "Materiais.h"

#include <iostream>
#include <vector>

#include <glm/glm.hpp> // vec3, vec4, ivec4, mat4, ...
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ...
#include <glm/gtc/type_ptr.hpp> // value_ptr

using namespace std;

bool Material::loadMaterial(const char* f) {

	// codigo para abrir o ficheiro
	FILE* file = fopen(f, "r");

	if (file == NULL) {
		printf("ficheiro nao abre! \n");
		return false;
	}

	while (1) {
		char lineHeader[128];

		//lê a primeira palavra da linha e guarda em lineHeader
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			fclose(file);

			return true;
		}
		else {
			//ambiente
			if (strcmp(lineHeader, "Ka") == 0) {
				fscanf(file, "%f %f %f\n", &ka.x, &ka.y, &ka.z);
			}

			//difusa
			if (strcmp(lineHeader, "Kd") == 0) {
				fscanf(file, "%f %f %f\n", &kd.x, &kd.y, &kd.z);
			}

			//especularidade
			if (strcmp(lineHeader, "Ks") == 0) {
				fscanf(file, "%f %f %f\n", &ks.x, &ks.y, &ks.z);
			}

			//brilho
			if (strcmp(lineHeader, "Ns") == 0) {
				fscanf(file, "%f\n", &ns);
			}


			//textura
			if (strcmp(lineHeader, "map") == 0) {
				fscanf(file, "%s\n", &textpath);
			}
		}
	}

}