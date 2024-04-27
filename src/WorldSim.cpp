#include <cmath>
#include <glad/glad.h>

#include <CGL/vector3D.h>
#include <nanogui/nanogui.h>

#include "camera.h"
#include "misc/camera_info.h"
#include "misc/file_utils.h"

#include "WorldSim.h"
#include "cubeFaceEnum.h"

// Needed to generate stb_image binaries. Should only define in exactly one source file importing stb_image.h.
#define STB_IMAGE_IMPLEMENTATION
#include "misc/stb_image.h"

WorldSim::WorldSim(std::string project_root, Screen* screen) {
	this->screen = screen;
    this->project_root = project_root;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
}

WorldSim::~WorldSim() {
    shader.free();
    shaderwater.free();
}

void WorldSim::init() {

    // Initialize GUI
    screen->setSize(default_window_size);
    initGUI(screen);

    // Initialize camera

    CGL::Collada::CameraInfo camera_info;
    camera_info.hFov = 50;
    camera_info.vFov = 35;
    camera_info.nClip = 0.01;
    camera_info.fClip = 10000;

    CGL::Vector3D target(8, 10, 40);

    CGL::Vector3D c_dir(0., 0., 0.);

    canonical_view_distance = 2.0f * 0.9;
    scroll_rate = canonical_view_distance / 10;

    view_distance = canonical_view_distance * 2;
    min_view_distance = canonical_view_distance / 10.0;
    max_view_distance = canonical_view_distance * 20.0;

    // canonicalCamera is a copy used for view resets

    camera.place(target, acos(c_dir.y), atan2(c_dir.x, c_dir.z), view_distance,
        min_view_distance, max_view_distance);
    canonicalCamera.place(target, acos(c_dir.y), atan2(c_dir.x, c_dir.z),
        view_distance, min_view_distance, max_view_distance);

    screen_w = default_window_size(0);
    screen_h = default_window_size(1);

    camera.configure(camera_info, screen_w, screen_h);
    canonicalCamera.configure(camera_info, screen_w, screen_h);

    world = new World();

    color black = color{ 0, 0, 0, 255 };
    color sand = color{ 255, 255, 0, 255 };
    color water = color{ 0, 0, 255, 178 };
    // floor 
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            world->spawnCell(vec3(x, 0, z), cell{ SAND_COLOR, SAND });
        }
    }

    world->spawnCell(vec3(3, 3, 3), cell{ SAND_COLOR, SAND });
    world->spawnCell(vec3(6, 6, 6), cell{ SAND_COLOR, SAND });
    world->spawnCell(vec3(1, 3, 1), cell{ sand, SAND });
    world->spawnCell(vec3(1, 6, 1), cell{ sand, SAND });


    for (int y = 7; y < CHUNK_SIZE; y++) {
        world->spawnCell(vec3(1, y, 1), cell{ SAND_COLOR, SAND });
        world->spawnCell(vec3(2, y, 5), cell{ SAND_COLOR, SAND });
        world->spawnCell(vec3(2, y, 6), cell{ SAND_COLOR, SAND });
    }

    for (int y = 7; y < CHUNK_SIZE; y++) {
        world->spawnCell(vec3(6, y, 6), cell{ water, WATER });
        world->spawnCell(vec3(7, y, 7), cell{ water, WATER });
        world->spawnCell(vec3(8, y, 8), cell{ water, WATER });
    }

    initShader();
}

void WorldSim::initShader() {
    shader.initFromFiles("default", project_root + "\\shaders\\default.vert", project_root + "\\shaders\\default.frag");
    shaderwater.initFromFiles("default", project_root + "\\shaders\\water.vert", project_root + "\\shaders\\water.frag");
}


Matrix4f WorldSim::getProjectionMatrix() {
	Matrix4f perspective;
	perspective.setZero();

	double cam_near = camera.near_clip();
	double cam_far = camera.far_clip();

	double theta = camera.v_fov() * PI / 360;
	double range = cam_far - cam_near;
	double invtan = 1. / tanf(theta);

	perspective(0, 0) = invtan / camera.aspect_ratio();
	perspective(1, 1) = invtan;
	perspective(2, 2) = -(cam_near + cam_far) / range;
	perspective(3, 2) = -1;
	perspective(2, 3) = -2 * cam_near * cam_far / range;
	perspective(3, 3) = 0;

	return perspective;
}

nanogui::Matrix4f WorldSim::getViewMatrix() {
	Matrix4f lookAt;
	Matrix3f R;

	lookAt.setZero();

	// Convert CGL vectors to Eigen vectors
	// TODO: Find a better way to do this!

	CGL::Vector3D c_pos = camera.position();
	CGL::Vector3D c_udir = camera.up_dir();
	CGL::Vector3D c_target = camera.view_point();

	Vector3f eye(c_pos.x, c_pos.y, c_pos.z);
	Vector3f up(c_udir.x, c_udir.y, c_udir.z);
	Vector3f target(c_target.x, c_target.y, c_target.z);

	R.col(2) = (eye - target).normalized();
	R.col(0) = up.cross(R.col(2)).normalized();
	R.col(1) = R.col(2).cross(R.col(0));

	lookAt.topLeftCorner<3, 3>() = R.transpose();
	lookAt.topRightCorner<3, 1>() = -R.transpose() * eye;
	lookAt(3, 3) = 1.0f;

	return lookAt;
}

bool WorldSim::cursorPosCallbackEvent(double x, double y) {
    if (left_down && !middle_down && !right_down) {
        if (ctrl_down) {
            mouseRightDragged(x, y);
        }
        else {
            mouseLeftDragged(x, y);
        }
    }
    else if (!left_down && !middle_down && right_down) {
        mouseRightDragged(x, y);
    }
    else if (!left_down && !middle_down && !right_down) {
        mouseMoved(x, y);
    }

    mouse_x = x;
    mouse_y = y;

    return true;
}

bool WorldSim::mouseButtonCallbackEvent(int button, int action,
    int modifiers) {
    switch (action) {
    case GLFW_PRESS:
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            left_down = true;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            middle_down = true;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            right_down = true;
            break;
        }
        return true;

    case GLFW_RELEASE:
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            left_down = false;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            middle_down = false;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            right_down = false;
            break;
        }
        return true;
    }

    return false;
}

void WorldSim::mouseMoved(double x, double y) { y = screen_h - y; }

void WorldSim::mouseLeftDragged(double x, double y) {
    float dx = x - mouse_x;
    float dy = y - mouse_y;
    camera.rotate_by(-dy * (PI / screen_h), -dx * (PI / screen_w));
    
    //int dx = mouse_x - x;
    //int dy = mouse_y - y;
    //world->spawnCell(vec3(dx, dy, spawn_distance), cell{ color{ 0, 0, 0, 0 }, WATER });

}

void WorldSim::mouseRightDragged(double x, double y) {
    camera.move_by(mouse_x - x, y - mouse_y, canonical_view_distance);
}

bool WorldSim::keyCallbackEvent(int key, int scancode, int action,
    int mods) {
    ctrl_down = (bool)(mods & GLFW_MOD_CONTROL);

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            is_alive = false;
            break;
        case 'r':
            world->update();
            break;
        case 'R':
            world->update();
            break;
        case ' ':
            resetCamera();
            break;
        case 'p':
        case 'P':
            is_paused = !is_paused;
            break;

        case 'W':
        case 'w':
            camera.move_by(0, 500, canonical_view_distance);
            break;
        case 'A':
        case 'a':
            camera.move_by(-500, 0, canonical_view_distance);
            break;
        case 'S':
        case 's':
            camera.move_by(0, -500, canonical_view_distance);
            break;
        case 'D':
        case 'd':
            camera.move_by(500, 0, canonical_view_distance);
            break;

        case 'n':
        case 'N':
            if (is_paused) {
                is_paused = false;
                drawContents();
                is_paused = true;
            }
            break;
        }
    }

    return true;
}

bool WorldSim::dropCallbackEvent(int count, const char** filenames) {
    return true;
}

bool WorldSim::scrollCallbackEvent(double x, double y) {
    camera.move_forward(y * scroll_rate);   
    spawn_distance -= ceil(y*scroll_rate);
    return true;
}

bool WorldSim::resizeCallbackEvent(int width, int height) {
    screen_w = width;
    screen_h = height;

    camera.set_screen_size(screen_w, screen_h);
    return true;
}

void WorldSim::resetCamera() { camera.copy_placement(canonicalCamera); }

void WorldSim::initGUI(Screen* screen) {
    Window* window;

    window = new Window(screen, "Simulation");
    window->setPosition(Vector2i(default_window_size(0) - 245, 15));
    window->setLayout(new GroupLayout(15, 6, 14, 5));

    // Mass-spring parameters

    new Label(window, "Parameters", "sans-bold");

    {
        Widget* panel = new Widget(window);
    }
}

void WorldSim::pushFace(MatrixXf& positions, MatrixXf& colors,
                                        vec3 pos ,
                                        CUBE_FACE face,
                                        color& color) {
    positions.conservativeResize(Eigen::NoChange, positions.cols() + Eigen::Index(6));
    colors.conservativeResize(Eigen::NoChange, colors.cols() + Eigen::Index(6));
    int idx = positions.cols() / 3 - 2;
    float dx = 0.0;
    float dy = 0.0;
    float dz = 0.0;
    float dc = 0.0;

    float coeff = 1.0;

    int x = pos[0];
    int y = pos[1];
    int z = pos[2];

    switch (face) {
        case BOT:
            // y is constant
            // dx dz are variables
            dx = 1;
            dy = 0;
            dc = 0;
            dz = -1;
            
            // set correct normal
            //ny = 1;
            coeff = 0.7;
            break;
        case TOP:
            // y is constant
            // dx dz change
            y += 1;
            dx = 1;
            dy = 0;
            dc = 0;
            dz = -1;

            // set correct normal
            //ny = 1.0;
            coeff = 1.0;
            break;
        case LEFT:
            // x is constant
            // dy dz change
            dx = 0;
            dy = 0;
            dc = 1;
            dz = -1;

            // set correct normal
            //nx = 1.0;
            coeff = 0.9;
            break;
        case RIGHT:
            // x is constant
            // dy dz change
            x += 1.0;
            dx = 0;
            dy = 0;
            dc = 1;
            dz = -1;
            
            // set correct normal
            //nx = 1.0;
            coeff = 0.83;
            break;
        case FORWARD:
            // z is constant
            // dx dy change
            dx = 1;
            dy = 1;
            dc = 0;
            dz = 0;

            // set correct normal
            //nz = 1.0;
            coeff = 0.87;
            break;
        case BACKWARD:
            // z is constant
            // dx dy change
            z -= 1.0;
            dx = 1;
            dy = 1;
            dc = 0;
            dz = 0;

            // set correct normal
            //nz = 1.0;
            coeff = 0.8;
            break;
        default:
            std::cout << "SOMETHING WENT WRONG" << std::endl;
    }
    
    // bottom left corner
    positions.col(idx * 3) << x, y, z, 1.0;

    //  top left corner
    positions.col(idx * 3 + 1) << x + dx, y + dc, z, 1.0;

    // bottom right corner
    positions.col(idx * 3 + 2) << x, y + dy, z + dz, 1.0;
    
    // top right corner
    positions.col((idx+1) * 3) << x + dx, y + dc, z, 1.0;
    
    // top left corner
    positions.col((idx+1) * 3 + 1) << x, y + dy, z + dz, 1.0;
    
    // dy + dc can't be 2, dy + dc <= 1
    // top right corner
    positions.col((idx+1) * 3 + 2) << x + dx, y + dy + dc, z + dz, 1.0;


    float r = color.r / 255.0 * coeff;
    float g = color.g / 255.0 * coeff;
    float b = color.b / 255.0 * coeff;
    float a = color.a / 255.0;

    colors.col(idx * 3) << r, g, b, a;
    colors.col(idx * 3 + 1) << r, g, b, a;
    colors.col(idx * 3 + 2) << r, g, b, a;
    colors.col((idx + 1) * 3) << r, g, b, a;
    colors.col((idx + 1) * 3 + 1) << r, g, b, a;
    colors.col((idx + 1) * 3 + 2) << r, g, b, a;
}

inline void WorldSim::pushCube(MatrixXf& positions, MatrixXf& colors, 
                                    vec3 pos, color& color) {
    pushFace(positions, colors, pos, LEFT, color);
    pushFace(positions, colors, pos, BOT, color);
    pushFace(positions, colors, pos, FORWARD, color);
    pushFace(positions, colors, pos, TOP, color);
    pushFace(positions, colors, pos, RIGHT, color);
    pushFace(positions, colors, pos, BACKWARD, color);
}

void WorldSim::pushChunkBbox(Chunk* chunk, MatrixXf& positions, MatrixXf& colors) {
    vec3 from = chunk->getBboxFrom();
    vec3 to = chunk->getBboxTo();
    for (int x = from.x; x < to.x; x++) {
        for (int y = from.y; y < to.y; y++) {
            for (int z = from.z; z < to.z; z++) {
                if (x == from.x || x == to.x - 1 || y == from.y || y == to.y - 1 || z == from.z || z == to.z - 1) {
                    //pushCube(positions, normals, chunk->getChunkPos() * CHUNK_SIZE + vec3(x, y, z));
                }
            }
        }
    }
}

void WorldSim::pushChunk(Chunk* chunk, MatrixXf& positions, MatrixXf& colors) {
    
    //std::cout << "pushing chunk x y z " << chunk->getChunkPos()[0]
    //    << " " << chunk->getChunkPos()[1]
    //    << " " << chunk->getChunkPos()[2] << std::endl;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk->getCell(vec3(x, y, z)).type != AIR) {
                    pushCube(positions, colors, chunk->getChunkPos() * CHUNK_SIZE + vec3(x, y, z), chunk->getCell(vec3(x, y, z)).color);
                }
            }
        }
    }
}

void WorldSim::simulate() {
    if (!is_paused) {
        /*
        world->spawnCell(vec3(3, 10, 3), cell{ color{ 0, 0, 0, 0 }, SAND });
        world->spawnCell(vec3(10, 10, 3), cell{ color{ 0, 0, 0, 0 }, WATER });
        */
        world->update();
    }
}

void WorldSim::drawContents() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
                            
    Matrix4f model;
    model.setIdentity();

    Matrix4f view = getViewMatrix();
    Matrix4f projection = getProjectionMatrix();
    Matrix4f viewProjection = projection * view;

    MatrixXf positions(4, 0);
    MatrixXf colors(4, 0);

    std::vector<Chunk*> chunks = world->getChunks();

    for (Chunk* chunk : chunks) {
        pushChunk(chunk, positions, colors);
    }
    shader.bind();
    shader.setUniform("u_model", model);
    shader.setUniform("u_view_projection", viewProjection);
    shader.uploadAttrib("in_position", positions, false);
    shader.uploadAttrib("in_colors", colors, false);

    shader.drawArray(GL_TRIANGLES, 0, positions.cols());
}