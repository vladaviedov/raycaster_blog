#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 800.0f
#define SCREEN_HEIGHT 600.0f
#define WORLD_X 10
#define WORLD_Y 10
#define MOVE_SPEED 0.05f
#define TURN_SPEED 0.05f
#define MAX_DIST 100.0f
#define FOV_DEG 80.0f
#define RAYS_PER_DEG 2
#define DEG_TO_RAD (M_PI / 180)

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

typedef enum {
	HRZ,
	VRT
} cast_dir;

typedef struct {
	int success;
	float distance;
	cast_dir direction;
} ray_result;

void draw_2d();
void draw_player(float x, float y, float angle);
void handle_movement(GLFWwindow *win, float *x, float *y, float angle);
void handle_turning(GLFWwindow *win, float *angle);
ray_result raycast(float x, float y, float angle);
ray_result hcast(float x, float y, float angle);
ray_result vcast(float x, float y, float angle);
float distance(float x1, float y1, float x2, float y2);
int near(float x1, float x2);
int is_oob(int x, int y);
void draw_3d(float x, float y, float angle);

int main() {
	glfwInit();

	// Create a new window: width, height, window title
	GLFWwindow *win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rc", NULL, NULL);
	glfwMakeContextCurrent(win);

	// Setup screen
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	// Player
	float player_x = 5.0f;
	float player_y = 5.0f;
	float view_angle = 0.0f;

	// GLFW loop
	while (!glfwWindowShouldClose(win)) {
		glClear(GL_COLOR_BUFFER_BIT);

		draw_3d(player_x, player_y, view_angle);
		handle_turning(win, &view_angle);
		handle_movement(win, &player_x, &player_y, view_angle);

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

void handle_movement(GLFWwindow *win, float *x, float *y, float angle) {
	int w = glfwGetKey(win, GLFW_KEY_W);
	int a = glfwGetKey(win, GLFW_KEY_A);
	int s = glfwGetKey(win, GLFW_KEY_S);
	int d = glfwGetKey(win, GLFW_KEY_D);

	// Forwards, 0 degrees
	if (w && !s) {
		*x += MOVE_SPEED * cosf(angle);
		*y += MOVE_SPEED * sinf(angle);
	}

	// Backwards, 180 degrees
	if (!w && s) {
		*x += MOVE_SPEED * cosf(angle + M_PI);
		*y += MOVE_SPEED * sinf(angle + M_PI);
	}

	// 'y' coordinates are flipped from what you would expect them to be
	// Left, 270 degrees
	if (a && !d) {
		*x += MOVE_SPEED * cosf(angle + 3.0f * M_PI / 2.0f);
		*y += MOVE_SPEED * sinf(angle + 3.0f * M_PI / 2.0f);
	}

	// Right, 90 degrees
	if (!a && d) {
		*x += MOVE_SPEED * cosf(angle + M_PI / 2.0f);
		*y += MOVE_SPEED * sinf(angle + M_PI / 2.0f);
	}
}

void handle_turning(GLFWwindow *win, float *angle) {
	int q = glfwGetKey(win, GLFW_KEY_Q);
	int e = glfwGetKey(win, GLFW_KEY_E);

	// Turn left (counter-clockwise)
	if (q && !e) {
		*angle -= TURN_SPEED;
	}

	// Turn right (clockwise)
	if (!q && e) {
		*angle += TURN_SPEED;
	}
}

ray_result raycast(float x, float y, float angle) {
	ray_result hres = hcast(x, y, angle);
	ray_result vres = vcast(x, y, angle);

	// Success on both - choose shortest
	if (hres.success && vres.success) {
		return hres.distance < vres.distance
			? hres
			: vres;
	}

	// Only vcast succeded
	if (vres.success) {
		return vres;
	}

	// Either hcast succeded or both failed
	return hres;
}

ray_result hcast(float x, float y, float angle) {
	ray_result res = {
		.success = 0,
		.distance = 0,
		.direction = HRZ
	};

	// Ray is horizontal - will never reach a horizontal line
	if (near(angle, 0) || near(angle, M_PI)) {
		return res;
	}

	// Before 180 degrees, we cast downwards
	// After 180 degrees, we cast upwards
	int trace_dir = angle > M_PI ? -1 : 1;

	// Reach first boundary
	float ry = trace_dir == 1 ? ceilf(y) : floorf(y);
	float rx = x + (ry - y) / tanf(angle);
	int celly = trace_dir == 1 ? ry : ry - 1;
	int cellx = rx;

	if (is_oob(cellx, celly)) {
		return res;
	}
	int solid = world[cellx][celly];
	float dist = distance(rx, ry, x, y);

	// Already too far
	if (dist > MAX_DIST) {
		return res;
	}

	// Continue until out of range or solid hit
	while (!solid && dist < MAX_DIST) {
		ry += trace_dir;
		rx += trace_dir / tanf(angle);
		celly = trace_dir == 1 ? ry : ry - 1;
		cellx = rx;

		if (is_oob(cellx, celly)) {
			return res;
		}
		solid = world[cellx][celly];
		dist = distance(rx, ry, x, y);
	}

	// If in range, populate result
	if (solid && dist < MAX_DIST) {
		res.success = 1;
		res.distance = dist;
	}

	return res;
}

ray_result vcast(float x, float y, float angle) {
	ray_result res = {
		.success = 0,
		.distance = 0,
		.direction = VRT
	};

	// Ray is vertical - will never reach a vertical line
	if (near(angle, M_PI / 2) || near(angle, 3 * M_PI / 2)) {
		return res;
	}

	// Between 90 and 270 degrees cast to the left
	int trace_dir = angle > M_PI / 2 && angle < 3 * M_PI / 2 ? -1 : 1;

	// Reach first boundary
	float rx = trace_dir == 1 ? ceilf(x) : floorf(x);
	float ry = y + (rx - x) * tanf(angle);
	int cellx = trace_dir == 1 ? rx : rx - 1;
	int celly = ry;

	if (is_oob(cellx, celly)) {
		return res;
	}
	int solid = world[cellx][celly];
	float dist = distance(rx, ry, x, y);

	// Already too far
	if (dist > MAX_DIST) {
		return res;
	}

	// Continue until out of range or solid hit
	while (!solid && dist < MAX_DIST) {
		rx += trace_dir;
		ry += trace_dir * tanf(angle);
		cellx = trace_dir == 1 ? rx : rx - 1;
		celly = ry;

		if (is_oob(cellx, celly)) {
			return res;
		}
		solid = world[cellx][celly];
		dist = distance(rx, ry, x, y);
	}

	// If in range, populate result
	if (solid && dist < MAX_DIST) {
		res.success = 1;
		res.distance = dist;
	}

	return res;
}

float distance(float x1, float y1, float x2, float y2) {
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

int near(float x1, float x2) {
	return fabs(x1 - x2) < 0.001f;
}

int is_oob(int x, int y) {
	if (x < 0 || y < 0) {
		return 1;
	}

	if (x >= WORLD_X || y >= WORLD_Y) {
		return 1;
	}

	return 0;
}

void draw_3d(float x, float y, float angle) {
	// Calculate line count and width
	int rays = FOV_DEG * RAYS_PER_DEG;
	float line_width = 800.0f / rays;

	// Get starting angle
	float start_angle = angle - (FOV_DEG * DEG_TO_RAD / 2.0f);
	
	// Change angle by this much each time
	float angle_delta = DEG_TO_RAD / RAYS_PER_DEG;

	// Cast rays and draw to screen
	for (int i = 0; i < rays; i++) {
		float current_angle = start_angle + i * angle_delta;
		ray_result result = raycast(x, y, current_angle);
		if (!result.success) {
			continue;
		}

		// Set color
		glColor3f(0.5f, 0.0f, 0.0f);

		// The height is inversly proportional to distance
		float height = 600.0f / result.distance;

		// Draw rectangle
		float start = line_width * i;
		glBegin(GL_QUADS);
		glVertex2f(start, (600.0f - height) / 2.0f);
		glVertex2f(start + line_width, (600.0f - height) / 2.0f);
		glVertex2f(start + line_width, (600.0f + height) / 2.0f);
		glVertex2f(start, (600.0f + height) / 2.0f);
		glEnd();
	}
}
