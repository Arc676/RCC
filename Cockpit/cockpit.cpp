#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void glfwErrorCallback(int error, const char* description) {
	std::cerr << "GFLW error " << error << ": " << description << std::endl;
}

bool glSetup(GLFWwindow*& window, bool lightMode = false) {
	glfwSetErrorCallback(glfwErrorCallback);
	if (glfwInit() == 0) {
		return false;
	}
#ifdef __APPLE__
	const char* glslVersion = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#else
	const char* glslVersion = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 600
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open RC Cockpit",
	                          NULL, NULL);
	if (window == nullptr) {
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize OpenGL loader" << std::endl;
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	if (lightMode) {
		ImGui::StyleColorsLight();
	} else {
		ImGui::StyleColorsDark();
	}
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	return true;
}

void newFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void drawFrame(GLFWwindow*& window) {
	ImGui::Render();

	int dw, dh;
	glfwGetFramebufferSize(window, &dw, &dh);
	glViewport(0, 0, dw, dh);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void glCleanup(GLFWwindow* window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main() {
	GLFWwindow* window;
	if (!glSetup(window)) {
		return 1;
	}

	while (glfwWindowShouldClose(window) == 0) {
		glfwPollEvents();

		newFrame();

		drawFrame(window);
	}

	glCleanup(window);
	return 0;
}