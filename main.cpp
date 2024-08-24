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

float deltaTime = 0.0f;	// ����� ����� ������� � ��������� �������
float lastFrame = 0.0f; // ����� ���������� �����

float modelHigh = 1.0f;
int power = 65;

int main()
{
	setlocale(LC_ALL, "Russian");

	// glfw: ������������� � ����������������
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

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

	Shader geometricShader("../midnight/shader.vs", "../midnight/shader.fs", NULL);  // ������ ��� ��������������� �������
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs", NULL); // ������ ��� ��������� �����-���������� �����
	Shader shaderSSAO("../midnight/screenShader.vs", "../midnight/shaderSSAO.fs", NULL); // ������ ��� ��������� ssao ��������
	Shader resultSSAOShader("../midnight/screenShader.vs", "../midnight/ssaoResultShader.fs", NULL); // ������ ��� ��������� ���������� ssao
	Shader twiseScreenShader("../midnight/screenShader.vs", "../midnight/twiseScreenShader.fs", NULL); // ������ ��� ���������� ssao-�������� �������� � ���������� ���������
	Shader screenShader("../midnight/screenShader.vs", "../midnight/screenShader.fs", NULL); // ������ ��� ���������� ssao-�������� �������� � ���������� ���������
	Shader shaderBlur("../midnight/screenShader.vs", "../midnight/blur.fs", NULL); // ������ ��� ��������
	Shader shadowShader("../midnight/shaderForShadow.vs", "../midnight/shadow.fs", "../midnight/geometricShadow.gs"); // ������ ��� ��������


	unsigned int floor = loadTexture("C:\\Users\\Tyurin\\Documents\\GitHub\\res\\floor.png");

	stbi_set_flip_vertically_on_load(true);
	Model backpack("C:/Users/Tyurin/Documents/GitHub/res/models/backpack/backpack.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);




	// ��������� ����������� ��� ����������� ����������
	unsigned int gBuffer, gPosition, gNormal, gColorSpec, gWorldPosition;
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

		// �������� ����� ������� �������
		glGenTextures(1, &gWorldPosition);
		glBindTexture(GL_TEXTURE_2D, gWorldPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gWorldPosition, 0);
		

		// �������� OpenGL, ����� ������������� �������� ����� (���������������� �����������) ���������� ������������ ��� ����������
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

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

	// ��������� ����������� ��������� �������� ������ � �������� ssao, � ������ �������� �������� ������� ������������ �������
	unsigned int ssaoFinalFBO, ssaoScreenTexture;
	{
		glGenFramebuffers(1, &ssaoFinalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFinalFBO);


		glGenTextures(1, &ssaoScreenTexture);
		glBindTexture(GL_TEXTURE_2D, ssaoScreenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoScreenTexture, 0);

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

	// ��������� ����������� ��� ������� ��������
	unsigned int forLightBuffer, usualScreenTexture, overLightScreenTexture;
	{
		glGenFramebuffers(1, &forLightBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, forLightBuffer);

		// ������ �������� ��� ��������
		glGenTextures(1, &usualScreenTexture);
		glBindTexture(GL_TEXTURE_2D, usualScreenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, usualScreenTexture, 0);

		// ������ �������� ��� ��������
		glGenTextures(1, &overLightScreenTexture);
		glBindTexture(GL_TEXTURE_2D, overLightScreenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, overLightScreenTexture, 0);

		// �������� ������� ������������ ��� ������������� �������� ������� � ��������� (������������� �� �� ����� ����� ���������)
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT); // ������������� ������ ������� ������������ ��� ������� ������� � ���������
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo); // ������ ����������� ��� ����

		// �������� OpenGL, ����� ������������� �������� ����� (���������������� �����������) ���������� ������������ ��� ����������
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		// ������, ����� �� ������� ���������� � ���������� ��� ����������� �������, ��������� ���������� ������������ �����������
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
	{
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongBuffer);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
			);
		}
	}

	// ����������� ��� �������� �����
	unsigned int depthMapFBO, depthCubemap;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	{
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthCubemap);

		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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



	// ���������
	glm::vec3 lightPos, lightColor;
	{
		lightPos = glm::vec3(5.0f, 1.0f, 1.0f);
		lightColor = glm::vec3(2.0f);
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
	{
		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	shaderSSAO.use();
	shaderSSAO.setInt("gPosition", 0);
	shaderSSAO.setInt("gNormal", 1);
	shaderSSAO.setInt("texNoise", 2);

	std::cout << glGetError() << std::endl; // ������ ��� 0 (no error)


	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float near = 1.0f;
	float far = 25.0f;
	

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
		lightPos.x = 10 * sin(glfwGetTime());
		lightPos.z = 10 * cos(glfwGetTime());

		//	glEnable(GL_FRAMEBUFFER_SRGB); // �������������� ����� ���������
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);


		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

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
				geometricShader.use();
				geometricShader.setMat4("projection", projection);
				geometricShader.setMat4("view", view);
			}

			// ��������� ��������
			{

				// ���
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 1.0f));
				geometricShader.setMat4("model", model);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, floor);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);


				// ��������� �������
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(5.0f, modelHigh, -5.0f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					geometricShader.setMat4("model", model);
					backpack.Draw(geometricShader);
				}
			}

		}



		// ��������� ����� ��� ��������� �����
		{
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			
			shadowShader.use();
			shadowShader.setVec3("lightPos", lightPos);
			shadowShader.setFloat("far_plane", far);
			for (int i = 0; i < 6; ++i)
			{
				shadowShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
			}

			// ��������� ��������
			{
				// ���
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 1.0f));
				shadowShader.setMat4("model", model);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);


				// ��������� �������
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(5.0f, modelHigh, -5.0f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					shadowShader.setMat4("model", model);
					backpack.Draw(shadowShader);
				}
			}

		}

		// ��������� ssao
		{
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, noiseTexture);
			shaderSSAO.use();
			shaderSSAO.setInt("power", power);

			for (int i = 0; i < 32; ++i)
			{
				shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
			}

			shaderSSAO.setMat4("projection", projection);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// ��������� ������ � �������� SSAO, �� ��� ���������� �����
		{
			glBindFramebuffer(GL_FRAMEBUFFER, ssaoFinalFBO);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			resultSSAOShader.use();
			resultSSAOShader.setInt("gPosition", 0);
			resultSSAOShader.setInt("gNormal", 1);
			resultSSAOShader.setInt("gColorSpec", 2);
			resultSSAOShader.setInt("ssao", 3);
			resultSSAOShader.setInt("depthMap", 4);
			resultSSAOShader.setInt("gWorldPosition", 5);

			resultSSAOShader.setFloat("far_plane", far);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gColorSpec);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, gWorldPosition);


			// ���������� ���������
			{
				glm::vec3 lightPosView = glm::vec3(view * glm::vec4(lightPos, 1.0));
				resultSSAOShader.setVec3("lights.Position", lightPos);
				resultSSAOShader.setVec3("lights.Color", lightColor);



				const float linear = 0.022;
				const float quadratic = 0.0019;

				resultSSAOShader.setFloat("lights.Linear", linear);
				resultSSAOShader.setFloat("lights.Quadratic", quadratic);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6); // ����������� �������� ssaoScreenTexture, ������� ������ ����� � ssao �������, �� ��� ����������� ���������� ���������

			// ��������� ���������� ��������� � �������� ssaoScreenTexture
			{
				glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ssaoFinalFBO); // ������ ������ ������� � �������� ������ � ssao-��������
				glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				glBindFramebuffer(GL_FRAMEBUFFER, ssaoFinalFBO);

				lightCubeShader.use();
				lightCubeShader.setMat4("projection", projection);
				lightCubeShader.setMat4("view", view);

				model = glm::mat4(1.0f);
				model = glm::translate(model, lightPos);
				model = glm::scale(model, glm::vec3(0.25f));
				lightCubeShader.setMat4("model", model);
				lightCubeShader.setVec3("color", lightColor);

				glBindVertexArray(lightCubeVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);;
			}
			glDisable(GL_DEPTH_TEST);

			// ��������� �������� �� ������ "�������� ssao-����� + ��������� ���������"
			glBindFramebuffer(GL_FRAMEBUFFER, forLightBuffer);


			glClear(GL_COLOR_BUFFER_BIT);

			twiseScreenShader.use();
			twiseScreenShader.setInt("ssaoScreenTexture", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ssaoScreenTexture);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// �������� �������� ��������������� ������
			bool horizontal = true, first_iteration = true;
			{
				int amount = 10;
				shaderBlur.use();
				shaderBlur.setInt("image", 0);
				for (unsigned int i = 0; i < amount; i++)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
					shaderBlur.setInt("horizontal", horizontal);
					glBindTexture(
						GL_TEXTURE_2D, first_iteration ? overLightScreenTexture : pingpongBuffer[!horizontal]
					);

					glBindVertexArray(VAO);
					glDrawArrays(GL_TRIANGLES, 0, 6);

					horizontal = !horizontal;
					if (first_iteration)
						first_iteration = false;
				}
			}

			// ��������� �� �����
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

				glClear(GL_COLOR_BUFFER_BIT);

				screenShader.use();
				screenShader.setInt("usualScreenTexture", 0);
				screenShader.setInt("blurScreenTexture", 1);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, usualScreenTexture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

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
		modelHigh = modelHigh + 0.01f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		modelHigh = modelHigh - 0.01f;

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		power = power - 1;
		std::cout << power << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		power = power + 1;
		std::cout << power << std::endl;
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