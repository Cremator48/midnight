#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader_class.h"
#include "stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "Mesh.h"
#include "model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
void key_callback(GLFWwindow* window, int key, int action, int scancode, int mods);

// Константы
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float sizeOfBox = 50.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;


bool firstMouse = true;

float deltaTime = 0.0f;	// время между текущим и последним кадрами
float lastFrame = 0.0f; // время последнего кадра

bool isFlashlightEnable = true;
bool isDirLightEnable = true;
bool isPointLightEnable = true;

int main()
{

	std::cout << "git on windows!\n";

	// glfw: инициализация и конфигурирование
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw: создание окна
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// callback-функции
	{
		glfwMakeContextCurrent(window);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetKeyCallback(window, key_callback);
	}
	

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: загрузка всех указателей на OpenGL-функции
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// Указывание вершин неба и лампочки
	float vertices[] = {
		// координаты        // нормали           // текстурные координаты
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.75f, 0.33f,   // 7  //Задний квадрат (закрашено)
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.33f,   // 4
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.66f,   // 5
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.66f,   // 5
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.75f,  0.66f,  // 6
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.75f,  0.33f,  // 7

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.33f, // 0   // Передний квадрат (закрашено)
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.33f, // 3
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.66f, // 2
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.66f, // 2
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.66f, // 1
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.33f, // 0

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.66f, // 12  // Левый квадрат (закрашено)
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.66f, // 6
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.33f, // 7
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.33f, // 7
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.33f, // 13
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.66f, // 12

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.66f,	// 2		// Правый квадрат
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.66f, // 5
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.33f,	// 4
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.33f,	// 4
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.33f,	// 3
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.66f, // 2

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.75f, 0.33f,  // 7        // Нижний квадрат
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.33f,  // 4
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,   // 9
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,   // 9
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.75f, 0.0f,   // 8
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.75f, 0.33f,  // 7

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 0.66f,	 // 6		// Верхний квадрат
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.66f,  // 5
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f,   // 10
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f,   // 10
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 1.0f,   // 11
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 0.66f   // 6
	};

	// Указывание вершин пола
	float floorVertices[] = {
	   -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,	// Верхний квадрат
		0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// Указывание вершин окна
	float windowVertices[] = {
		-0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, // 0   // Передний квадрат (закрашено)
		0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f, // 3
		0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // 2
		0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // 2
	   -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, // 1
	   -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f  // 0
	};

	// Указывание вершин фрейм-прямоугольника
	float frameVertices[] =
	{
		/*
	   -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, // 0   // Передний квадрат (закрашено)
		0.5f, -0.5f,  0.0f, 1.0f,  0.0f, // 3
		0.5f,  0.5f,  0.0f, 1.0f,  1.0f, // 2

		0.5f,  0.5f,  0.0f, 1.0f,  1.0f, // 2
	   -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // 1
	   -0.5f, -0.5f,  0.0f, 0.0f,  0.0f  // 0
	   */

	   - 1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};


	//Настройка атрибутов вершин для скайбокса (и VBO для лампочки)
	unsigned int VBO, VAO;
	{
		//Данные для скайбокса
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Атрибуты текстур
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	//Настройка VBO и VAO для окна
	unsigned int windowVAO, windowVBO;
	{
		glGenVertexArrays(1, &windowVAO);
		glGenBuffers(1, &windowVBO);

		glBindVertexArray(windowVAO);
		glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);

		//Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Атрибуты нормалей
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Атрибуты текстур
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	//Настройка атрибутов вершин для пола
	unsigned int floorVAO;
	{
		unsigned int floorVBO;
		glGenVertexArrays(1, &floorVAO);
		glGenBuffers(1, &floorVBO);

		glBindVertexArray(floorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

		//Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Атрибуты нормалей
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Атрибуты текстур
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);

		glDeleteBuffers(1, &floorVBO);
	}
	
	// Настройка атрибутов вершин для лампочки
	unsigned int lightVAO;
	{
		//Настройка куба освещения
		glGenVertexArrays(1, &lightVAO);

		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}


	// Настройка Фрейм-буффера
	unsigned int fbo;
	unsigned int frameBuferTexture;
	{
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);


		//Создание текстуры для фрейм-буфера
		
		{
			
			glGenTextures(1, &frameBuferTexture);
			glBindTexture(GL_TEXTURE_2D, frameBuferTexture);

		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuferTexture, 0);

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frameBuferTexture, 0);
	}


	// Настройка рендр-буфера
	unsigned int rbo;
	{
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Настройка VAO и VBO для фреймбуфера
	unsigned int frameVAO, frameVBO;
	{
		glGenVertexArrays(1, &frameVAO);
		glGenBuffers(1, &frameVBO);

		glBindVertexArray(frameVAO);
		glBindBuffer(GL_ARRAY_BUFFER, frameVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), frameVertices, GL_STATIC_DRAW);

		//Координатный атрибут
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Атрибуты текстур
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	

	Shader ourShader("../midnight/shader.vs", "../midnight/shader.fs");
	Shader screenShader("../midnight/frameBuffShader.vs", "../midnight/frameBuffShader.fs");
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs");
	Shader skyBoxShader("../midnight/skyBoxShader.vs", "../midnight/skyBoxShader.fs");

	Model bankaModel("../res/models/banka/model.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	Model skullModel("../res/models/Skull/12140_Skull_v3_L2.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	Model backpackModel("../res/models/backpack/backpack.obj", aiProcess_Triangulate | /*aiProcess_FlipUVs | */  aiProcess_CalcTangentSpace);

	unsigned int diffuseMap = loadTexture("../res/box.png");
	unsigned int specularMap = loadTexture("../res/specular_map.png");
	unsigned int skyBox = loadTexture("../res/skybox.png");
	unsigned int windowModel = loadTexture("../res/blending_transparent_window.png");

	glm::vec3 pointLightPosition;

	


	// Позиции окон
	std::vector<glm::vec3> windows = {
		glm::vec3(-2.0f, 0.0f, -1.0f),
		glm::vec3(-2.0f, 0.0f, -2.0f)
	};

	// отрисовывать кадр при каждом обновлении экрана 
	glfwSwapInterval(1);

	float lastTime = glfwGetTime(); // Переменная хранящая время начала работы программы
	int nbFrames = 0;

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	// Раскомментируйте следующую строку для отрисовки полигонов в режиме каркаса
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Цикл рендеринга
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime(); // время текущего кадра с начала работы программы
		nbFrames++;  
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			std::cout << "FPS: " << double(nbFrames) << "\n";
			nbFrames = 0;
			lastTime += 1.0;
		}


		//отсчёт времени для нормализации скорости движения
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

		// Обработка ввода
		processInput(window);

		// Рендеринг в кастомный-фреймбуфер
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		{

			//Вкючить буфер глубины
			glEnable(GL_DEPTH_TEST);
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Рендеринг фона
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



			//Настройки освещения
			{

				// Куб-не источник
				ourShader.use();
				ourShader.setFloat("material.shininess", 32.0f);

				//[1] SpotLight
				//Настройки цвета самого света
				ourShader.setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
				ourShader.setVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				ourShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

				ourShader.setInt("isFlashlightEnable", isFlashlightEnable);


				//настройка затухания света
				ourShader.setFloat("spotLight.constant", 1.0f);
				ourShader.setFloat("spotLight.linear", 0.09f);
				ourShader.setFloat("spotLight.quadratic", 0.032f);

				//Настройка направления освещения + конуса прожектора
				ourShader.setVec3("spotLight.position", camera.Position);
				ourShader.setVec3("spotLight.direction", camera.Front);
				ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
				ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));


				//[2] PointLight
				ourShader.setInt("isPointLightEnable", isPointLightEnable);
				ourShader.setVec3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
				ourShader.setVec3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				ourShader.setVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

				//настройка затухания света
				ourShader.setFloat("pointLight.constant", 1.0f);
				ourShader.setFloat("pointLight.linear", 0.09f);
				ourShader.setFloat("pointLight.quadratic", 0.032f);

				//Настройка расположения источника света
				float x = sin(glfwGetTime()) * 5.0f;
				float y = 0.0f;
				float z = cos(glfwGetTime()) * 5.0f;
				pointLightPosition = glm::vec3(x, y, z);
				ourShader.setVec3("pointLight.position", pointLightPosition);


				//[3] DirLight

				ourShader.setInt("isDirLightEnable", isDirLightEnable);
				ourShader.setVec3("dirLight.direction", glm::vec3(3.0f, 3.0f, 3.0f));
				ourShader.setVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
				ourShader.setVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
				ourShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
			}


			//настройка основных матриц: вида и проекции
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 model;

			//Передача основных матриц и позиции камеры шейдеру
			{
				ourShader.use(); //Шейдерная программа уже запущенна в блоке кода "Настройки освещения"
				ourShader.setMat4("projection", projection);
				ourShader.setMat4("view", view);
				ourShader.setVec3("viewPos", camera.Position);
			}
			

			//Отрисовка пола
			{
				//Трансляция модели вниз и увеличение
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
				model = glm::scale(model, glm::vec3(13.0f, 13.0f, 13.0f));
				ourShader.setMat4("model", model);

				//Привязка diffuse текстуры
				ourShader.setInt("material.texture_diffuse1", 0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);

				//Привязка specular текстуры
				ourShader.setInt("material.texture_specular1", 1);
				glBindTexture(GL_TEXTURE_2D, specularMap);


				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularMap);


				//Отрисовка пола
				ourShader.use();
				glBindVertexArray(floorVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
				glBindVertexArray(0);
				glActiveTexture(GL_TEXTURE0);

			}

			// Отрисовка неба
			{
				skyBoxShader.use();
				skyBoxShader.setInt("myTexture", 0);

				//Трансляция модели вниз и увеличение
				model = glm::mat4(1.0f);
				model = glm::scale(model, glm::vec3(sizeOfBox, sizeOfBox, sizeOfBox));

				skyBoxShader.setMat4("model", model);
				skyBoxShader.setMat4("view", view);
				skyBoxShader.setMat4("projection", projection);

				//Привязка diffuse текстуры
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, skyBox);

				//Отрисовка куба
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);

				// Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
				glActiveTexture(GL_TEXTURE0);
				glBindVertexArray(0);
			}

			//Если PointLight включен - отрисовывай вращающийся куб-источник света
			if (isPointLightEnable)
			{
				glBindVertexArray(lightVAO);
				//Отрисовка куба-источника света
				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPosition);
				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
				lightCubeShader.use();
				lightCubeShader.setMat4("model", model);
				lightCubeShader.setMat4("projection", projection);
				lightCubeShader.setMat4("view", view);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			//Отрисовка моделей
			{
				//Отрисовка модели рюкзака
				{
					ourShader.use();
					model = glm::mat4(1.0f);
					model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
					ourShader.setMat4("model", model);
					backpackModel.Draw(ourShader);
				}


				//Отрисовка банки
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					ourShader.setMat4("model", model);
					bankaModel.Draw(ourShader);
				}

				//Отрисовка черепа
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					ourShader.setMat4("model", model);
					skullModel.Draw(ourShader);
				}

				//Отрисовка окон

				/*
				
				{
					//Сортировка отрисовки полупрозрачных окон по дистанции (для рендера начная с самого далёкого окна)
					std::map<float, glm::vec3> sorted;
					for (unsigned int i = 0; i < windows.size(); i++)
					{
						float distance = glm::length(camera.Position - windows[i]);
						sorted[distance] = windows[i];
					}



					//Привязка diffuse текстуры
					ourShader.setInt("material.texture_diffuse1", 0);
					glBindTexture(GL_TEXTURE_2D, windowModel);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, windowModel);

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, 0);

					ourShader.use();
					glBindVertexArray(windowVAO);

					for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
					{
						model = glm::mat4(1.0f);
						model = glm::translate(model, it->second);
						ourShader.setMat4("model", model);
						glDrawArrays(GL_TRIANGLES, 0, 6);
					}


				}
				*/

			}

			//Отрисовка камеры
			{
				
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 2.0f));

				screenShader.use();
				screenShader.setMat4("projection", projection);
				screenShader.setMat4("view", view);
				screenShader.setMat4("model", model);

				screenShader.setInt("screenTexture", 0);
				glBindVertexArray(frameVAO);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, frameBuferTexture);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				
			}

		}

		// Рендеринг на экран
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);

			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			screenShader.use();
			screenShader.setMat4("projection", glm::mat4(1.0f));
			screenShader.setMat4("view", glm::mat4(1.0f));
			screenShader.setMat4("model", glm::mat4(1.0f));
			screenShader.setInt("screenTexture", 0);
			glBindVertexArray(frameVAO);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, frameBuferTexture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		
		

		// glfw: обмен содержимым переднего и заднего буферов. Опрос событий ввода\вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	// Опционально: освобождаем все ресурсы, как только они выполнили свое предназначение
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &windowVBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteVertexArrays(1, &windowVAO);
	glDeleteFramebuffers(1, &fbo);



	// glfw: завершение, освобождение всех ранее задействованных GLFW-ресурсов
	glfwTerminate();
	return 0;
}

// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании кнопки мыши в данном кадре и соответствующая обработка данных событий
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Убеждаемся, что окно просмотра соответствует новым размерам окна.
	// Обратите внимание, что ширина и высота будут значительно больше, чем указано, на Retina-дисплеях
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // перевернуто, так как y-координаты идут снизу вверх

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		isFlashlightEnable = !isFlashlightEnable;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		isDirLightEnable = !isDirLightEnable;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		isPointLightEnable = !isPointLightEnable;
	}

}
