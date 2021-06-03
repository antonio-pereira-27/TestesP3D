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
void deformObject(GLFWwindow* window);

#define WIDTH 1920
#define HEIGHT 1080


GLuint VAO;
GLuint Buffers[3];
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
bool def_active, key5_releassed;


OBJ IronMan;

int main(void) {
	//criar a variavel da janela
	GLFWwindow* window = nullptr;
	GLFWwindow* oldWindow = nullptr;
	GLclampf backgroundColor[3] = { 1.0f, 1.0f, 1.0f };
	def_active = true;


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
		
			"Iron_Man/Iron_Man_D.tga"
		};
		load_textures(textureFiles);

		init();
		// funçao para chamar as teclas fisicas do teclado
		glfwSetKeyCallback(window, keyReceiver);

		// funçao para chamar o scroll do rato
		glfwSetScrollCallback(window, scrollReceiver);
		glfwSetCursorPosCallback(window, mouseReceiver);


		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT);

			deformObject(window);
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
	glGenBuffers(3, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferStorage(GL_ARRAY_BUFFER, IronMan.vertices.size() * sizeof(glm::vec3), &IronMan.vertices[0], 0);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferStorage(GL_ARRAY_BUFFER, IronMan.uvs.size() * sizeof(glm::vec2), &IronMan.uvs[0], 0);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glBufferStorage(GL_ARRAY_BUFFER, IronMan.normais.size() * sizeof(glm::vec3), &IronMan.normais[0], 0);
	
	
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
	// Obtém a localização do atributo 'vTexture' no 'programa'.
	GLint uvsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vTexture");

	// Ativa o VBO 'Buffer'.
	// Liga a localização do atributo 'vPosition' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Especifica também como é que a informação do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo irá receber, por vértice, 3 elementos do tipo float. Stride de 6 floats e offset de 0 bytes.
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(coordsId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0 /*stride*/, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glVertexAttribPointer(uvsId, 2 /*2 elementos por vértice*/, GL_FLOAT /*do tipo float*/, GL_FALSE, 0 /* stride*/, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glVertexAttribPointer(normalId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0 /*stride*/, (void*)0);
	
	// Habitita o atributo com localização 'coresId' para o VAO ativo.
	glEnableVertexAttribArray(coordsId);
	// Habitita o atributo com localização 'uvsId' para o VAO ativo.
	glEnableVertexAttribArray(uvsId);
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

	// Fonte de luz conica
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(glm::vec3(5.0, 5.0, 5.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotDirection"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.constant"), 1.0f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.linear"), 0.06f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.quadratic"), 0.02f);
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotCutoff"), glm::cos(glm::radians(11.0f)));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotExponent"), glm::cos(glm::radians(13.0f)));

	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.emissive"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.shininess"), IronMan.material.ns);

	// Light control first declaration
	GLint ambLightId = glGetProgramResourceLocation(programa, GL_UNIFORM, "AmbLightOn");
	glProgramUniform1i(programa, ambLightId, 1);
	GLint dirLightId = glGetProgramResourceLocation(programa, GL_UNIFORM, "DirLightOn");
	glProgramUniform1i(programa, dirLightId, 1);
	GLint pointLightId = glGetProgramResourceLocation(programa, GL_UNIFORM, "PointLightOn");
	glProgramUniform1i(programa, pointLightId, 1);
	GLint spotLightId = glGetProgramResourceLocation(programa, GL_UNIFORM, "SpotLightOn");
	glProgramUniform1i(programa, spotLightId, 1);

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

	glGenTextures(1, &textureName);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, textureName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nChannels;
	unsigned char* imageData = stbi_load("Iron_Man/Iron_Man_D.tga", &width, &height, &nChannels, 0);
	if (imageData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

		glGenerateMipmap(GL_TEXTURE_2D);
		
		stbi_image_free(imageData);
	}
	else
	{
		cout << "Error loading texture!" << endl;
	}
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

void deformObject(GLFWwindow* window) {
	// Release all the keys
	if (!glfwGetKey(window, 53)) // ASCII value for number 5
		key5_releassed = true;

	// Deformation control on/off
	if (glfwGetKey(window, 53) && key5_releassed)
	{
		if (def_active)
		{
			// Deform active
			GLint effectViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "EffectActive");
			glProgramUniform1i(programa, effectViewId, 1);
			std::cout << "Deformation is on!" << std::endl;
			def_active = false;
		}
		else
		{
			// Deform not active
			GLint effectViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "EffectActive");
			glProgramUniform1i(programa, effectViewId, 0);
			std::cout << "Deformation is off!" << std::endl;
			def_active = true;
		}
		key5_releassed = false;
	}
}