#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader_class.h"
#include "stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "model.h"
#include "camera.h"

#include <iomanip>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow* window);

// ��������
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int plusMatrix = 32;


// ���������
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// ������
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int glDisplayBoneIndex = 0;

int main()
{
	setlocale(LC_ALL, "Russian");

	long long StartTimeMillis = 0;

	StartTimeMillis = glfwGetTime() * 1000;

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
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetKeyCallback(window, key_callback);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: �������� ���� ���������� �� OpenGL-�������
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	
	Shader shader("../midnight/shader.vs", "../midnight/shader.fs", NULL); // ������
	

	// ����� Model ����� ��������� ��������� ��������� �����:
	// m_VAO
	// m_Buffers[NumOfBonesPerVertex]
	// m_Positions - �������
	// m_Normals - �������
	// m_TexCoord - ���������� ����������
	// m_Bones - ������� � ���� ������ <VertexBoneData>
	// m_Meshes - ������ �������� ������ � ����� <MeshInformation>
	// ��������� MeshInformation � VertexBoneData
	// define NumOfBonesPerVertex ���������� �������� ������ �� �������. � ��� 4

	// ������������������ �������� ������:
	// 1. �������� � ��������� VAO � VBO (��� EBO). 
	// 2. �������� �������� ��� �������� ������ ��� ������ �������:
	//	�) m_Positions - �������
	//  �) m_Normals - �������
	//	�) m_TexCoord - ���������� ����������
	//  �) m_Bones - ������� � ���� ������ <VertexBoneData>
	// 3. ���������� �������� ����������� �� ����
	// 4. �������� �������� ��� �������� ������ � ��������
	// 5. �������� � �������� ������ ��� �������� 
	// 6. ��������� ��������� ���������
	
	// 
	// 
	// 
	//   Model ourModel("C:/Users/tyuri/Documents/GitHub/res/models/man5wt.fbx_Scene.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	
		
	
	
	//	 Model ourModel("C:/Users/tyuri/Documents/GitHub/res/models/snake/snake2.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PopulateArmatureData);
	//	 Model ourModel("C:/Users/tyuri/Download/untitled.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PopulateArmatureData);
		 Model ourModel("C:/Users/tyuri/Documents/GitHub/res/models/1newTry/simple_men (3).fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PopulateArmatureData);
	//	 Model ourModel("C:/Users/tyuri/Documents/GitHub/res/models/1newTry/untitled.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	
	

	// ������������ ���� ��� ������ ���������� ������ 
	glfwSwapInterval(1);

	// ���������������� ��������� ������ ��� ��������� ��������� � ������ �������
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	glEnable(GL_DEPTH_TEST);

	int firstFrame = 1;

	// ���� ����������
	while (!glfwWindowShouldClose(window))
	{
		// ���������� ����� ������ �� �������� ��� ������� �����
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// ��������� �����
		processInput(window);

		shader.use();

		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		
		shader.setMat4("model", model);
		
		shader.setInt("glDisplayBoneIndex", glDisplayBoneIndex);

		glm::vec3 lightPos(0.0f, -1.5f, 0.0f);

		lightPos.z = sin(glfwGetTime()) * 2;
		

		shader.setVec3("lightPos", lightPos);
		
		long long CurrentTimeMillis = glfwGetTime() * 1000;
		float AnimationTimeSec = ((float)(CurrentTimeMillis - StartTimeMillis)) / 1000.0f;

//		std::cout << AnimationTimeSec << "\n";

		std::vector<glm::mat4> Transforms;
		ourModel.GetBoneTransforms(AnimationTimeSec, Transforms);

		

		for (int i = 0; i < Transforms.size(); i++)
		{
			shader.setMat4("gBones["+std::to_string(i)+"]", Transforms[i]);	
		}
	
		if (firstFrame)
		{

			/*std::cout << "\nourModel.m_BoneNameToIndexMap\nNum of bones: " << ourModel.m_BoneNameToIndexMap.size() << "\n";
			std::cout << "\nTransforms.size() = " << Transforms.size() << "\n";

			for (auto& element : ourModel.m_BoneNameToIndexMap)
			{
				std::cout << element.first << " " << element.second << "\n";
			}
			
			std::cout << "\nourModel.m_BoneInfo\nNum of bones: " << ourModel.m_BoneInfo.size() << "\n";
			int iterator = 0;
			for (auto& element : ourModel.m_BoneInfo)
			{
				std::cout << element.name << " " << iterator << "\n";
				iterator++;
			}
*/

		}
		firstFrame = 0;

		ourModel.Render(shader);

		

		
		// glfw: ����� ���������� ��������� � ������� �������. ����� ������� �����\������ (���� �� ������/�������� ������, ��������� ������ ���� � �.�.)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	
	// glfw: ����������, ������������ ���� ����� ��������������� GLFW-��������
	glfwTerminate();


//	std::cout << glGetError() << std::endl; // ������ ��� 0 (no error)

	
	return 0;
}

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
}

// glfw: ������ ���, ����� ���������� ������� ���� (������������� ��� ������������ ��������), ���������� ������ callback-�������
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// ����������, ��� ���� ��������� ������������� ����� �������� ����.
	// �������� ��������, ������ � ������ ����� ����������� ������, ��� �������, �� Retina-��������
	glViewport(0, 0, width, height);
}

// glfw: ������ ���, ����� ������������ ����, ���������� ������ callback-�������
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

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float module(float a)
{
	return a < 0 ? -a : a;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
	{
		plusMatrix = plusMatrix + 1;
		std::cout << plusMatrix << "\n";
	}
		
	if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
	{
		plusMatrix = plusMatrix - 1;
		std::cout << plusMatrix << "\n";
	}

};