#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader_class.h"
#include "stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);


// Константы
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

float deltaTime = 0.0f;	// время между текущим и последним кадрами
float lastFrame = 0.0f; // время последнего кадра

glm::vec3 positionOfBox(0.0f, 0.0f, 0.0f);


int main()
{
	std::cout << "Naked program!\n";

	// glfw: инициализация и конфигурирование
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	// Включаем мультисэмплинг
	glfwWindowHint(GLFW_SAMPLES, 4);


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
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// glad: загрузка всех указателей на OpenGL-функции
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// Координаты вершин куба
	float cubeVerticesPositions[] =
	{
		/*--------------------------------------------------------------------*/
			// Дальняя грань (против часовой)
			 0.5f,  0.5f, -0.5f,	// 2
			 0.5f, -0.5f, -0.5f,	// 1
			-0.5f, -0.5f, -0.5f,	// 0

			-0.5f, -0.5f, -0.5f,	// 0
			-0.5f,  0.5f, -0.5f,	// 3
			 0.5f,  0.5f, -0.5f,	// 2
			 /*--------------------------------------------------------------------*/
				 // Ближняя грань (против часовой)
				 -0.5f, -0.5f,  0.5f,	// 4
				  0.5f, -0.5f,  0.5f,	// 5
				  0.5f,  0.5f,  0.5f,	// 6

				  0.5f,  0.5f,  0.5f,	// 6
				 -0.5f,  0.5f,  0.5f,	// 7
				 -0.5f, -0.5f,  0.5f,	// 4
				 /*--------------------------------------------------------------------*/
					 // Левая грань (против часовой)
					 -0.5f,  0.5f,  0.5f,	// 7
					 -0.5f,  0.5f, -0.5f,	// 3
					 -0.5f, -0.5f, -0.5f,	// 0

					 -0.5f, -0.5f, -0.5f,	// 0
					 -0.5f, -0.5f,  0.5f,	// 4
					 -0.5f,  0.5f,  0.5f,	// 7 
					 /*--------------------------------------------------------------------*/
						  // Правая грань (против часовой)
						  0.5f, -0.5f, -0.5f,	// 1
						  0.5f,  0.5f, -0.5f,	// 2
						  0.5f,  0.5f,  0.5f,	// 6

						  0.5f,  0.5f,  0.5f,	// 6
						  0.5f, -0.5f,  0.5f,	// 5
						  0.5f, -0.5f, -0.5f,	// 1
						  /*--------------------------------------------------------------------*/
							  // Нижняя грань (против часовой)
							  -0.5f, -0.5f, -0.5f,	// 0
							   0.5f, -0.5f, -0.5f,	// 1
							   0.5f, -0.5f,  0.5f,	// 5

							   0.5f, -0.5f,  0.5f,  	// 5
							  -0.5f, -0.5f,  0.5f,  	// 4
							  -0.5f, -0.5f, -0.5f, 	// 0
							  /*--------------------------------------------------------------------*/
								   // Верхняя грань (против часовой)
								   0.5f,  0.5f,  0.5f,  	// 6
								   0.5f,  0.5f, -0.5f,	// 2
								  -0.5f,  0.5f, -0.5f,	// 3	

								  -0.5f,  0.5f, -0.5f,	// 3
								  -0.5f,  0.5f,  0.5f,	// 7
								   0.5f,  0.5f,  0.5f		// 6
								   /*--------------------------------------------------------------------*/
	};

	// Нормали куба
	float cubeVerticesNormals[] =
	{
		/*--------------------------------------------------------------------*/
			// Дальняя грань (против часовой)
			 0.0f,  0.0f, -1.0f,	// 2	
			 0.0f,  0.0f, -1.0f,	// 1
			 0.0f,  0.0f, -1.0f,	// 0

			 0.0f,  0.0f, -1.0f,	// 0
			 0.0f,  0.0f, -1.0f,	// 3
			 0.0f,  0.0f, -1.0f,	// 2

			 /*--------------------------------------------------------------------*/
				 // Ближняя грань (против часовой)
				  0.0f,  0.0f, 1.0f,		// 4	
				  0.0f,  0.0f, 1.0f,		// 5
				  0.0f,  0.0f, 1.0f,		// 6

				  0.0f,  0.0f, 1.0f,		// 6
				  0.0f,  0.0f, 1.0f,		// 7
				  0.0f,  0.0f, 1.0f,		// 4

				  /*--------------------------------------------------------------------*/
					  // Левая грань (против часовой)
					   -1.0f,  0.0f,  0.0f,	// 7	
					   -1.0f,  0.0f,  0.0f,	// 3
					   -1.0f,  0.0f,  0.0f,	// 0

					   -1.0f,  0.0f,  0.0f,	// 0
					   -1.0f,  0.0f,  0.0f,	// 4
					   -1.0f,  0.0f,  0.0f,	// 7

					   /*--------------------------------------------------------------------*/
						   // Правая грань (против часовой)
							1.0f,  0.0f,  0.0f,	// 1	
							1.0f,  0.0f,  0.0f,	// 2
							1.0f,  0.0f,  0.0f,	// 6	

							1.0f,  0.0f,  0.0f,	// 6
							1.0f,  0.0f,  0.0f,	// 5
							1.0f,  0.0f,  0.0f,	// 1

							/*--------------------------------------------------------------------*/
								// Нижняя грань (против часовой)
								 0.0f, -1.0f,  0.0f, 	// 0	
								 0.0f, -1.0f,  0.0f,	// 1
								 0.0f, -1.0f,  0.0f,	// 5

								 0.0f, -1.0f,  0.0f,	// 5
								 0.0f, -1.0f,  0.0f,	// 4
								 0.0f, -1.0f,  0.0f,	// 0

								 /*--------------------------------------------------------------------*/
									 // Верхняя грань (против часовой)
									  0.0f,  1.0f,  0.0f,	// 6	
									  0.0f,  1.0f,  0.0f,	// 2
									  0.0f,  1.0f,  0.0f,	// 3	

									  0.0f,  1.0f,  0.0f,	// 3
									  0.0f,  1.0f,  0.0f,	// 7
									  0.0f,  1.0f,  0.0f	// 6

									  /*--------------------------------------------------------------------*/
	};

	// Текстурные координаты куба
	float cubeVerticesTextureCoords[] =
	{
		/*--------------------------------------------------------------------*/
		// Дальняя грань (против часовой)
		1.0f, 1.0f,	// 2	
		1.0f, 0.0f,	// 1
		0.0f, 0.0f,	// 0

		0.0f, 0.0f,	// 0
		0.0f, 1.0f,   // 3
		1.0f, 1.0f,   // 2

		/*--------------------------------------------------------------------*/
		 // Ближняя грань (против часовой)
		0.0f, 0.0f,	// 4	
		1.0f, 0.0f,	// 5
		1.0f, 1.0f,	// 6

		1.0f, 1.0f,	// 6
		0.0f, 1.0f,	// 7
		0.0f, 0.0f,	// 4

		/*--------------------------------------------------------------------*/
		 // Левая грань (против часовой)
		1.0f, 0.0f,	// 7	
		1.0f, 1.0f,	// 3
		0.0f, 1.0f,	// 0

		0.0f, 1.0f,	// 0
		0.0f, 0.0f,	// 4
		1.0f, 0.0f,	// 7

		/*--------------------------------------------------------------------*/
		 // Правая грань (против часовой)
		0.0f, 1.0f,	// 1	
		1.0f, 1.0f,	// 2
		1.0f, 0.0f,	// 6	

		1.0f, 0.0f,	// 6
		0.0f, 0.0f,	// 5
		0.0f, 1.0f,	// 1

		/*--------------------------------------------------------------------*/
		 // Нижняя грань (против часовой)
		0.0f, 1.0f, 	// 0	
		1.0f, 1.0f,	// 1
		1.0f, 0.0f,	// 5

		1.0f, 0.0f,	// 5
		0.0f, 0.0f,	// 4
		0.0f, 1.0f,	// 0

		/*--------------------------------------------------------------------*/
		 // Верхняя грань (против часовой)
		1.0f, 0.0f,	// 6	
		1.0f, 1.0f,	// 2
		0.0f, 1.0f,	// 3	

		0.0f, 1.0f,	// 3
		0.0f, 0.0f,	// 7
		1.0f, 0.0f	// 6

		/*--------------------------------------------------------------------*/
	};

	// Координаты вершин пола
	float floorVerticesPositions[] =
	{
		// Верхняя грань (против часовой)
		 0.5f,  0.5f,  0.5f,	// 6
		 0.5f,  0.5f, -0.5f,	// 2
		-0.5f,  0.5f, -0.5f,	// 3	

		-0.5f,  0.5f, -0.5f,	// 3
		-0.5f,  0.5f,  0.5f,	// 7
		 0.5f,  0.5f,  0.5f		// 6
	};

	// Текстурные координаты пола
	float floorTextureCoords[] =
	{
		10.0f, 0.0f,	// 6	
		10.0f, 10.0f,	// 2
		0.0f,  10.0f,	// 3	

		0.0f,  10.0f,	// 3
		0.0f,  0.0f,	// 7
		10.0f, 0.0f		// 6
	};


	float floorNormals[] =
	{
		// Верхняя грань (против часовой)
									  0.0f,  1.0f,  0.0f,	// 6	
									  0.0f,  1.0f,  0.0f,	// 2
									  0.0f,  1.0f,  0.0f,	// 3	

									  0.0f,  1.0f,  0.0f,	// 3
									  0.0f,  1.0f,  0.0f,	// 7
									  0.0f,  1.0f,  0.0f	// 6
	};

	// Координаты вершин экранного прямоугольника
	float screenPlaneVertices[] =
	{
			-0.5f, -0.5f, 0.5f,	// 4
			 0.5f, -0.5f, 0.5f,	// 5
			 0.5f, 0.5f, 0.5f,	// 6

			 0.5f, 0.5f, 0.5f,	// 6
			-0.5f, 0.5f, 0.5f,	// 7
			-0.5f, -0.5f, 0.5f,	// 4
	};

	// Координаты текстур экранного прямоугольника
	float screenTextureCoords[] =
	{
		0.0f, 0.0f,	// 4	
		1.0f, 0.0f,	// 5
		1.0f, 1.0f,	// 6

		1.0f, 1.0f,	// 6
		0.0f, 1.0f,	// 7
		0.0f, 0.0f,	// 4
	};

	unsigned int screenVAO, screenVBO;
	{
		glGenVertexArrays(1, &screenVAO);
		glGenBuffers(1, &screenVBO);

		glBindVertexArray(screenVAO);

		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(screenPlaneVertices) + sizeof(screenTextureCoords), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(screenPlaneVertices), &screenPlaneVertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(screenPlaneVertices), sizeof(screenTextureCoords), &screenTextureCoords);


		// Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// Атрибут текстурных координат
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(screenPlaneVertices)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	// VAO и VBO для куба-объекта
	unsigned int VAO, VBO;
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals) + sizeof(cubeVerticesTextureCoords), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVerticesPositions), &cubeVerticesPositions);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions), sizeof(cubeVerticesNormals), &cubeVerticesNormals);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals), sizeof(cubeVerticesTextureCoords), &cubeVerticesTextureCoords);
		

		// Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// Атрибут нормалей
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVerticesPositions)));
		glEnableVertexAttribArray(1);

		// Атрибут текстурных координат
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(cubeVerticesPositions)+sizeof(cubeVerticesNormals)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

	}


	// VAO и VBO для куба-источника света
	unsigned int lightCubeVAO, lightCubeVBO;
	{
		glGenVertexArrays(1, &lightCubeVAO);
		glGenBuffers(1, &lightCubeVBO);

		glBindVertexArray(lightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVerticesPositions), &cubeVerticesPositions);

		// Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	unsigned int floorVAO, floorVBO;
	{
		glGenVertexArrays(1, &floorVAO);
		glGenBuffers(1, &floorVBO);

		glBindVertexArray(floorVAO);

		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerticesPositions) + sizeof(floorNormals) + sizeof(floorTextureCoords), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floorVerticesPositions), &floorVerticesPositions);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(floorVerticesPositions), sizeof(floorNormals), &floorNormals);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(floorVerticesPositions) + sizeof(floorNormals), sizeof(floorTextureCoords), &floorTextureCoords);

		// Координатный атрибут
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// Атрибут нормалей
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(floorVerticesPositions)));
		glEnableVertexAttribArray(1);

		// Атрибут текстурных координат
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(floorVerticesPositions) + sizeof(floorNormals)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	Shader ourShader("../midnight/shader.vs", "../midnight/shader.fs", NULL);
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs", NULL);
	Shader shadowShader("../midnight/shadow.vs", "../midnight/shadow.fs", NULL);
	Shader testShader("../midnight/test.vs", "../midnight/test.fs", NULL);

	unsigned int diffuseMap = loadTexture("../res/box.png");
	unsigned int specularMap = loadTexture("../res/specular_map.png");
	unsigned int floorTexture = loadTexture("../res/floor.png");

	glm::vec3 pointLightPosition = glm::vec3(-2.0f, 2.0f, -1.0f);

	// отрисовывать кадр при каждом обновлении экрана 
	glfwSwapInterval(1);

	float lastTime = glfwGetTime(); // Переменная хранящая время начала работы программы
	int nbFrames = 0;

	// Буффер для карты теней
	unsigned int depthMapFBO;
	unsigned int depthMap;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	{
		glGenFramebuffers(1, &depthMapFBO);

		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	glm::vec3 scaleOfFloor(13.0f, 1.0f, 13.0f);
	

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
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
		}

		// Обработка ввода
		processInput(window);


		glEnable(GL_MULTISAMPLE);

		glEnable(GL_DEPTH_TEST);

		
		glEnable(GL_CULL_FACE); //Включить режим отсечения граней
		glCullFace(GL_BACK);	// отсечение задних граней
		glFrontFace(GL_CCW);	// Передняя грань определяется "против часовой" стрелки
		

		glm::mat4 lightSpaceMatrix;

		// Проход для расчёта теней
		{
			// Настройка матриц для теневой карты
			
			{
				float near_plane = 1.0f, far_plane = 7.5f;

				glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

				glm::mat4 lightView = glm::lookAt(pointLightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				lightSpaceMatrix = lightProjection * lightView;
			}


			shadowShader.use();
			shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 model;


			// Рендеринг куба-объекта
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, positionOfBox);
				shadowShader.setMat4("model", model);

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// Рендеринг пола
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
				model = glm::scale(model, scaleOfFloor);
				shadowShader.setMat4("model", model);

				glBindVertexArray(floorVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			

		}
		

		// Проход основной
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

			//настройка основных матриц: вида и проекции
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 model;


			//Передача основных матриц и позиции камеры шейдеру
			{
				ourShader.use();
				ourShader.setMat4("projection", projection);
				ourShader.setMat4("view", view);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);
				ourShader.setInt("texture_diffuse1", 0);

				glActiveTexture(GL_TEXTURE1);
				ourShader.setInt("shadowMap", 1);
				glBindTexture(GL_TEXTURE_2D, depthMap);


				ourShader.setVec3("lightPos", pointLightPosition);
				ourShader.setVec3("viewPos", camera.Position);
				ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			}


			// Рендеринг куба-объекта
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, positionOfBox);
				ourShader.setMat4("model", model);



				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// Рендеринг пола
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
				model = glm::scale(model, scaleOfFloor);
				ourShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, floorTexture);
				ourShader.setInt("texture_diffuse1", 0);

				glBindVertexArray(floorVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			
		}
		

		

		// glfw: обмен содержимым переднего и заднего буферов. Опрос событий ввода\вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

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

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		positionOfBox.x = positionOfBox.x - 0.01f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		positionOfBox.x = positionOfBox.x + 0.01f;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		positionOfBox.y = positionOfBox.y - 0.01f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		positionOfBox.y = positionOfBox.y + 0.01f;
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Установка параметров фильтрации текстуры
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);





		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}