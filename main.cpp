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

glm::mat4 modelOfPlane = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
glm::mat4 modelOfCube = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, -2.0f));


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


	// ������� ������� ��������� ������ float � ������ vec3 ������� ������
	std::vector<glm::vec3> cubePositions;
	int counter = 0;
	while (counter < sizeof(cubeVerticesPositions)/sizeof(cubeVerticesPositions[0]))
	{
		cubePositions.push_back(glm::vec3(cubeVerticesPositions[counter], cubeVerticesPositions[counter + 1], cubeVerticesPositions[counter + 2]));
		counter = counter + 3;
	}

	

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

	// ������� ������� �������� ������ float � ������ vec3 �������� ������
	std::vector<glm::vec3> cubeNormals;
	counter = 0;
	while (counter < sizeof(cubeVerticesNormals) / sizeof(cubeVerticesNormals[0]))
	{
		cubeNormals.push_back(glm::vec3(cubeVerticesNormals[counter], cubeVerticesNormals[counter + 1], cubeVerticesNormals[counter + 2]));
		counter = counter + 3;
	}






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



	// ������� ������� ���������� ��������� ������ float � ������ vec2 ���������� ���������
	std::vector<glm::vec2> cubeTextures;
	counter = 0;
	while (counter < (sizeof(cubeVerticesTextureCoords) / sizeof(cubeVerticesTextureCoords[0])))
	{
		cubeTextures.push_back(glm::vec2(cubeVerticesTextureCoords[counter], cubeVerticesTextureCoords[counter + 1]));
		counter = counter + 2;
	}


	// ������� ����������� � ������������� ��� ����
	float cubeTangent[36];
	float cubeBitangent[36];

	// ����� ����������� � ��������� ������� ����������� � �������������
	counter = 0;
	while(counter < (sizeof(cubeTangent) / sizeof(cubeTangent[0])))
	{

		// ����������
		glm::vec3 pos1(cubePositions[counter]);
		glm::vec3 pos2(cubePositions[counter+1]);
		glm::vec3 pos3(cubePositions[counter+2]);

		// ���������� ����������
		glm::vec2 uv1(cubeTextures[counter]);
		glm::vec2 uv2(cubeTextures[counter+1]);
		glm::vec2 uv3(cubeTextures[counter+2]);

		// ������ �������
		glm::vec3 nm(cubeNormals[counter]);

		// �����������
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		cubeTangent[counter] = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		cubeTangent[counter+1] = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		cubeTangent[counter+2] = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		cubeBitangent[counter] = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		cubeBitangent[counter+1] = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		cubeBitangent[counter+2] = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		counter = counter + 3;

	}

	counter = 0;
	int string = 0;
	while (counter < (sizeof(cubeTangent) / sizeof(cubeTangent[0])))
	{
		std::cout << '[' << string << "] " << cubeTangent[counter] << ' ' << cubeTangent[counter + 1] << ' ' << cubeTangent[counter+ 2] << '\n';
		counter = counter + 3;
		string = string + 1;
	}

	
	

	// ��������� VAO � VBO ��� ���� � ������� ��������
	unsigned int cubeVAO, cubeVBO;
	{
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindVertexArray(cubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(cubeVerticesPositions)
			+sizeof(cubeVerticesNormals)
			+sizeof(cubeVerticesTextureCoords)
			+sizeof(cubeTangent) 
			+sizeof(cubeBitangent), 
			NULL, // �� ��������� ����� ������
			GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVerticesPositions), &cubeVerticesPositions);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions), sizeof(cubeVerticesNormals), &cubeVerticesNormals);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals), sizeof(cubeVerticesTextureCoords), &cubeVerticesTextureCoords);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals) + sizeof(cubeVerticesTextureCoords), sizeof(cubeTangent), &cubeTangent);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals) + sizeof(cubeVerticesTextureCoords) + sizeof(cubeTangent), sizeof(cubeBitangent), &cubeBitangent);



		// ������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// ������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVerticesPositions)));
		glEnableVertexAttribArray(1);

		// ������� ���������� ���������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals)));
		glEnableVertexAttribArray(2);

		// ������� �����������
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals)+ sizeof(cubeVerticesTextureCoords)));
		glEnableVertexAttribArray(3);

		// ������� �������������
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVerticesPositions) + sizeof(cubeVerticesNormals) + sizeof(cubeVerticesTextureCoords)+sizeof(cubeTangent)));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
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

	// ��������� �����������/������������� ������� ����� �������������
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;

	// ����������� �1
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	// ����������� �2
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


	float quadVertices[] = {
		// ����������           // �������        // �����. ����������      // �����������          // �������������
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// �������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// ������� ���������� ���������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// ������� �����������
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		// ������� �������������
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
		glEnableVertexAttribArray(4);

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


	Shader ourShader("../midnight/shader.vs", "../midnight/shader.fs", NULL);
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs", NULL);

	unsigned int brickWall = loadTexture("../res/brickwall.png");
	unsigned int BrickWallNormal = loadTexture("../res/brickwall_normal.png");


	glm::vec3 pointLightPosition = glm::vec3(0.0f);

	// ������������ ���� ��� ������ ���������� ������ 
	glfwSwapInterval(1);

	float lastTime = glfwGetTime(); // ���������� �������� ����� ������ ������ ���������
	int nbFrames = 0;




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
		//glEnable(GL_FRAMEBUFFER_SRGB); // �������������� ����� ���������
		glEnable(GL_DEPTH_TEST);



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

				ourShader.setVec3("lightPos", pointLightPosition);
				ourShader.setVec3("viewPos", camera.Position);

			}



			// ��������� �������
			{

				ourShader.setMat4("model", modelOfPlane);

				ourShader.setInt("texture_diffuse1", 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, brickWall);

				ourShader.setInt("texture_normal1", 1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, BrickWallNormal);

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			// ��������� ���� � ���������
			{
				ourShader.setMat4("model", modelOfCube);

				ourShader.setInt("texture_diffuse1", 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, brickWall);

				ourShader.setInt("texture_normal1", 1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, BrickWallNormal);

				glBindVertexArray(cubeVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
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

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		modelOfCube = glm::rotate(modelOfCube, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		modelOfCube = glm::rotate(modelOfCube, glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		modelOfCube = glm::rotate(modelOfCube, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		modelOfCube = glm::rotate(modelOfCube, glm::radians(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
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