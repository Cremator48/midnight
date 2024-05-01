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

// ���������
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float sizeOfBox = 50.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;


bool firstMouse = true;

float deltaTime = 0.0f;	// ����� ����� ������� � ��������� �������
float lastFrame = 0.0f; // ����� ���������� �����

bool isFlashlightEnable = false;
bool isDirLightEnable = true;
bool isPointLightEnable = false;

int main()
{



	std::cout << "git on windows!\n";

	// glfw: ������������� � ����������������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw: �������� ����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// glad: �������� ���� ���������� �� OpenGL-�������
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}



	// ���������� ������ (� �������) � ��������� ��������� ���������
	float vertices[] = {
		// ����������        // �������           // ���������� ����������
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.75f, 0.33f,   // 7  //������ ������� (���������)
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.33f,   // 4
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.66f,   // 5
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.66f,   // 5
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.75f,  0.66f,  // 6
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.75f,  0.33f,  // 7

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.33f, // 0   // �������� ������� (���������)
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.33f, // 3
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.66f, // 2
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.66f, // 2
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.66f, // 1
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.33f, // 0

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.66f, // 12  // ����� ������� (���������)
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.66f, // 6
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.33f, // 7
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.33f, // 7
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.33f, // 13
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.66f, // 12

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.66f,	// 2		// ������ �������
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.66f, // 5
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.33f,	// 4
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.33f,	// 4
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.33f,	// 3
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.66f, // 2

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.75f, 0.33f,  // 7        // ������ �������
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.33f,  // 4
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,   // 9
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,   // 9
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.75f, 0.0f,   // 8
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.75f, 0.33f,  // 7

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 0.66f,	 // 6		// ������� �������
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.66f,  // 5
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f,   // 10
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f,   // 10
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 1.0f,   // 11
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 0.66f   // 6
	};

	float floorVertices[] = {
	   -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,	// ������� �������
		0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};


	//��������� ��������� ������ ��� ��������� (� VBO ��� ��������)
	unsigned int VBO, VAO;
	{
		//������ ��� ���������
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// �������� �������
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
	
	//��������� ��������� ������ ��� ����
	unsigned int floorVAO;
	{
		unsigned int floorVBO;
		glGenVertexArrays(1, &floorVAO);
		glGenBuffers(1, &floorVBO);

		glBindVertexArray(floorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

		//������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// �������� ��������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// �������� �������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);

		glDeleteBuffers(1, &floorVBO);
	}
	
	// ��������� ��������� ������ ��� ��������
	unsigned int lightVAO;
	{
		//��������� ���� ���������
		glGenVertexArrays(1, &lightVAO);

		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//������������ �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	// ���������������� ��������� ������ ��� ��������� ��������� � ������ �������
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Shader ourShader("../midnight/shader.vs", "../midnight/shader.fs");
	Shader lightCubeShader("../midnight/shader_1.vs", "../midnight/shader_1.fs");
	Shader skyBoxShader("../midnight/skyBoxShader.vs", "../midnight/skyBoxShader.fs");

	Model bankaModel("../res/models/banka/model.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	Model skullModel("../res/models/Skull/12140_Skull_v3_L2.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	Model backpackModel("../res/models/backpack/backpack.obj", aiProcess_Triangulate | /*aiProcess_FlipUVs | */  aiProcess_CalcTangentSpace);

	unsigned int diffuseMap = loadTexture("../res/box.png");
	unsigned int specularMap = loadTexture("../res/specular_map.png");
	unsigned int skyBox = loadTexture("../res/skybox.png");

	glm::vec3 pointLightPosition;

	// ���� ����������
	while (!glfwWindowShouldClose(window))
	{
		//������ ������� ��� ������������ �������� ��������
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//������� ����� �������
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);


		// ��������� �����
		processInput(window);

		// ��������� ����
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//��������� ���������
		{

			// ���-�� ��������
			ourShader.use();
			ourShader.setFloat("material.shininess", 32.0f);

			//[1] SpotLight
			//��������� ����� ������ �����
			ourShader.setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
			ourShader.setVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
			ourShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

			ourShader.setInt("isFlashlightEnable", isFlashlightEnable);


			//��������� ��������� �����
			ourShader.setFloat("spotLight.constant", 1.0f);
			ourShader.setFloat("spotLight.linear", 0.09f);
			ourShader.setFloat("spotLight.quadratic", 0.032f);

			//��������� ����������� ��������� + ������ ����������
			ourShader.setVec3("spotLight.position", camera.Position);
			ourShader.setVec3("spotLight.direction", camera.Front);
			ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));


			//[2] PointLight
			ourShader.setInt("isPointLightEnable", isPointLightEnable);
			ourShader.setVec3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
			ourShader.setVec3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
			ourShader.setVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

			//��������� ��������� �����
			ourShader.setFloat("pointLight.constant", 1.0f);
			ourShader.setFloat("pointLight.linear", 0.09f);
			ourShader.setFloat("pointLight.quadratic", 0.032f);

			//��������� ������������ ��������� �����
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


		//��������� �������� ������: ���� � ��������
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;

		//�������� �������� ������ � ������� ������ �������
		ourShader.use(); //��������� ��������� ��� ��������� � ����� ���� "��������� ���������"
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setVec3("viewPos", camera.Position);




		//��������� ����
		{
			//���������� ������ ���� � ����������
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
			model = glm::scale(model, glm::vec3(13.0f, 13.0f, 13.0f));
			ourShader.setMat4("model", model);

			//�������� diffuse ��������
			ourShader.setInt("material.texture_diffuse1", 0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);

			//�������� specular ��������
			ourShader.setInt("material.texture_specular1", 1);
			glBindTexture(GL_TEXTURE_2D, specularMap);


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);


			//��������� ����
			ourShader.use();
			glBindVertexArray(floorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			
			// ��������� ������� ��������� ���������� �������� ���������� � �� �������������� ���������
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);

		}

		// ��������� ����
		{
			skyBoxShader.use();
			skyBoxShader.setInt("myTexture", 0);

			//���������� ������ ���� � ����������
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(sizeOfBox, sizeOfBox, sizeOfBox));

			skyBoxShader.setMat4("model", model);
			skyBoxShader.setMat4("view", view);
			skyBoxShader.setMat4("projection", projection);

			//�������� diffuse ��������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, skyBox);

			//��������� ����
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			
			// ��������� ������� ��������� ���������� �������� ���������� � �� �������������� ���������
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(0);
		}

		//��������� �������
		{
			//��������� ������ �������
			ourShader.use();
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			ourShader.setMat4("model", model);
			backpackModel.Draw(ourShader);

			//��������� �����
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ourShader.setMat4("model", model);
			bankaModel.Draw(ourShader);

			//��������� ������
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ourShader.setMat4("model", model);
			skullModel.Draw(ourShader);

		}
		

		//���� PointLight ������� - ����������� ����������� ���-�������� �����
		if (isPointLightEnable)
		{
			glBindVertexArray(lightVAO);
			//��������� ����-��������� �����
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPosition);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			lightCubeShader.use();
			lightCubeShader.setMat4("model", model);
			lightCubeShader.setMat4("projection", projection);
			lightCubeShader.setMat4("view", view);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// glfw: ����� ���������� ��������� � ������� �������. ����� ������� �����\������ (���� �� ������/�������� ������, ��������� ������ ���� � �.�.)
		glfwSwapBuffers(window);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// �����������: ����������� ��� �������, ��� ������ ��� ��������� ���� ��������������
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &VBO);



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
