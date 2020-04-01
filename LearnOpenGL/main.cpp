#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

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
void drawTris(glm::vec2 A, glm::vec2 B, glm::vec2 C, int n, std::vector<float> &vertices);
void drawTri(glm::vec2 A, glm::vec2 B, glm::vec2 C, std::vector<float> &vertex_arr);
glm::vec2 mid(glm::vec2 A, glm::vec2 B);
ColorVec3 getHSVColor(float h, float s, float v);

const int SCR_WID = 600;
const int SCR_HT = 800;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCR_HT, SCR_WID, "Sierpinski's Triangle", NULL, NULL);
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

	glViewport(0, 0, SCR_HT, SCR_WID);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// SHADERS
	Shader ourShader("shader.vert", "shader.frag");

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Tell openGl which texture unit each sample belongs to
	ourShader.use();

	// render loop
	// -----------
	std::vector<float> vertices;
	glm::vec2 pA(0.0f, 0.5f), pB(0.5f, -0.5f), pC(-0.5f, -0.5f); // Original Points For Triangle
	drawTris(pA, pB, pC, 5, vertices); // Populatue vertices vector with sierpinskis algorith,
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	while (!glfwWindowShouldClose(window)) {
		// INPUT //
		processInput(window);

		// RENDERING //
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		float time = (float)glfwGetTime();
		float h = (sin(time) / 2.0f) + 0.5f;
		h = 360.0f * h;
		ColorVec3 color = getHSVColor(h, 1.0f, 1.0f); // Generate from degree with HSV
		glUniform3f(glGetUniformLocation(ourShader.ID, "colorOver"), color.r, color.g, color.b);
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, time, glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// CHECK/CALL EVENTS AND BUFFER SWAP //
		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void drawTri(glm::vec2 A, glm::vec2 B, glm::vec2 C, std::vector<float> &vertex_arr)
{
	//std::cout << A.x << " " << A.y << std::endl;
	//std::cout << B.x << " " << B.y << std::endl;
	//std::cout << C.x << " " << C.y << std::endl;
	float tri[] = {
		// positions    
		 A.x, A.y, 0.0f, 1.f, 0.5f, 0.0f,
		 B.x, B.y, 0.0f, 1.f, 0.5f, 0.0f,
		 C.x, C.y, 0.0f, 1.f, 0.5f, 0.0f,
	};
	for (int i = 0; i < 18; i++) {
		vertex_arr.push_back(tri[i]);
	}

}

void drawTris(glm::vec2 A, glm::vec2 B, glm::vec2 C, int n, std::vector<float> &vertices) {
	// Sierpinski's Algorithm
	drawTri(mid(A, B) , mid(B, C), mid(A, C), vertices);
	if (n > 0) {
		drawTris(A, mid(A, B), mid(A, C), n - 1, vertices);
		drawTris(B, mid(A, B), mid(B, C), n - 1, vertices);
		drawTris(C, mid(A, C), mid(B, C), n - 1, vertices);
	}
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
}
