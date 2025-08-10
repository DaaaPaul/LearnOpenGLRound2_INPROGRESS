#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
		} else {
			std::cout << wordName << " linked successfully\n";
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, sizeof(errorMessage), NULL, errorMessage);
			std::cout << "There was an error compiling the " << wordName << ':' << '\n' << errorMessage << '\n';
		} else {
			std::cout << wordName << " compiled successfully\n";
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
	if (window == NULL) {
		std::cout << "Failed to load GLFW";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, glfwFrameBufferCallback);
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::cout << "Failed to load GLAD";
		glfwTerminate();
		return -1;
	}

	unsigned program = createShaderProgram("source/shaders/VertexShader.txt", "source/shaders/FragmentShader.txt");

	float vertexPositions[] = { -0.5, -0.5, 0.0, -0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 0.5, -0.5, 0.0 };
	float vertexColors[] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0 };
	float textureCoordinates[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
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

	stbi_set_flip_vertically_on_load(true);
	unsigned awesomefaceTexture, containerTexture;
	glGenTextures(1, &awesomefaceTexture);
	glBindTexture(GL_TEXTURE_2D, awesomefaceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int awesomefaceWidth, awesomefaceHeight, awesomefaceChannels;
	unsigned char* awesomefaceData = stbi_load("source/textures/awesomeface.png", &awesomefaceWidth, &awesomefaceHeight, &awesomefaceChannels, 0);
	unsigned awesomefaceFormat = (awesomefaceChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, awesomefaceWidth, awesomefaceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, awesomefaceData); // TODO
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(awesomefaceData);
	glBindTexture(GL_TEXTURE_2D, NULL);	

	glGenTextures(1, &containerTexture);
	glBindTexture(GL_TEXTURE_2D, containerTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int containerWidth, containerHeight, containerChannels;
	unsigned char* containerData = stbi_load("source/textures/container.jpg", &containerWidth, &containerHeight, &containerChannels, 0);
	unsigned containerFormat = (containerChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, containerWidth, containerHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, containerData); // TODO
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(containerData);
	glBindTexture(GL_TEXTURE_2D, NULL);

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "awesomeface"), 0);
	glUniform1i(glGetUniformLocation(program, "container"), 1);
	glUseProgram(NULL);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		checkGlfwWindowActions(window);

		glClearColor(0.2, 0.7, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, awesomefaceTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, containerTexture);

		glBindVertexArray(VAO);
		glUseProgram(program);
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