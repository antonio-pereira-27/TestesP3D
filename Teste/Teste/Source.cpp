#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")


#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include "LoadShaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "OBJLoad.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp> // glm::inverseTranspose()


void print_error(int error, const char* description);
void print_gl_info(void);
void load_textures(vector<string> textureFiles);
void init(void);
void display(void);
void keyReceiver(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollReceiver(GLFWwindow* window, double xoffset, double yoffset);
void mouseReceiver(GLFWwindow* window, double xpos, double ypos);

#define WIDTH 1920
#define HEIGHT 1080


GLuint VAO;
GLuint Buffer;
const GLuint NumVertices = 6 * 2 * 3; // 6 faces * 2 triângulos/face * 3 vértices/triângulo

GLuint programa;

glm::mat4 Model, View, Projection;
glm::mat3 NormalMatrix;
GLfloat zoom = 5.0f;
GLfloat rotationAngle = 0.0f;
GLfloat angx = 0.0f;
GLfloat angy = 0.0f;

// variável que guardo o estado da janela
int windowState = 0; // 0 = normal, 1 = fullscreen, -1 = nao faz nada


OBJ IronMan;

int main(void) {
	//criar a variavel da janela
	GLFWwindow* window = nullptr;
	GLFWwindow* oldWindow = nullptr;
	GLclampf backgroundColor[3] = { 1.0f, 1.0f, 1.0f };


	glfwSetErrorCallback(print_error);

	if (!glfwInit()) return -1;

	while (windowState != -1)
	{
		if (windowState == 0) // janela normal de 1280 por 720
		{
			// atribuir os valores para a janela
			window = glfwCreateWindow(WIDTH, HEIGHT, "Trabalho Grupo 20", nullptr, oldWindow);
			cout << "windowed program" << endl;
			if (oldWindow != nullptr)
				glfwDestroyWindow(oldWindow);
			oldWindow = window;
		}
		else if (windowState == 1) // fullscreen
		{
			// atribuir os valores para a janela
			window = glfwCreateWindow(WIDTH, HEIGHT, "Trabalho Grupo 20", glfwGetPrimaryMonitor(), oldWindow);
			cout << "fullscreen program" << endl;
			if (oldWindow != nullptr)
				glfwDestroyWindow(oldWindow);
			oldWindow = window;
		}

		if (window == nullptr)//termina o programa
		{

			cout << "closed program" << endl;
			glfwTerminate();
			return -1;
		}

		// valor -1 para a janela não fazer nada
		windowState = -1;

		glfwMakeContextCurrent(window);

		// Inicia o gestor de extensões GLEW
		glewExperimental = GL_TRUE;
		glewInit();

		print_gl_info();

		vector<string> textureFiles{
			/*"textures/xpos.tga",
			"textures/xneg.tga",
			"textures/ypos.tga",
			"textures/yneg.tga",
			"textures/zpos.tga",
			"textures/zneg.tga"*/
			"Iron_Man/Iron_Man_D.tga"
		};
		load_textures(textureFiles);

		init();
		// funçao para chamar as teclas fisicas do teclado
		glfwSetKeyCallback(window, keyReceiver);

		// funçao para chamar o scroll do rato
		glfwSetScrollCallback(window, scrollReceiver);
		glfwSetCursorPosCallback(window, mouseReceiver);

		// Indicação da Unidade de Textura a ligar ao sampler 'cubeMap'.
		GLint location_textureArray = glGetProgramResourceLocation(programa, GL_UNIFORM, "cubeMap");
		glProgramUniform1i(programa, location_textureArray, 0 /* Unidade de Textura #0 */);

		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT);

			display();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}

void print_gl_info(void) {
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	cout << "\nOpenGL version " << major << '.' << minor << endl;

	const GLubyte* glversion = glGetString(GL_VERSION);
	const GLubyte* glvendor = glGetString(GL_VENDOR);
	const GLubyte* glrenderer = glGetString(GL_RENDERER);
	cout << "\nVersion:  " << glversion << endl <<
		"Vendor:   " << glvendor << endl <<
		"Renderer: " << glrenderer << endl;

	cout << "\nSupported GLSL versions:\n";
	const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "Higher supported version:\n\t" << glslversion << endl;
	GLint numglslversions;
	cout << "Other supported versions:\n";
	glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &numglslversions);
	for (int n = 0; n < numglslversions; n++) {
		cout << '\t' << glGetStringi(GL_SHADING_LANGUAGE_VERSION, n) << endl;
	}
}

void print_error(int error, const char* description) {
	cout << description << endl;
}

void init(void) {

	IronMan.loadOBJ("Iron_Man/Iron_Man.obj");

	//printf(IronMan.matlink.data());
	// ****************************************************
	// VAOs - Vertex Array Objects
	// ****************************************************

	// Gerar nomes para VAOs.
	// Neste caso gera apenas 1 nome.
	glGenVertexArrays(1, &VAO);
	// Faz bind do VAO, cujo nome está definido em 'VAO', com o contexto do OpenGL.
	// Um VAO é criado no primero bind que lhe seja feito.
	// Este VAO passa a estar ativo até que seja feito o bind a outro VAO, ou seja feito o bind com valor 0.
	glBindVertexArray(VAO);

	// ****************************************************
	// VBOs - Vertex Buffer Objects
	// ****************************************************

	// Gera 'NumBuffers' nomes para VBOs.
	// Neste caso gera 1 nome
	// Esta função pode ser chamada antes da criação de VAOs.
	glGenBuffers(1, &Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Buffer);
	// Inicializa o VBO (que está ativo) com dados imutáveis.
	glBufferStorage(GL_ARRAY_BUFFER, IronMan.vertices.size() * sizeof(glm::vec3), &IronMan.vertices[0], 0);

	// ****************************************************
	// Shaders
	// ****************************************************

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER,   "light.vert" },
		{ GL_FRAGMENT_SHADER, "light.frag" },
		{ GL_NONE, NULL }
	};

	programa = LoadShaders(shaders);
	if (!programa) exit(EXIT_FAILURE);
	glUseProgram(programa);

	// ****************************************************
	// Ligar Atributos aos Shaders
	// ****************************************************

	// Obtém a localização do atributo 'vPosition' no 'programa'.
	GLint coordsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vPosition");
	// Obtém a localização do atributo 'vNormal' no 'programa'.
	GLint normalId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vNormal");

	// Ativa o VBO 'Buffer'.
	glBindBuffer(GL_ARRAY_BUFFER, Buffer);
	// Liga a localização do atributo 'vPosition' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Especifica também como é que a informação do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo irá receber, por vértice, 3 elementos do tipo float. Stride de 6 floats e offset de 0 bytes.
	glVertexAttribPointer(coordsId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0 /*stride*/, (void*)0);
	glVertexAttribPointer(normalId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0 /*stride*/, (void*)0);

	// Habitita o atributo com localização 'coresId' para o VAO ativo.
	glEnableVertexAttribArray(coordsId);
	// Habitita o atributo com localização 'normalId' para o VAO ativo.
	glEnableVertexAttribArray(normalId);

	// ****************************************************
	// Matrizes de transformação
	// ****************************************************

	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(glm::vec3(5.0f, 5.0f, zoom), glm::vec3(0.0f, 2.0f, -0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// Matriz do Modelo
	Model = glm::mat4(1.0f);

	// Implementar a rotação ao cubo para se ver todas as faces
	//Model = rotate(Model, rotationAngle += 0.01f, normalize(vec3(1.0f, 1.0f, 1.0f)));
	Model = glm::rotate(Model, angx, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::mat4 ModelView = View * Model;
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));

	// ****************************************************
	// Uniforms
	// ****************************************************

	// Atribui valor ao uniform Model
	GLint modelId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(programa, modelId, 1, GL_FALSE, glm::value_ptr(Model));
	// Atribui valor ao uniform View
	GLint viewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(programa, viewId, 1, GL_FALSE, glm::value_ptr(View));
	// Atribui valor ao uniform ModelView
	GLint modelViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(programa, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));
	// Atribui valor ao uniform Projection
	GLint projectionId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(programa, projectionId, 1, GL_FALSE, glm::value_ptr(Projection));
	// Atribui valor ao uniform NormalMatrix
	GLint normalViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(programa, normalViewId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

	// Fonte de luz ambiente global
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

	// Fonte de luz direcional
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

	// Fonte de luz pontual #1
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(-5.0, -5.0, -5.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

	// Fonte de luz pontual #2
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(5.0, 5.0, 5.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].constant"), 1.0f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].linear"), 0.06f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].quadratic"), 0.02f);

	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.emissive"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.shininess"), 12.0f);

	// ****************************************************
	// Definir a janela de visualização (viewport)
	// ****************************************************

	glViewport(0, 0, WIDTH, HEIGHT);

	// ****************************************************
	// Outros parâmetros do OpenGL
	// ****************************************************
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Por defeito está desativado
	glCullFace(GL_BACK); // GL_FRONT, [GL_BACK], GL_FRONT_AND_BACK
}

void display(void) {
	static const GLfloat black[] = {
		0.0f, 0.0f, 0.0f, 0.0f
	};
	// Limpa o buffer de cor
	glClearBufferfv(GL_COLOR, 0, black);
	// Limpa o buffer de profundidade
	glClear(GL_DEPTH_BUFFER_BIT);

	// Atualiza os dados do Uniform
	View = glm::lookAt(glm::vec3(5.0f, 5.0f, zoom), glm::vec3(0.0f, 2.0f, -0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// Matriz do Modelo
	Model = glm::mat4(1.0f);

	// Implementar a rotação ao cubo para se ver todas as faces
	//Model = rotate(Model, rotationAngle += 0.01f, normalize(vec3(1.0f, 1.0f, 1.0f)));
	Model = glm::rotate(Model, angx, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::mat4 ModelView = View * Model;
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));
	// Atribui valor ao uniform Model
	GLint modelId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(programa, modelId, 1, GL_FALSE, glm::value_ptr(Model));
	GLint viewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(programa, viewId, 1, GL_FALSE, glm::value_ptr(View));
	GLint modelViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(programa, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));
	// Atribui valor ao uniform NormalMatrix
	GLint normalViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(programa, normalViewId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

	// Vincula (torna ativo) o VAO
	glBindVertexArray(VAO);
	// Envia comando para desenho de primitivas GL_TRIANGLES, que utilizará os dados do VAO vinculado.
	glDrawArrays(GL_TRIANGLES, 0, IronMan.vertices.size());
}

void load_textures(vector<string> textureFiles) {
	GLuint textureName = 0;

	// Gera um nome de textura
	glGenTextures(1, &textureName);

	// Ativa a Unidade de Textura #0
	// A Unidade de Textura 0 está ativa por defeito.
	// Só uma Unidade de Textura pode estar ativa.
	glActiveTexture(GL_TEXTURE0);

	// Vincula esse nome de textura ao target GL_TEXTURE_CUBE_MAP da Unidade de Textura ativa.
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureName);

	// NOTA:
	// Num cube map de texturas, todas as texturas devem:
	// - ter a mesma resolução;
	// - possuir o mesmo número de níveis de mipmap; e,
	// - partilhar os mesmos parâmetros.

	// Define os parâmetros de filtragem (wrapping e ajuste de tamanho)
	// para a textura que está vinculada ao target GL_TEXTURE_CUBE_MAP da Unidade de Textura ativa.
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#ifdef _D_STORAGE
	// Aloca memória para o cube map de texturas
	// Textura imutável, i.e., apenas é possível alterar a imagem.
	{
		// Leitura da resolução e número de canais da imagem.
		int width, height, nChannels;
		// Ativa a inversão vertical da imagem, aquando da sua leitura para memória.
		stbi_set_flip_vertically_on_load(true);
		unsigned char* imageData = stbi_load(textureFiles[0].c_str(), &width, &height, &nChannels, 0);
		if (imageData) {
			stbi_image_free(imageData);

			// Alocação de memória
			glTexStorage2D(GL_TEXTURE_CUBE_MAP,
				1,					// Número de níveis de Mipmap para as texturas. 1 se não forem utilizados Mipmaps.
				nChannels == 4 ? GL_RGBA8 : GL_RGB8,	// Formato interno da imagem de textura
				width, height		// width, height
			);
		}
		else {
			cout << "Error loading texture!" << endl;
		}
	}

	// Para cada face do cubo
	GLint face = 0;
	for (auto file : textureFiles) {
		// Leitura/descompressão do ficheiro com imagem de textura
		int width, height, nChannels;
		unsigned char* imageData = stbi_load(file.c_str(), &width, &height, &nChannels, 0);
		if (imageData) {
			// Carrega os dados da imagem para o Objeto de Textura vinculado ao target da face
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
				0,					// Nível do Mipmap
				0, 0,				// xoffset, yoffset
				width, height,		// width, height
				nChannels == 4 ? GL_RGBA : GL_RGB,	// Formato da imagem
				GL_UNSIGNED_BYTE,	// Tipos dos dados da imagem
				imageData);			// Apontador para os dados da imagem de textura

			face++;

			// Liberta a imagem da memória do CPU
			stbi_image_free(imageData);
		}
		else {
			cout << "Error loading texture!" << endl;
		}
	}
#else
	// Ativa a inversão vertical da imagem, aquando da sua leitura para memória.
	stbi_set_flip_vertically_on_load(true);

	// Para cada face do cubo
	GLint face = 0;

	for (auto file : textureFiles) {
		// Leitura/descompressão do ficheiro com imagem de textura
		int width, height, nChannels;
		unsigned char* imageData = stbi_load(file.c_str(), &width, &height, &nChannels, 0);
		if (imageData) {
			// Carrega os dados da imagem para o Objeto de Textura vinculado ao target da face
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
				0,					// Nível do Mipmap
				GL_RGB,				// Formato interno do OpenGL
				width, height,		// width, height
				0,					// border
				nChannels == 4 ? GL_RGBA : GL_RGB,	// Formato da imagem
				GL_UNSIGNED_BYTE,	// Tipos dos dados da imagem
				imageData);			// Apontador para os dados da imagem de textura

			face++;

			// Liberta a imagem da memória do CPU
			stbi_image_free(imageData);
		}
		else {
			cout << "Error loading texture!" << endl;
		}
	}
#endif
}

// funçao para reconhecer as teclas do teclado e quando estas são pressionadas fazerem algo em especifico
void keyReceiver(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// recolhe um apontador do janela 
	GLclampf* myColor = (GLclampf*)glfwGetWindowUserPointer(window);

	// tecla f1 para fullscreen
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		windowState = 1;
	}

	//tecla f2 para windowed
	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		windowState = 0;
	}

	// tecla esc par sair da janela 
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

//função para receber o movimento do scroll
//o scrollCallBack só dá os valores 0, 1 e -1 por isso se o valor for 1, quer dizer que estamos a fazer zoom no modelo, se o valor for -1 então estamos a afastarmo-nos do modelo
// é utilizado o fabs para retornar sempre o valor absoluto do zoom para assim este não poder se afastar demais do modelo
void scrollReceiver(GLFWwindow* window, double xoffset, double yoffset)
{
	// aproxima
	if (yoffset == 1)
		zoom -= fabs(zoom) * 0.1f;

	// afasta
	if (yoffset == -1)
		zoom += fabs(zoom) * 0.1f;
}

void mouseReceiver(GLFWwindow* window, double xpos, double ypos) {
	angx = xpos * 0.01f;
	angy = ypos * 0.01f;
}
