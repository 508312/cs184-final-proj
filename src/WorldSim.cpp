#include <cmath>
#include <glad/glad.h>

#include <CGL/vector3D.h>
#include <nanogui/nanogui.h>

#include "camera.h"
#include "misc/camera_info.h"
#include "misc/file_utils.h"

#include "WorldSim.h"

// Needed to generate stb_image binaries. Should only define in exactly one source file importing stb_image.h.
#define STB_IMAGE_IMPLEMENTATION
#include "misc/stb_image.h"

WorldSim::WorldSim(std::string project_root, Screen* screen) {
	this->screen = screen;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
}

WorldSim::~WorldSim() {
    shader.free();
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

    canonical_view_distance = 1.0f * 0.9;
    scroll_rate = canonical_view_distance / 10;

    view_distance = canonical_view_distance * 2;
    min_view_distance = canonical_view_distance / 10.0;
    max_view_distance = canonical_view_distance * 20.0;

    // canonicalCamera is a copy used for view resets

    camera.place(0.0f, 0, 0, view_distance,
        min_view_distance, max_view_distance);
    canonicalCamera.place(0.0f, 0, 0,
        view_distance, min_view_distance, max_view_distance);

    screen_w = default_window_size(0);
    screen_h = default_window_size(1);

    camera.configure(camera_info, screen_w, screen_h);
    canonicalCamera.configure(camera_info, screen_w, screen_h);

    initShader();
}

void WorldSim::initShader() {
    shader.initFromFiles("default", "../../../shaders/default.vert", "../../../shaders/default.frag");
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
        case 'R':
            break;
        case ' ':
            //resetCamera();
            break;
        case 'p':
        case 'P':
            is_paused = !is_paused;
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
    return true;
}

bool WorldSim::resizeCallbackEvent(int width, int height) {
    screen_w = width;
    screen_h = height;

    camera.set_screen_size(screen_w, screen_h);
    return true;
}


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

void WorldSim::drawContents() {
    glEnable(GL_DEPTH_TEST);
    shader.bind();
                            
    Matrix4f model;
    model.setIdentity();

    Matrix4f view = getViewMatrix();
    Matrix4f projection = getProjectionMatrix();
    Matrix4f viewProjection = projection * view;

    shader.setUniform("u_model", model);
    shader.setUniform("u_view_projection", viewProjection);

    int num_tris = 2;

    MatrixXf positions(4, num_tris * 3);
    MatrixXf normals(4, num_tris * 3);

    positions.col(0 * 3) << 1.0, 1.0, 0.0, 1.0;
    positions.col(0 * 3 + 1) << 0.0, 1.0, 0.0, 1.0;
    positions.col(0 * 3 + 2) << 1.0, 0.0, 0.0, 1.0;

    normals.col(0 * 3) << 0.0, 0.0, 1.0, 0.0;
    normals.col(0 * 3 + 1) << 0.0, 0.0, 1.0, 0.0;
    normals.col(0 * 3 + 2) << 0.0, 0.0, 1.0, 0.0;

    positions.col(1 * 3) << 1.0, 0.0, 0.0, 1.0;
    positions.col(1 * 3 + 1) << 0.0, 0.0, 0.0, 1.0;
    positions.col(1 * 3 + 2) << 0.0, 1.0, 0.0, 1.0;

    normals.col(1 * 3) << 0.0, 1.0, 0.0, 0.0;
    normals.col(1 * 3 + 1) << 0.0, 1.0, 0.0, 0.0;
    normals.col(1 * 3 + 2) << 0.0, 1.0, 0.0, 0.0;

    shader.uploadAttrib("in_position", positions, false);
    shader.uploadAttrib("in_normal", normals, false);

    shader.drawArray(GL_TRIANGLES, 0, num_tris * 3);
}