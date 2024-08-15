#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader_class.h"
#include "stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../model.h"
#include "../mesh.h"

#include "camera.h"
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
float lerp(float a, float b, float f);


// ���������
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
float exposure = 0.1f;

float deltaTime = 0.0f;	// ����� ����� ������� � ��������� �������
float lastFrame = 0.0f; // ����� ���������� �����

float modelHigh = 1.0f;

int main()
{
	setlocale(LC_ALL, "Russian");
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


	// ����������
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);

	// ���������� ����������
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	// ������ �������
	glm::vec3 nm(0.0f, 0.0f, 1.0f);


	float quadVertices[] = {
		// ����������           // �������        // �����. ����������
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y
	};


	// VAO � VBO ��� ��������������-�������
	unsigned int VAO, VBO;
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);


		// ������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// �������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// ������� ���������� ���������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		glBindVertexArray(0);

	}

	float cubeVerticesPositions[] =
	{
		/*--------------------------------------------------------------------*/

			 0.5f,  0.5f, -0.5f,	// 2
			 0.5f, -0.5f, -0.5f,	// 1
			-0.5f, -0.5f, -0.5f,	// 0

			-0.5f, -0.5f, -0.5f,	// 0
			-0.5f,  0.5f, -0.5f,	// 3
			 0.5f,  0.5f, -0.5f,	// 2
			 /*--------------------------------------------------------------------*/

				 -0.5f, -0.5f,  0.5f,	// 4
				  0.5f, -0.5f,  0.5f,	// 5
				  0.5f,  0.5f,  0.5f,	// 6

				  0.5f,  0.5f,  0.5f,	// 6
				 -0.5f,  0.5f,  0.5f,	// 7
				 -0.5f, -0.5f,  0.5f,	// 4
				 /*--------------------------------------------------------------------*/

					 -0.5f,  0.5f,  0.5f,	// 7
					 -0.5f,  0.5f, -0.5f,	// 3
					 -0.5f, -0.5f, -0.5f,	// 0

					 -0.5f, -0.5f, -0.5f,	// 0
					 -0.5f, -0.5f,  0.5f,	// 4
					 -0.5f,  0.5f,  0.5f,	// 7 
					 /*--------------------------------------------------------------------*/

						  0.5f, -0.5f, -0.5f,	// 1
						  0.5f,  0.5f, -0.5f,	// 2
						  0.5f,  0.5f,  0.5f,	// 6

						  0.5f,  0.5f,  0.5f,	// 6
						  0.5f, -0.5f,  0.5f,	// 5
						  0.5f, -0.5f, -0.5f,	// 1
						  /*--------------------------------------------------------------------*/

							  -0.5f, -0.5f, -0.5f,	// 0
							   0.5f, -0.5f, -0.5f,	// 1
							   0.5f, -0.5f,  0.5f,	// 5

							   0.5f, -0.5f,  0.5f,  	// 5
							  -0.5f, -0.5f,  0.5f,  	// 4
							  -0.5f, -0.5f, -0.5f, 	// 0
							  /*--------------------------------------------------------------------*/

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

	// VAO ��� ����������� ����
	unsigned int cubeVAO, cubeVBO;
	{
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVerticesPositions), &cubeVerticesPositions);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions), sizeof(cubeVerticesNormals), &cubeVerticesNormals);

		// ������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// �������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVerticesPositions)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}


	Shader ourShader("../midnight/shader.vs", "../midnight/shader.fs", NULL);
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs", NULL);
	Shader screenShader("../midnight/screenShader.vs", "../midnight/screenShader.fs", NULL);
	Shader shaderSSAO("../midnight/screenShader.vs", "../midnight/shaderSSAO.fs", NULL);

	unsigned int floorAndWall = loadTexture("../res/floor.png");
	unsigned int box = loadTexture("../res/box.png");
	//	stbi_set_flip_vertically_on_load(true);
	unsigned int saha = loadTexture("../res/saha.png");

	Model backpack("C:/Users/tyuri/Documents/GitHub/res/models/backpack/backpack.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// ��������� ����������� ��� ����������� ����������
	unsigned int gBuffer, gPosition, gNormal, gColorSpec;
	{
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// �������� ����� �������
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// �������� ����� ��������
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// �������� ����� ����� + ���������� ������������
		glGenTextures(1, &gColorSpec);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

		// �������� OpenGL, ����� ������������� �������� ����� (���������������� �����������) ���������� ������������ ��� ����������
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// �������� ������� ������������ ��� ������������� �������� ������� � ��������� (������������� �� �� ����� ����� ���������)
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT); // ������������� ������ ������� ������������ ��� ������� ������� � ���������
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo); // ������ ����������� ��� ����

		// ������, ����� �� ������� ���������� � ���������� ��� ����������� �������, ��������� ���������� ������������ �����������
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// ssao ����������
	unsigned int ssaoFBO, ssaoColorBuffer;
	{
		glGenFramebuffers(1, &ssaoFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);


		glGenTextures(1, &ssaoColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	}


	// ���������
	glm::vec3 lightPos[3], lightColor[3];
	{
		lightPos[0] = glm::vec3(5.0f, 1.0f, 1.0f);
		lightPos[1] = glm::vec3(3.0f, 1.0f, -13.0f);
		lightPos[2] = glm::vec3(-5.0f, 1.0f, 2.0f);

		lightColor[0] = glm::vec3(0.7f, 0.0f, 0.0f);
		lightColor[1] = glm::vec3(0.0f, 0.7f, 0.0f);
		lightColor[2] = glm::vec3(0.0f, 0.0f, 0.7f);
	}


	// ������������ ���� ��� ������ ���������� ������ 
	glfwSwapInterval(1);

	float lastTime = glfwGetTime(); // ���������� �������� ����� ������ ������ ���������
	int nbFrames = 0;


	// ���������������� ��������� ������ ��� ��������� ��������� � ������ �������
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // ��������� �������� ���� float �� ��������� [0.0, 1.0]
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		float scale = (float)i / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	unsigned int noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	shaderSSAO.use();
	shaderSSAO.setInt("gPosition", 0);
	shaderSSAO.setInt("gNormal", 1);
	shaderSSAO.setInt("texNoise", 2);

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

		//	glEnable(GL_FRAMEBUFFER_SRGB); // �������������� ����� ���������
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		// ������ � ��������� ����������
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		{
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

			glCullFace(GL_BACK);	// ��������� ������ ������
			glFrontFace(GL_CCW);	// �������� ����� ������������ "������ �������" �������

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//�������� �������� ������ � ������� ������ �������
			{
				ourShader.use();
				ourShader.setMat4("projection", projection);
				ourShader.setMat4("view", view);
				ourShader.setInt("texture_diffuse1", 0);
			}

			// ��������� �������
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, saha);

				// ����� ������
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -26.0f));
				ourShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, floorAndWall);

				// ������ ������ ������
				model = glm::mat4(1.0f);

				model = glm::translate(model, glm::vec3(1.0f, 0.0f, -14.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(12.0f, 1.0f, 1.0f));
				ourShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ����� ������ ������
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -14.0f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(12.0f, 1.0f, 1.0f));
				ourShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ������� ������ ������
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 1.0f, -14.0f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.0f, 12.0f, 1.0f));
				ourShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ������ ������ ������
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.0f, -14.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.0f, 12.0f, 1.0f));
				ourShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ���
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 1.0f));
				ourShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ������ �������� ������
				model = glm::mat4(1.0f);

				model = glm::translate(model, glm::vec3(4.0f, 1.0f, -1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				ourShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, box);

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ��������� ���� ��� ���������
				{
					model = glm::mat4(1.0f);
					glm::vec3 cubePose = glm::vec3(lightPos[0].x, lightPos[0].y - 1.0f, lightPos[0].z);
					model = glm::translate(model, cubePose);

					ourShader.setMat4("model", model);

					glBindVertexArray(cubeVAO);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}

				// ��������� �������
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(5.0f, modelHigh, -5.0f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					ourShader.setMat4("model", model);
					backpack.Draw(ourShader);
				}
			}

		}

		// ��������� ssao
		{
			glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, noiseTexture);
			shaderSSAO.use();


			//	SendKernelSamplesToShader();
			for (int i = 0; i < 64; ++i)
			{
				shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
			}



			shaderSSAO.setMat4("projection", projection);
			//	RenderQuad();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// ��������� ��������� ��������������
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			screenShader.use();
			screenShader.setInt("gPosition", 0);
			screenShader.setInt("gNormal", 1);
			screenShader.setInt("gColorSpec", 2);
			screenShader.setInt("ssao", 3);
			screenShader.setVec3("viewPos", camera.Position);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gColorSpec);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

			// ���������� ���������
			{


				screenShader.setVec3("lights[0].Position", lightPos[0]);
				screenShader.setVec3("lights[1].Position", lightPos[1]);
				screenShader.setVec3("lights[2].Position", lightPos[2]);



				screenShader.setVec3("lights[0].Color", lightColor[0]);
				screenShader.setVec3("lights[1].Color", lightColor[1]);
				screenShader.setVec3("lights[2].Color", lightColor[2]);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// ��������� ���������� ���������
			{
				glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // ������ � �������� �� ��������� ����������
				glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				lightCubeShader.use();
				lightCubeShader.setMat4("projection", projection);
				lightCubeShader.setMat4("view", view);

				for (unsigned int i = 0; i < sizeof(lightPos) / sizeof(lightPos[0]); i++)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, lightPos[i]);
					model = glm::scale(model, glm::vec3(0.25f));
					lightCubeShader.setMat4("model", model);
					lightCubeShader.setVec3("color", lightColor[i]);

					glBindVertexArray(lightCubeVAO);
					glDrawArrays(GL_TRIANGLES, 0, 36);;
				}
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		modelHigh = modelHigh + 0.01f;
		std::cout << modelHigh << "\n";
	}


	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		modelHigh = modelHigh - 0.01f;
		std::cout << modelHigh << "\n";
	}
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

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}