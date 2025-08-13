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

void glfwFrameBufferCallback(GLFWwindow* targetWindow, const int newWidth, const int newHeight) {
	glViewport(0, 0, newWidth, newHeight);
}

void checkGlfwWindowActions(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
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

	unsigned program = createShaderProgram("source/shaders/VertexShader.txt", "source/shaders/FragmentShader.txt");

	float vertexPositions[] = { 
		-0.5, -0.5, 0.0, 
		-0.5,  0.5, 0.0, 
		 0.5,  0.5, 0.0, 
		 0.5, -0.5, 0.0 };
	float vertexColors[] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0 };
	float textureCoordinates[] = { 
		0.0, 0.0, 
		0.0, 1.0, 
		1.0, 1.0, 
		1.0, 0.0 };
	unsigned indices[] = { 0, 1, 3, 2, 1, 3 };
	unsigned VAO, EBO, VBOPositions, VBOColors, VBOTextureCoordinates;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &VBOPositions);
	glBindBuffer(GL_ARRAY_BUFFER, VBOPositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	glGenBuffers(1, &VBOColors);
	glBindBuffer(GL_ARRAY_BUFFER, VBOColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

	glGenBuffers(1, &VBOTextureCoordinates);
	glBindBuffer(GL_ARRAY_BUFFER, VBOTextureCoordinates);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	stbi_set_flip_vertically_on_load(true);
	unsigned warwickTexture;
	glGenTextures(1, &warwickTexture);
	glBindTexture(GL_TEXTURE_2D, warwickTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int warwickWidth, warwickHeight, warwickChannels;
	unsigned char* warwickData = stbi_load("source/textures/warwick.jpg", &warwickWidth, &warwickHeight, &warwickChannels, 0);
	unsigned warwickFormat = (warwickChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, warwickFormat, warwickWidth, warwickHeight, 0, warwickFormat, GL_UNSIGNED_BYTE, warwickData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(warwickData);
	glBindTexture(GL_TEXTURE_2D, NULL);	

	glm::mat4 transformation(1.0);
	transformation = glm::translate(transformation, glm::vec3(-0.4, -0.4, 0.0));
	transformation = glm::rotate(transformation, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));

	glm::mat4 transformation2(1.0);
	transformation2 = glm::translate(transformation2, glm::vec3(0.4, 0.4, 0.0));
	transformation2 = glm::scale(transformation2, glm::vec3(1.5, 1.5, 0.0));

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "warwick"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, warwickTexture);
	glUseProgram(NULL);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		checkGlfwWindowActions(window);

		glClearColor(0.2, 0.7, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(transformation));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(transformation2));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &VBOPositions);
	glDeleteBuffers(1, &VBOColors);
	glDeleteBuffers(1, &VBOTextureCoordinates);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(program);
	glfwTerminate();
	return 0;
}