#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include <GL/glu.h> //anado para la glu perspective
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

float angle = 0.0f; //anado angulo de rotacion

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	// Configurar la matriz de proyección
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1, 100.0);

	// Volver al modo de la matriz del modelo/vista
	glMatrixMode(GL_MODELVIEW);
}

static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
	glColor4ub(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLES);
	glVertex3d(center.x, center.y + size, center.z);
	glVertex3d(center.x - size, center.y - size, center.z);
	glVertex3d(center.x + size, center.y - size, center.z);
	glEnd();
}

static void draw_cube() {

	glEnable(GL_TEXTURE_2D);

	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	GLuint textureID;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBegin(GL_TRIANGLES);

	// face v0-v1-v2
	glVertex3f(1, 1, 1); glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1, 1, 1); glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1, -1, 1); glTexCoord2f(1.0f, 0.0f);
	// face v2-v3-v0
	glVertex3f(-1, -1, 1); glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, -1, 1); glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, 1, 1); glTexCoord2f(0.0f, 0.0f);

	// face v0-v3-v4
	glVertex3f(1, 1, 1); glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, -1, 1); glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, -1, -1); glTexCoord2f(0.0f, 0.0f);
	// face v4-v5-v0
	glVertex3f(1, -1, -1); glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, 1, -1); glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, 1, 1); glTexCoord2f(0.0f, 0.0f);

	// face v0-v5-v6
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, -1);
	glVertex3f(-1, 1, -1);
	
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);

	// face  v1-v6-v7
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, -1, -1);
	
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);

	// face v7-v4-v3
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, -1, -1);

	// face v4-v7-v6
	glVertex3f(1, -1, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, 1, -1);
	
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, -1, -1);

	glEnd();
}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//draw_triangle(u8vec4(255, 0, 0, 255), vec3(0.0, 0.0, 0.0), 0.5);

	glLoadIdentity(); // mirar funcion displayCB() del ejemplo para ver si se puede mejorar
	glTranslatef(0.0f, 0.0f, -5.0f);

	// Rotación del cubo con funcion q dijo el profe
	glRotatef(angle, 1.0f, 1.0f, 1.0f);

	draw_cube();

	angle += 0.5f;


}

int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

	init_openGL();

	while (window.processEvents() && window.isOpen()) {
		const auto t0 = hrclock::now();
		display_func();
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}

	return 0;
}