#ifndef WORLD_SIMULATOR_H
#define WORLD_SIMULATOR_H

#include <nanogui/nanogui.h>
#include <memory>

#include "camera.h"
#include "cubeFaceEnum.h"
#include <CGL/vector3D.h>
#include "CGL/vector3D.h"
#include "chunk.h"

#include "world.h"

using namespace nanogui;
using namespace CGL;

// calling this mesh but there is probably a better name
struct mesh {
	MatrixXf positions;
	MatrixXf positions_transparent;
	MatrixXf orientation;
	MatrixXf colors;
	MatrixXf colors_transparent;
	MatrixXf orientation_transparent;
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
	void pushFacePoint(MatrixXf& positions, MatrixXf& colors, MatrixXf& orientation, vec3 pos, CUBE_FACE face, color& color);
	void pushChunkCubePoint(MatrixXf& positions, MatrixXf& colors, MatrixXf& orientation, Chunk* chunk, vec3 pos);
	void pushChunkCube(MatrixXf& positions, MatrixXf& colors, Chunk* chunk, vec3 pos);

	bool rotatable = false;

private:
	void pushChunk(Chunk* chunk, mesh& mesh);
	void pushChunkBbox(Chunk* chunk, MatrixXf& positions, MatrixXf& colors);

	std::vector<mesh*> getChunkMeshes();
	vec3 vec3d2vec3(Vector3D vec3D);
	void updateWorld();
	void pushChunks(std::vector<Chunk*>& chunks);

	virtual void initGUI(Screen* screen);
	
	vec3 getLookBlockPos();
	CGL::Vector3D getLookBlockFloat();
	void spawnKeyHeld();

	std::unordered_map<int, mesh> chunk_meshes;

	GLShader shader;
	GLShader shaderCursor;

	World* world;

	void initShader();

	void mousePositionToWorld();

	// File management
	std::string project_root;

	vec3 Vector3dtovec3(Vector3D vector3d);

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
	double pan_speed;
	double max_scroll_distance;
	double move_constant;

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
	CellType curr_type = SAND;
	color(*get_curr_color)() = GET_COLOR_FUNC(SAND_COLOR);
	int brush_size = 1;

	// Screen attributes

	double mouse_x;
	double mouse_y;

	int screen_w;
	int screen_h;

	bool is_alive = true;

	bool w_held = false;
	bool a_held = false;
	bool s_held = false;
	bool d_held = false;
	bool shift_held = false;
	double falling_speed = 0.0f, falling_acceleration = 50.0f;
	bool camera_falling = false;
	bool space_held = false;
	double max_falling_speed;
	bool draw_cursor = true;

	Vector2i default_window_size = Vector2i(800, 800);
};


#endif // WORLD_SIMULATOR_H