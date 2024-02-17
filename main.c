#include <GLFW/glfw3.h>

int main() {
	glfwInit();

	// Create a new window: height, width, window title
	GLFWwindow *win = glfwCreateWindow(800, 600, "rc", NULL, NULL);
	glfwMakeContextCurrent(win);

	// Exit program
	glfwTerminate();
	return 0;
}
