#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
float distance = 0.0f;
float degrees = 0.0f;

void glfwFrameBufferCallback(GLFWwindow* targetWindow, const int newWidth, const int newHeight) {
	glViewport(0, 0, newWidth - 100, newHeight - 100);
}

void checkGlfwWindowActions(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) distance -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) distance += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) degrees += 5.0f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) degrees -= 5.0f;
}

bool checkShaderErrors(const unsigned shader, const char* wordName, const bool isProgram) {
	int success;
	char errorMessage[512];

	if (isProgram) {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, sizeof(errorMessage), NULL, errorMessage);
			std::cout << "There was an error linking the " << wordName << ':' <<'\n' << errorMessage << '\n';
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, sizeof(errorMessage), NULL, errorMessage);
			std::cout << "There was an error compiling the " << wordName << ':' << '\n' << errorMessage << '\n';
		}
	}

	return success;
}

unsigned createShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath) {
	std::ifstream vertexShaderFile(vertexShaderPath);
	std::ifstream fragmentShaderFile(fragmentShaderPath);
	std::stringstream intermediary;
	
	intermediary << vertexShaderFile.rdbuf();
	std::string vertexTempStringHolder = intermediary.str();
	const char* vertexShaderContents = vertexTempStringHolder.c_str();
	unsigned vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderContents, NULL);
	glCompileShader(vertexShader);
	checkShaderErrors(vertexShader, "vertex shader", false);

	intermediary.str("");
	intermediary.clear();
	intermediary << fragmentShaderFile.rdbuf();
	std::string fragmentTempStringHolder = intermediary.str();
	const char* fragmentShaderContents = fragmentTempStringHolder.c_str();
	unsigned fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderContents, NULL);
	glCompileShader(fragmentShader);
	checkShaderErrors(fragmentShader, "fragment shader", false);

	unsigned shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkShaderErrors(shaderProgram, "shader program", true);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGLRound2", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, glfwFrameBufferCallback);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glEnable(GL_DEPTH_TEST);

	unsigned program = createShaderProgram("source/shaders/VertexShader.txt", "source/shaders/FragmentShader.txt");

	float verticies[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	unsigned VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned sionTexture;
	glGenTextures(1, &sionTexture);
	glBindTexture(GL_TEXTURE_2D, sionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int sionWidth, sionHeight, sionChannels;
	unsigned char* sionData = stbi_load("source/textures/sion.jpg", &sionWidth, &sionHeight, &sionChannels, 0);
	unsigned sionFormat = (sionChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, sionFormat, sionWidth, sionHeight, 0, sionFormat, GL_UNSIGNED_BYTE, sionData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(sionData);
	glBindTexture(GL_TEXTURE_2D, NULL);	

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "sion"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sionTexture);
	glUseProgram(NULL);

	glm::mat4 model(1.0);
	model = glm::rotate(model, glm::radians(-75.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 view(1.0);
	view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		checkGlfwWindowActions(window);
		view = glm::translate(view, glm::vec3(0.0, 0.0, distance));
		distance = 0.0f;
		model = glm::rotate(model, glm::radians(degrees), glm::vec3(0.5, 1.0, 0.0));
		degrees = 0.0f;

		glClearColor(0.2, 0.7, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(program);
	glfwTerminate();
	return 0;
}