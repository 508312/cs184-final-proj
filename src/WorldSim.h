#ifndef WORLD_SIMULATOR_H
#define WORLD_SIMULATOR_H

#include <nanogui/nanogui.h>
#include <memory>

#include "camera.h"
#include "cubeFaceEnum.h"
#include "chunk.h"

#include "world.h"

using namespace nanogui;

class WorldSim {
public:
	WorldSim(std::string project_root, Screen* screen);
	~WorldSim();

	void init();

	virtual bool isAlive() { return is_alive; }
	virtual void simulate();
	virtual void drawContents();

	// Screen events

	virtual bool cursorPosCallbackEvent(double x, double y);
	virtual bool mouseButtonCallbackEvent(int button, int action, int modifiers);
	virtual bool keyCallbackEvent(int key, int scancode, int action, int mods);
	virtual bool dropCallbackEvent(int count, const char** filenames);
	virtual bool scrollCallbackEvent(double x, double y);
	virtual bool resizeCallbackEvent(int width, int height);

	void pushCube(MatrixXf& positions, MatrixXf& normals, vec3 pos);
	void pushFace(MatrixXf& positions, MatrixXf& normals, vec3 pos, CUBE_FACE face);

private:
	void pushChunk(Chunk* chunk, MatrixXf& positions, MatrixXf& normals, MatrixXf& positions_water, MatrixXf& normals_water);

	virtual void initGUI(Screen* screen);

	GLShader shader;
	GLShader shaderwater;

	World* world;

	void initShader();

	// File management
	std::string project_root;

	// Camera methods
	virtual Matrix4f getProjectionMatrix();
	virtual Matrix4f getViewMatrix();

	// Default simulation values
	int frames_per_sec = 90;
	int simulation_steps = 30;

	// Camera attributes

	CGL::Camera camera;
	CGL::Camera canonicalCamera;

	double view_distance;
	double canonical_view_distance;
	double min_view_distance;
	double max_view_distance;

	double scroll_rate;

	// Screen methods
	Screen* screen;
	void mouseLeftDragged(double x, double y);
	void mouseRightDragged(double x, double y);
	void mouseMoved(double x, double y);

	// Mouse flags
	bool left_down = false;
	bool right_down = false;
	bool middle_down = false;

	// Keyboard flags

	bool ctrl_down = false;

	// Simulation flags

	bool is_paused = true;

	// Screen attributes

	int mouse_x;
	int mouse_y;

	int screen_w;
	int screen_h;

	bool is_alive = true;

	Vector2i default_window_size = Vector2i(1024, 800);
};


#endif // WORLD_SIMULATOR_H