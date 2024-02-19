#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#define WORLD_X 10
#define WORLD_Y 10

int world[WORLD_X][WORLD_Y] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};

void draw_2d();
void draw_player(float x, float y, float angle);

int main() {
	glfwInit();

	// Create a new window: height, width, window title
	GLFWwindow *win = glfwCreateWindow(800, 600, "rc", NULL, NULL);
	glfwMakeContextCurrent(win);

	// Setup screen
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 800, 600, 0);

	// Player
	float player_x = 5.0f;
	float player_y = 5.0f;
	float view_angle = 0.0f;

	// GLFW loop
	while (!glfwWindowShouldClose(win)) {
		glClear(GL_COLOR_BUFFER_BIT);
		draw_2d();
		draw_player(player_x, player_y, view_angle);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	// Exit program
	glfwDestroyWindow(win);
	glfwTerminate();
	return 0;
}

void draw_2d() {
	for (int x = 0; x < WORLD_X; x++) {
		// Make each edge 32 pixels long 
		int screen_x = x * 32;

		for (int y = 0; y < WORLD_Y; y++) {
			int screen_y = y * 32;

			// Set color to draw with
			if (world[x][y] == 1) {
				// Yellow for walls
				glColor3f(0.7f, 0.7f, 0.0f);
			} else {
				// Green for air
				glColor3f(0.0f, 0.7f, 0.0f);
			}

			// Draw a rectangle
			glBegin(GL_QUADS);
			glVertex2i(screen_x, screen_y);
			glVertex2i(screen_x, screen_y + 32);
			glVertex2i(screen_x + 32, screen_y + 32);
			glVertex2i(screen_x + 32, screen_y);
			glEnd();
		}
	}
}

void draw_player(float x, float y, float angle) {
	// Set line width
	glLineWidth(4);

	// Draw direction ray
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2f(x * 32, y * 32);
	glVertex2f((x + cosf(angle)) * 32, (y + sinf(angle)) * 32);
	glEnd();

	// Set diameter for player circle
	glPointSize(16);

	// Draw player
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(x  * 32, y * 32);
	glEnd();
}
