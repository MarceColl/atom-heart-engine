#include <GLFW/glfw3.h>
#include <stdio.h>
#include <inttypes.h>
#include <string>


struct WindowProperties {
	uint16_t width;
	uint16_t height;
	std::string title;
};


void glfw_error_callback(int error, const char* description) {
	puts(description);
}


GLFWwindow* initialize_GLFW(WindowProperties winprop) {
	if(!glfwInit()) {
		return NULL;
	}

	GLFWwindow *window = NULL; 
	window = glfwCreateWindow(winprop.width,
				  winprop.height,
				  winprop.title.c_str(),
				  NULL, NULL);
	return window;
}


int main(int argc, char** argv) {
	WindowProperties winprop;
	winprop.width = 640;
	winprop.height = 400;
	winprop.title = "My Window";
	
	GLFWwindow *window = initialize_GLFW(winprop);

	glfwMakeContextCurrent(window);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
