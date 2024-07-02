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


// ���������
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

float deltaTime = 0.0f;	// ����� ����� ������� � ��������� �������
float lastFrame = 0.0f; // ����� ���������� �����

glm::vec3 positionOfBox(0.0f, 0.0f, 0.0f);


int main()
{
	std::cout << "Naked program!\n";

	// glfw: ������������� � ����������������
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	// �������� ��������������
	glfwWindowHint(GLFW_SAMPLES, 4);


	// glfw: �������� ����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// callback-�������
	{
		glfwMakeContextCurrent(window);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// glad: �������� ���� ���������� �� OpenGL-�������
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// ���������� ������ ����
	float cubeVerticesPositions[] =
	{
		/*--------------------------------------------------------------------*/
			// ������� ����� (������ �������)
			 0.5f,  0.5f, -0.5f,	// 2
			 0.5f, -0.5f, -0.5f,	// 1
			-0.5f, -0.5f, -0.5f,	// 0

			-0.5f, -0.5f, -0.5f,	// 0
			-0.5f,  0.5f, -0.5f,	// 3
			 0.5f,  0.5f, -0.5f,	// 2
			 /*--------------------------------------------------------------------*/
				 // ������� ����� (������ �������)
				 -0.5f, -0.5f,  0.5f,	// 4
				  0.5f, -0.5f,  0.5f,	// 5
				  0.5f,  0.5f,  0.5f,	// 6

				  0.5f,  0.5f,  0.5f,	// 6
				 -0.5f,  0.5f,  0.5f,	// 7
				 -0.5f, -0.5f,  0.5f,	// 4
				 /*--------------------------------------------------------------------*/
					 // ����� ����� (������ �������)
					 -0.5f,  0.5f,  0.5f,	// 7
					 -0.5f,  0.5f, -0.5f,	// 3
					 -0.5f, -0.5f, -0.5f,	// 0

					 -0.5f, -0.5f, -0.5f,	// 0
					 -0.5f, -0.5f,  0.5f,	// 4
					 -0.5f,  0.5f,  0.5f,	// 7 
					 /*--------------------------------------------------------------------*/
						  // ������ ����� (������ �������)
						  0.5f, -0.5f, -0.5f,	// 1
						  0.5f,  0.5f, -0.5f,	// 2
						  0.5f,  0.5f,  0.5f,	// 6

						  0.5f,  0.5f,  0.5f,	// 6
						  0.5f, -0.5f,  0.5f,	// 5
						  0.5f, -0.5f, -0.5f,	// 1
						  /*--------------------------------------------------------------------*/
							  // ������ ����� (������ �������)
							  -0.5f, -0.5f, -0.5f,	// 0
							   0.5f, -0.5f, -0.5f,	// 1
							   0.5f, -0.5f,  0.5f,	// 5

							   0.5f, -0.5f,  0.5f,  	// 5
							  -0.5f, -0.5f,  0.5f,  	// 4
							  -0.5f, -0.5f, -0.5f, 	// 0
							  /*--------------------------------------------------------------------*/
								   // ������� ����� (������ �������)
								   0.5f,  0.5f,  0.5f,  	// 6
								   0.5f,  0.5f, -0.5f,	// 2
								  -0.5f,  0.5f, -0.5f,	// 3	

								  -0.5f,  0.5f, -0.5f,	// 3
								  -0.5f,  0.5f,  0.5f,	// 7
								   0.5f,  0.5f,  0.5f		// 6
								   /*--------------------------------------------------------------------*/
	};

	// ������� ����
	float cubeVerticesNormals[] =
	{
		/*--------------------------------------------------------------------*/
			// ������� ����� (������ �������)
			 0.0f,  0.0f, -1.0f,	// 2	
			 0.0f,  0.0f, -1.0f,	// 1
			 0.0f,  0.0f, -1.0f,	// 0

			 0.0f,  0.0f, -1.0f,	// 0
			 0.0f,  0.0f, -1.0f,	// 3
			 0.0f,  0.0f, -1.0f,	// 2

			 /*--------------------------------------------------------------------*/
				 // ������� ����� (������ �������)
				  0.0f,  0.0f, 1.0f,		// 4	
				  0.0f,  0.0f, 1.0f,		// 5
				  0.0f,  0.0f, 1.0f,		// 6

				  0.0f,  0.0f, 1.0f,		// 6
				  0.0f,  0.0f, 1.0f,		// 7
				  0.0f,  0.0f, 1.0f,		// 4

				  /*--------------------------------------------------------------------*/
					  // ����� ����� (������ �������)
					   -1.0f,  0.0f,  0.0f,	// 7	
					   -1.0f,  0.0f,  0.0f,	// 3
					   -1.0f,  0.0f,  0.0f,	// 0

					   -1.0f,  0.0f,  0.0f,	// 0
					   -1.0f,  0.0f,  0.0f,	// 4
					   -1.0f,  0.0f,  0.0f,	// 7

					   /*--------------------------------------------------------------------*/
						   // ������ ����� (������ �������)
							1.0f,  0.0f,  0.0f,	// 1	
							1.0f,  0.0f,  0.0f,	// 2
							1.0f,  0.0f,  0.0f,	// 6	

							1.0f,  0.0f,  0.0f,	// 6
							1.0f,  0.0f,  0.0f,	// 5
							1.0f,  0.0f,  0.0f,	// 1

							/*--------------------------------------------------------------------*/
								// ������ ����� (������ �������)
								 0.0f, -1.0f,  0.0f, 	// 0	
								 0.0f, -1.0f,  0.0f,	// 1
								 0.0f, -1.0f,  0.0f,	// 5

								 0.0f, -1.0f,  0.0f,	// 5
								 0.0f, -1.0f,  0.0f,	// 4
								 0.0f, -1.0f,  0.0f,	// 0

								 /*--------------------------------------------------------------------*/
									 // ������� ����� (������ �������)
									  0.0f,  1.0f,  0.0f,	// 6	
									  0.0f,  1.0f,  0.0f,	// 2
									  0.0f,  1.0f,  0.0f,	// 3	

									  0.0f,  1.0f,  0.0f,	// 3
									  0.0f,  1.0f,  0.0f,	// 7
									  0.0f,  1.0f,  0.0f	// 6

									  /*--------------------------------------------------------------------*/
	};

	// ���������� ���������� ����
	float cubeVerticesTextureCoords[] =
	{
		/*--------------------------------------------------------------------*/
		// ������� ����� (������ �������)
		1.0f, 1.0f,	// 2	
		1.0f, 0.0f,	// 1
		0.0f, 0.0f,	// 0

		0.0f, 0.0f,	// 0
		0.0f, 1.0f,   // 3
		1.0f, 1.0f,   // 2

		/*--------------------------------------------------------------------*/
		 // ������� ����� (������ �������)
		0.0f, 0.0f,	// 4	
		1.0f, 0.0f,	// 5
		1.0f, 1.0f,	// 6

		1.0f, 1.0f,	// 6
		0.0f, 1.0f,	// 7
		0.0f, 0.0f,	// 4

		/*--------------------------------------------------------------------*/
		 // ����� ����� (������ �������)
		1.0f, 0.0f,	// 7	
		1.0f, 1.0f,	// 3
		0.0f, 1.0f,	// 0

		0.0f, 1.0f,	// 0
		0.0f, 0.0f,	// 4
		1.0f, 0.0f,	// 7

		/*--------------------------------------------------------------------*/
		 // ������ ����� (������ �������)
		0.0f, 1.0f,	// 1	
		1.0f, 1.0f,	// 2
		1.0f, 0.0f,	// 6	

		1.0f, 0.0f,	// 6
		0.0f, 0.0f,	// 5
		0.0f, 1.0f,	// 1

		/*--------------------------------------------------------------------*/
		 // ������ ����� (������ �������)
		0.0f, 1.0f, 	// 0	
		1.0f, 1.0f,	// 1
		1.0f, 0.0f,	// 5

		1.0f, 0.0f,	// 5
		0.0f, 0.0f,	// 4
		0.0f, 1.0f,	// 0

		/*--------------------------------------------------------------------*/
		 // ������� ����� (������ �������)
		1.0f, 0.0f,	// 6	
		1.0f, 1.0f,	// 2
		0.0f, 1.0f,	// 3	

		0.0f, 1.0f,	// 3
		0.0f, 0.0f,	// 7
		1.0f, 0.0f	// 6

		/*--------------------------------------------------------------------*/
	};

	// ���������� ������ ����
	float floorVerticesPositions[] =
	{
		// ������� ����� (������ �������)
		 0.5f,  0.5f,  0.5f,	// 6
		 0.5f,  0.5f, -0.5f,	// 2
		-0.5f,  0.5f, -0.5f,	// 3	

		-0.5f,  0.5f, -0.5f,	// 3
		-0.5f,  0.5f,  0.5f,	// 7
		 0.5f,  0.5f,  0.5f		// 6
	};

	// ���������� ���������� ����
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
		// ������� ����� (������ �������)
									  0.0f,  1.0f,  0.0f,	// 6	
									  0.0f,  1.0f,  0.0f,	// 2
									  0.0f,  1.0f,  0.0f,	// 3	

									  0.0f,  1.0f,  0.0f,	// 3
									  0.0f,  1.0f,  0.0f,	// 7
									  0.0f,  1.0f,  0.0f	// 6
	};

	// ���������� ������ ��������� ��������������
	float screenPlaneVertices[] =
	{
			-0.5f, -0.5f, 0.5f,	// 4
			 0.5f, -0.5f, 0.5f,	// 5
			 0.5f, 0.5f, 0.5f,	// 6

			 0.5f, 0.5f, 0.5f,	// 6
			-0.5f, 0.5f, 0.5f,	// 7
			-0.5f, -0.5f, 0.5f,	// 4
	};

	// ���������� ������� ��������� ��������������
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


		// ������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// ������� ���������� ���������
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(screenPlaneVertices)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	// VAO � VBO ��� ����-�������
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
		

		// ������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// ������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVerticesPositions)));
		glEnableVertexAttribArray(1);

		// ������� ���������� ���������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(cubeVerticesPositions)+sizeof(cubeVerticesNormals)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

	}


	// VAO � VBO ��� ����-��������� �����
	unsigned int lightCubeVAO, lightCubeVBO;
	{
		glGenVertexArrays(1, &lightCubeVAO);
		glGenBuffers(1, &lightCubeVBO);

		glBindVertexArray(lightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVerticesPositions), &cubeVerticesPositions);

		// ������������ �������
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

		// ������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// ������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(floorVerticesPositions)));
		glEnableVertexAttribArray(1);

		// ������� ���������� ���������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(floorVerticesPositions) + sizeof(floorNormals)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	Shader ourShader("../midnight/shader.vs", "../midnight/shader.fs", NULL);
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs", NULL);
	Shader shadowShader("../midnight/shadow.vs", "../midnight/shadow.fs", "../midnight/shadow.gs");

	unsigned int diffuseMap = loadTexture("../res/box.png");
	unsigned int specularMap = loadTexture("../res/specular_map.png");
	unsigned int floorTexture = loadTexture("../res/floor.png");

	glm::vec3 pointLightPosition = glm::vec3(-2.0f, 2.0f, -1.0f);

	// ������������ ���� ��� ������ ���������� ������ 
	glfwSwapInterval(1);

	float lastTime = glfwGetTime(); // ���������� �������� ����� ������ ������ ���������
	int nbFrames = 0;



	// ��������� ���������� ����� �����
	unsigned int depthMapFBO;
	unsigned int depthCubemap;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	{
		glGenFramebuffers(1, &depthMapFBO);

		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	



	
	glm::vec3 scaleOfFloor(13.0f, 1.0f, 13.0f);
	

	// ���������������� ��������� ������ ��� ��������� ��������� � ������ �������
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ���� ����������
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime(); // ����� �������� ����� � ������ ������ ���������
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			std::cout << "FPS: " << double(nbFrames) << "\n";
			nbFrames = 0;
			lastTime += 1.0;
		}

		//������ ������� ��� ������������ �������� ��������
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
		}

		// ��������� �����
		processInput(window);


		glEnable(GL_MULTISAMPLE);

		glEnable(GL_DEPTH_TEST);

		float time = glfwGetTime();
		pointLightPosition.x = sin(time) * 2;
		pointLightPosition.z = cos(time) * 2;
		
		// ������ ������ ������������� ���������� ����� �������
		float near_plane = 1.0f, far_plane = 25.0f;
		float near = 1.0f;
		float far = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj* glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj* glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj* glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj* glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj* glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj* glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		

		
		// ������ ��� ������� �����
		{
			// ��������� ������ ��� ������� �����
				

			glEnable(GL_CULL_FACE); //�������� ����� ��������� ������
			glCullFace(GL_FRONT);	// ��������� ������ ������
			glFrontFace(GL_CCW);	// �������� ����� ������������ "������ �������" �������

			


			shadowShader.use();
			shadowShader.setFloat("far_plane", far_plane);
			shadowShader.setVec3("lightPos", pointLightPosition);

			shadowShader.setMat4("shadowMatrices[0]", shadowTransforms[0]);
			shadowShader.setMat4("shadowMatrices[1]", shadowTransforms[1]);
			shadowShader.setMat4("shadowMatrices[2]", shadowTransforms[2]);
			shadowShader.setMat4("shadowMatrices[3]", shadowTransforms[3]);
			shadowShader.setMat4("shadowMatrices[4]", shadowTransforms[4]);
			shadowShader.setMat4("shadowMatrices[5]", shadowTransforms[5]);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 model;


			// ��������� ����-�������
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, positionOfBox);
				shadowShader.setMat4("model", model);

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// ��������� ����
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
				model = glm::scale(model, scaleOfFloor);
				shadowShader.setMat4("model", model);

				glBindVertexArray(floorVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			

		}
		

		// ������ ��������
		{
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			glCullFace(GL_BACK);	// ��������� ������ ������
			glFrontFace(GL_CCW);	// �������� ����� ������������ "������ �������" �������

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			

			//��������� �������� ������: ���� � ��������
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 model;


			//�������� �������� ������ � ������� ������ �������
			{
				ourShader.use();
				ourShader.setMat4("projection", projection);
				ourShader.setMat4("view", view);

				glActiveTexture(GL_TEXTURE0);
				ourShader.setInt("depthMap", 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);


				ourShader.setVec3("lightPos", pointLightPosition);
				ourShader.setVec3("viewPos", camera.Position);
				ourShader.setFloat("far_plane", far_plane);
			}


			// ��������� ����-�������
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, positionOfBox);
				ourShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);
				ourShader.setInt("texture_diffuse1", 1);


				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}


			// ��������� ����
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
				model = glm::scale(model, scaleOfFloor);
				ourShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, floorTexture);
				ourShader.setInt("texture_diffuse1", 1);

				glBindVertexArray(floorVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			 

			// ��������� ���� ��������� �����
			{
				model = glm::mat4(1.0f);

				model = glm::translate(model, pointLightPosition);

				model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

				lightCubeShader.use();
				lightCubeShader.setMat4("model", model);
				lightCubeShader.setMat4("projection", projection);
				lightCubeShader.setMat4("view", view);



				glBindVertexArray(lightCubeVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			
		}
		

		

		// glfw: ����� ���������� ��������� � ������� �������. ����� ������� �����\������ (���� �� ������/�������� ������, ��������� ������ ���� � �.�.)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// glfw: ����������, ������������ ���� ����� ��������������� GLFW-��������
	glfwTerminate();
	return 0;
}

// ��������� ���� ������� �����: ������ GLFW � �������/���������� ������ ���� � ������ ����� � ��������������� ��������� ������ �������
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

// glfw: ������ ���, ����� ���������� ������� ���� (������������� ��� ������������ ��������), ���������� ������ callback-�������
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// ����������, ��� ���� ��������� ������������� ����� �������� ����.
	// �������� ��������, ��� ������ � ������ ����� ����������� ������, ��� �������, �� Retina-��������
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
	float yoffset = lastY - ypos; // �����������, ��� ��� y-���������� ���� ����� �����

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
		// ��������� ���������� ���������� ��������
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