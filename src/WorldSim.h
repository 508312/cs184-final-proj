#ifndef WORLD_SIMULATOR_H
#define WORLD_SIMULATOR_H

#include <nanogui/nanogui.h>
#include <memory>

#include "camera.h"
#include "cubeFaceEnum.h"
#include "chunk.h"

#include "world.h"

using namespace nanogui;

// calling this mesh but there is probably a better name
struct mesh {
	MatrixXf positions;
	MatrixXf colors;
};

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
  	virtual void resetCamera();

	void pushCube(MatrixXf& positions, MatrixXf& colors, vec3 pos, color& color);
	void pushFace(MatrixXf& positions, MatrixXf& colors, vec3 pos, CUBE_FACE face, color& color);

private:
	void pushChunk(Chunk* chunk, MatrixXf& positions, MatrixXf& colors);
	void pushChunkBbox(Chunk* chunk, MatrixXf& positions, MatrixXf& colors);

	std::vector<mesh*> getChunkMeshes();
	void updateWorld();
	void pushChunks(std::vector<Chunk*>& chunks);

	virtual void initGUI(Screen* screen);
	
	vec3 getLookBlockPos();
	CGL::Vector3D getLookBlockFloat();

	std::unordered_map<int, mesh> chunk_meshes;

	GLShader shader;
	//GLShader shaderwater;

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
	double min_scroll_distance;
	double max_scroll_distance;

	// Screen methods
	Screen* screen;
	void mouseLeftDragged(double x, double y);
	void mouseRightDragged(double x, double y);
	void mouseMoved(double x, double y);

	// Mouse flags
	bool left_down = false;
	bool right_down = false;
	bool middle_down = false;
	int spawn_distance = 5;

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

	bool w_held = false;
	bool a_held = false;
	bool s_held = false;
	bool d_held = false;

	Vector2i default_window_size = Vector2i(800, 800);
};


#endif // WORLD_SIMULATOR_H