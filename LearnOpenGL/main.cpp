#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "stb_image.h" // All credit goes to Sean Barrett
#include "Shader.h"

struct ColorVec3 {
	float r;
	float g;
	float b;
	ColorVec3(float r0, float g0, float b0) {
		r = r0;
		g = g0;
		b = b0;
	}
};

void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow * window);
void drawTris(glm::vec2 A, glm::vec2 B, glm::vec2 C, int n);
void drawTri(glm::vec2 A, glm::vec2 B, glm::vec2 C);
glm::vec2 mid(glm::vec2 A, glm::vec2 B);
ColorVec3 getHSVColor(float h, float s, float v);

float mixValue = 0.2f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// SHADERS
	Shader ourShader("shader.vert", "shader.frag");

	// VERTEX DATA
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f,    1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.15f, 0.15f, 0.1f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.1f, 0.1f, 0.1f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3,
	};

	// TEXTURES // 
	unsigned int texture1, texture2;
	// TEXTURE 1 //
	// Initialize Texture
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// Wrapping and Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Load Texture Data
	int width, height, nrChannels;
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// Generate texture if loaded properly
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else 
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// TEXTURE 2 //
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
	{
		std::cout << "Error loading texture" << std::endl;
	}

	// BUFFERS
	//unsigned int VBOs[1], VAOs[1], EBOs[1];
	//glGenVertexArrays(1, VAOs);
	//glGenBuffers(1, VBOs);
	//glGenBuffers(1, EBOs);

	//glBindVertexArray(VAOs[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//// Position Attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// Color Attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//// Texture Attribute
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	//// SECOND TRIANGLE
	//glBindVertexArray(VAOs[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	// UNBIND BEFORE RENDER LOOP
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Tell openGl which texture unit each sample belongs to
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setFloat("blend", mixValue);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// INPUT //
		processInput(window);

		// RENDERING //
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		ourShader.setFloat("blend", mixValue);

		glm::vec2 pA(0.0f,0.5f), pB(0.5f, -0.5f), pC(-0.5f, -0.5f);
		drawTri(pA, pB, pC);
		//drawTris(pA, pB, pC, 2);
		//int vertexColorLocation = glGetUniformLocation(shaderProgram1, "ourColor");

		//float timeValue = glfwGetTime();
		//float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
		//float cValue = cos(timeValue) / 2.0f;
		//float hValue = 360 * cValue;
		//ColorVec3 colorVec = getHSVColor(hValue, 1.0f, 1.0f);
		//float r, g, b;
		//r = colorVec.r; g = colorVec.g; b = colorVec.b;
		//glUniform4f(vertexColorLocation, r, g, b, 1.0f);

		// FIRST TRIANGLE
		/*glBindVertexArray(VAOs[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);*/

		//// SECOND TRIANGLE
		//glBindVertexArray(VAOs[1]);
		//glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glBindVertexArray(VAOs[0]);


		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);


		//glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		//trans = glm::rotate(trans, (float)timeValue, glm::vec3(0.0, 0.0, 1.0));
		//unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
		//trans = glm::scale(trans, glm::vec3((float)cValue, (float)cValue, (float)cValue));
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);



		// CHECK/CALL EVENTS AND BUFFER SWAP //
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Delete Resources
	/*glDeleteVertexArrays(1, VAOs);
	glDeleteBuffers(1, VBOs);*/

	glfwTerminate();
	return 0;
}

void drawTri(glm::vec2 A, glm::vec2 B, glm::vec2 C)
{
	float tri[] = {
		// positions          // colors           // texture coords
		 A.x, A.y, 0.0f,   0.5f, 0.5f, 0.5f,    0.0f, 0.0f,   // top
		 B.x, B.y, 0.0f,   0.15f, 0.15f, 0.1f,  0.0f, 0.0f,   // bottom right
		 C.x, C.y, 0.0f,   0.1f, 0.1f, 0.1f,    0.0f, 0.0f,   // bottom left
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
	// Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Color Data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture Data
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);



	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void drawTris(glm::vec2 A, glm::vec2 B, glm::vec2 C, int n) {
	drawTri(A, B, C);
	if (n > 0) {
		std::cout << n << std::endl;
		drawTris(A, mid(A, B), mid(A, C), n - 1);
		//drawTris(B, mid(A, B), mid(B, C), n - 1);
		//drawTris(C, mid(A, C), mid(B, C), n - 1);
	}

	//// VERTEX DATA
	//float tri[] = {
	//	// positions          // colors           // texture coords
	//	 0.0f, 0.5f, 0.0f,   0.5f, 0.5f, 0.5f,    0.0f, 0.0f,   // top
	//	 0.5f,  -0.5f, 0.0f,   0.15f, 0.15f, 0.1f,  0.0f, 0.0f,   // bottom right
	//	 -0.5f, -0.5f, 0.0f,   0.1f, 0.1f, 0.1f,    0.0f, 0.0f,   // bottom left
	//};
}

glm::vec2 mid(glm::vec2 A, glm::vec2 B)
{
	float x = (A.x + B.x) / 2;
	float y = (A.y + B.y) / 2;
	return glm::vec2(x, y);
}

ColorVec3 getHSVColor(float h, float s, float v) {
	// h [0, 360] s/v [0.0. 1.0];
	int i = (int)floor(h / 60.0f) % 6;
	float f = h / 60.0f - floor(h / 60.0f);
	float p = v * (float)(1 - s);
	float q = v * (float)(1 - s * f);
	float t = v * (float)(1 - (1 - f) * s);
	ColorVec3 color(0.0f, 0.0f, 0.0f);
	switch (i) {
		case 0: color = ColorVec3(v, t, p);
			break;
		case 1: color = ColorVec3(q, v, p);
			break;
		case 2: color = ColorVec3(p, v, t);
			break;
		case 3: color = ColorVec3(p, q, v);
			break;
		case 4: color = ColorVec3(t, p, v);
			break;
		case 5: color = ColorVec3(v, p, q);
			break;
		default: color = ColorVec3(0.f, 0.f, 0.f);
	}
	return color;
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		mixValue += 0.05;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		mixValue -= 0.05;

}
