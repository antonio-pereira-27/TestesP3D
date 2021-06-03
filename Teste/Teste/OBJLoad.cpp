
#define _CRT_SECURE_NO_DEPRECATE

#include "OBJLoad.h"
#include <iostream>
#include <vector>

#include <glm/glm.hpp> // vec3, vec4, ivec4, mat4, ...
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ...
#include <glm/gtc/type_ptr.hpp> // value_ptr


/*%d - inteiro %f - float   %s - string*/


bool OBJ::loadOBJ(const char* path) {
	// variáveis para guardar o ficheiro
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;



	// codigo para abrir o ficheiro
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("ficheiro nao abre! \n");
		return false;
	}




	while (1) {
		char lineHeader[128];

		//lê a primeira palavra da linha e guarda em lineHeader
		int res = fscanf(file, "%s", lineHeader);

		//ver se o ficheiro acabou
		if (res == EOF) {
			fclose(file);

			//processar data
			//vertices
			for (int i = 0; i < vertexIndices.size(); i++) {
				int vertexindex = vertexIndices[i];
				glm::vec3 vertex = temp_vertices[vertexindex - 1];
				vertices.push_back(vertex);
			}

			//normais
			for (int i = 0; i < uvIndices.size(); i++) {
				int uvindex = uvIndices[i];
				glm::vec2 uvex = temp_uvs[uvindex - 1];
				uvs.push_back(uvex);
			}

			//normais
			for (int i = 0; i < normalIndices.size(); i++) {
				int normalindex = normalIndices[i];
				glm::vec3 normal = temp_normals[normalindex - 1];
				normais.push_back(normal);
			}

			string folderpath = "Iron_Man/";
			string path = folderpath + matlink.data();

			if (material.loadMaterial((char*)path.data()) == true) {
				cout << "Material Carregado" << endl;
			}
			else {
				cout << "erro a carregar material " << endl;
			}

			break;
		}
		//passar a informação
		else {

			//se a linha começa com  mtlib é o caminho para o ficheiro do material, guardamos na variável
			if (strcmp(lineHeader, "mtllib") == 0) {
				fscanf(file, "%s", &matlink);
			}


			//validação de vertices
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				//"%f %f %f" indica a estrutura dos dados, os vérices são apresentados como floats e separados por espaços
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

				temp_vertices.push_back(vertex);
			}
			//validação de uvs
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);

				temp_uvs.push_back(uv);
			}
			//validação de normais	
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);

				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

				if (matches != 9) {
					return false;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}


		}
	}

	cout << "Vertices: " << temp_vertices.size() << endl;
	cout << "UVs: " << temp_uvs.size() << endl;
	cout << "Normais: " << temp_normals.size() << endl;
	cout << "Indices vertics: " << vertexIndices.size() << endl;
	cout << "uvIndices: " << uvIndices.size() << endl;
	cout << "normalIndices: " << normalIndices.size() << endl;

	for (size_t i = 0; i < vertexIndices.size(); ++i)
	{
		vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
		uvs.push_back(temp_uvs[uvIndices[i] - 1]);
		normais.push_back(temp_normals[normalIndices[i] - 1]);
	}
}