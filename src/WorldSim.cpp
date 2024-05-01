#include <cmath>
#include <glad/glad.h>

#include <CGL/vector3D.h>
#include "CGL/vector3D.h"
#include <iostream>
#include "CGL/matrix3x3.h"
#include <nanogui/nanogui.h>
#include <utility>

#include "GLFW/glfw3.h"
#include "camera.h"
#include "misc/camera_info.h"
#include "misc/file_utils.h"

#include "WorldSim.h"
#include "cubeFaceEnum.h"

// Needed to generate stb_image binaries. Should only define in exactly one source file importing stb_image.h.
#define STB_IMAGE_IMPLEMENTATION
#include "misc/stb_image.h"
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ifstream;
using std::ofstream;
using namespace CGL;

WorldSim::WorldSim(std::string project_root, Screen* screen) {
	this->screen = screen;
    this->project_root = project_root;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
}

WorldSim::~WorldSim() {
    shader.free();
    shaderCursor.free();
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
    camera_info.fClip = 100000;

    CGL::Vector3D target(8, 10, 40);

    CGL::Vector3D c_dir(0., 0., 0.);

    canonical_view_distance = 2.0f * 0.9;
    max_falling_speed = 800.0f;
    scroll_rate = canonical_view_distance / 1.3;
    min_scroll_distance = canonical_view_distance*2;
    max_scroll_distance = canonical_view_distance*20;

    view_distance = canonical_view_distance * 2;
    min_view_distance = canonical_view_distance / 10.0;
    pan_speed = canonical_view_distance * 4.0;
    max_view_distance = canonical_view_distance * 20.0;
    move_constant = 300.0;

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

    world->loadWorld(project_root + "demo");


    std::vector<Chunk*> chunks = world->getChunks();
    pushChunks(chunks);

    initShader();
}

void WorldSim::initShader() {
    shader.initFromFiles("default", project_root + "\\shaders\\default.vert", project_root + "\\shaders\\default.frag", project_root + "\\shaders\\default.geom");
    shaderCursor.initFromFiles("cursor", project_root + "\\shaders\\cursor.vert", project_root + "\\shaders\\cursor.frag");
}

void WorldSim::mousePositionToWorld() {
    auto transformation_matrix = camera.getC2W();
    double mousex, mousey = mouse_x, mouse_y;
    const Vector3D& world_mouse = mousex * transformation_matrix[0] + mousey * transformation_matrix[1];
    vec3 world_mouse_Vec3 = vec3(world_mouse.x, world_mouse.y, world_mouse.z);
    auto chunk = world->getChunkIndex(world_mouse_Vec3);

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

void WorldSim::mouseMoved(double x, double y) {
    //std::cout << x << ' ' << y << '\n';
    //std::cout << mouse_x << ' ' << mouse_y << '\n';

    float dx = x - mouse_x;
    float dy = y - mouse_y;
    float sensitivity = 0.02f;
    if (rotatable)
        camera.rotate_by(-dy * sensitivity, -dx * sensitivity); 

}

void WorldSim::mouseLeftDragged(double x, double y) {
    float dx = x - mouse_x;
    float dy = y - mouse_y;
    camera.rotate_by(-dy * (PI / screen_h), -dx * (PI / screen_w));
    
    //int dx = mouse_x - x;
    //int dy = mouse_y - y;
    //world->spawnCell(vec3(dx, dy, spawn_distance), cell{ WATER_COLOR, WATER });

}

void WorldSim::mouseRightDragged(double x, double y) {
    camera.move_by(mouse_x - x, y - mouse_y, pan_speed);
}

inline vec3 WorldSim::getLookBlockPos() {
    CGL::Vector3D tmp = camera.position() - spawn_distance * camera.for_dir();
    return vec3(tmp.x, tmp.y, tmp.z);
}

inline CGL::Vector3D WorldSim::getLookBlockFloat() {
    return camera.position() - spawn_distance * camera.for_dir();

}

void WorldSim::spawnKeyHeld() {
    std::vector<Chunk*> updated_chunk;
    vec3 temp = getLookBlockPos();
    if (brush_size > 1) {
        for (int dz = temp.z - (brush_size - 1) /2; dz <= temp.z + (brush_size - 1) /2; dz++) {
            for (int dy = temp.y - (brush_size - 1) /2; dy <= temp.y + (brush_size - 1) /2; dy++) {
                for (int dx = temp.x - (brush_size - 1) /2; dx <= temp.x + (brush_size - 1) /2; dx++) {
                    world->spawnCell(vec3(dx,dy,dz), cell{ get_curr_color(), curr_type});
                    if (std::find(updated_chunk.begin(), updated_chunk.end(), world->getChunkAtBlock(vec3(dx, dy, dz))) == updated_chunk.end()) {
                        updated_chunk.push_back(world->getChunkAtBlock(vec3(dx, dy, dz)));
                    }
                }
            }
        }
    }
    else {
        world->spawnCell(temp, cell{ get_curr_color(), curr_type });
        updated_chunk.push_back(world->getChunkAtBlock(getLookBlockPos()));
    }
    pushChunks(updated_chunk);
}

bool WorldSim::keyCallbackEvent(int key, int scancode, int action,
    int mods) {
    ctrl_down = (bool)(mods & GLFW_MOD_CONTROL);

    std::vector<Chunk*> updated_chunk;

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            is_alive = false;
            break;
        case 'r':
        case 'R':
            updateWorld();
            break;
        case 'p':
        case 'P':
            is_paused = !is_paused;
            break;

        case 'W':
        case 'w':
            w_held = true;
            break;
        case 'A':
        case 'a':
            a_held = true;
            break;
        case 'S':
        case 's':
            s_held = true;
            break;
        case 'D':
        case 'd':
            d_held = true;
            break;
        case GLFW_KEY_UP:
            w_held = true;
            break;
        case GLFW_KEY_DOWN:
            s_held = true;
            break;
        case GLFW_KEY_LEFT:
            a_held = true;
            break;
        case GLFW_KEY_RIGHT:
            d_held = true;
        case GLFW_KEY_LEFT_SHIFT:
            shift_held = true;
            break;
        case ' ':
            space_held = true;
            break;

        case 'f':
        case 'F':
            //std::cout << spawn_distance;

            spawnKeyHeld();
            break;
        case 'n':
        case 'N':
            if (is_paused) {
                is_paused = false;
                drawContents();
                is_paused = true;
            }
            break;
        case 'c':
        case 'C':
            draw_cursor = !draw_cursor;
            break;
        case 'l':
        case 'L':
            camera_falling = !camera_falling;
            break;
        }
        
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
            case 'W':
            case 'w':
                w_held = false;
                break;
            case 'A':
            case 'a':
                a_held = false;
                break;
            case 'S':
            case 's':
                s_held = false;
                break;
            case 'D':
            case 'd':
                d_held = false;
                break;
            case GLFW_KEY_UP:
                w_held = false;
                break;
            case GLFW_KEY_DOWN:
                s_held = false;
                break;
            case GLFW_KEY_LEFT:
                a_held = false;
                break;
            case GLFW_KEY_RIGHT:
                d_held = false;
            case GLFW_KEY_LEFT_SHIFT:
                shift_held = false;
            case ' ':
                space_held = false;
        }
    }

    return true;
}

bool WorldSim::dropCallbackEvent(int count, const char** filenames) {
    return true;
}

bool WorldSim::scrollCallbackEvent(double x, double y) {
    //camera.move_forward(y * scroll_rate);   
    spawn_distance = fmax(min_scroll_distance, spawn_distance - ceil(y*scroll_rate));
    spawn_distance = fmin(max_scroll_distance, spawn_distance);
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
    new Label(window, "Reset World", "sans-bold");
    {
        Button* b = new Button(window, "reset-world");
        b->setFlags(Button::NormalButton);
        b->setCallback([this]() {
            init();
            });
    }
    new Label(window, "Material Types", "sans-bold");

    {
        Button* b = new Button(window, "sand");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = SAND;
            get_curr_color = GET_COLOR_FUNC(SAND_COLOR);
            });

        b = new Button(window, "water");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = WATER;
            get_curr_color = GET_COLOR_FUNC(WATER_COLOR);
            });

        b = new Button(window, "fire");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = FIRE;
            get_curr_color = GET_COLOR_FUNC(FIRE_COLOR);
            });

        b = new Button(window, "snow");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = SNOW;
            get_curr_color = GET_COLOR_FUNC(SNOW_COLOR);
            });

        b = new Button(window, "grass");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = GRASS;
            get_curr_color = GET_COLOR_FUNC(GRASS_COLOR);
            });

        b = new Button(window, "wood");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = WOOD;
            get_curr_color = GET_COLOR_FUNC(WOOD_COLOR);
            });

        b = new Button(window, "stone");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = WALL;
            get_curr_color = GET_COLOR_FUNC(WALL_COLOR);
            });

        b = new Button(window, "steam");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = STEAM;
            get_curr_color = GET_COLOR_FUNC(STEAM_COLOR);
            });

        b = new Button(window, "smoke");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = SMOKE;
            get_curr_color = GET_COLOR_FUNC(SMOKE_COLOR);
            });

        b = new Button(window, "delete");
        b->setFlags(Button::RadioButton);
        b->setCallback([this]() {
            curr_type = AIR;
            get_curr_color = GET_COLOR_FUNC(AIR_COLOR);
            });
    }

    new Label(window, "Brush Size", "sans-bold");

    {
        Widget* panel = new Widget(window);
        panel->setLayout(
            new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 5));

        Slider* slider = new Slider(panel);
        slider->setValue((float) brush_size / 10);
        slider->setFixedWidth(125);

        TextBox* size = new TextBox(panel);
        size->setFixedWidth(50);
        size->setValue(std::to_string(brush_size));

        slider->setCallback([size](float value) {
            int intValue = static_cast<int>(value * 9.0f + 1);
            size->setValue(std::to_string(intValue));
            });

        slider->setFinalCallback([&](float value) {
            brush_size = static_cast<int>(value * 9.0f + 1);
            });
    }

    new Label(window, "Filename", "sans-bold");

    {
        TextBox* filename = new TextBox(window);
        filename->setFixedWidth(300);
        filename->setValue("Enter file name here");
        filename->setEditable(true);

        Button* b = new Button(window, "Save");
        b->setFlags(Button::NormalButton);
        b->setCallback([this, filename]() {
            world->dumpWorld(project_root + filename->value());
            });

        b = new Button(window, "Load");
        b->setFlags(Button::NormalButton);
        b->setCallback([this, filename]() {
            world->loadWorld(project_root + filename->value());
            pushChunks(world->getChunks());
            });
    }
}

void WorldSim::pushFacePoint(MatrixXf& positions, MatrixXf& colors, MatrixXf& orientation,
    vec3 pos,
    CUBE_FACE face,
    color& color) {
    positions.conservativeResize(Eigen::NoChange, positions.cols() + Eigen::Index(1));
    colors.conservativeResize(Eigen::NoChange, colors.cols() + Eigen::Index(1));
    orientation.conservativeResize(Eigen::NoChange, orientation.cols() + Eigen::Index(1));
    int idx = positions.cols() - 1;
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
    positions.col(idx) << x, y, z, 1.0;
    orientation.col(idx) << dx, dy, dz, dc;


    float r = color.r / 255.0 * coeff;
    float g = color.g / 255.0 * coeff;
    float b = color.b / 255.0 * coeff;
    float a = color.a / 255.0;
    colors.col(idx) << r, g, b, a;
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

inline void WorldSim::pushChunkCubePoint(MatrixXf& positions, MatrixXf& colors, MatrixXf& orientation, Chunk* chunk, vec3 posInChunk) {
    if (chunk->getCell(posInChunk).color.a == 0) {
        return;
    }
    vec3 pos = chunk->getChunkPos() * CHUNK_SIZE + posInChunk;  // global pos
    cell& self = chunk->getCell(posInChunk);
    cell& topNeighbor = chunk->getCell(posInChunk + vec3(0, 1, 0));
    cell& botNeighbor = chunk->getCell(posInChunk + vec3(0, -1, 0));
    cell& leftNeighbor = chunk->getCell(posInChunk + vec3(-1, 0, 0));
    cell& rightNeighbor = chunk->getCell(posInChunk + vec3(1, 0, 0));
    cell& forwardNeighbor = chunk->getCell(posInChunk + vec3(0, 0, 1));
    cell& backwardNeighbor = chunk->getCell(posInChunk + vec3(0, 0, -1));

    if (leftNeighbor.color.a != 255 && leftNeighbor.type != self.type)
        pushFacePoint(positions, colors, orientation, pos, LEFT, self.color);
    if (botNeighbor.color.a != 255 && botNeighbor.type != self.type)
        pushFacePoint(positions, colors, orientation, pos, BOT, self.color);
    if (forwardNeighbor.color.a != 255 && forwardNeighbor.type != self.type)
        pushFacePoint(positions, colors, orientation, pos, FORWARD, self.color);
    if (topNeighbor.color.a != 255 && topNeighbor.type != self.type)
        pushFacePoint(positions, colors, orientation, pos, TOP, self.color);
    if (rightNeighbor.color.a != 255 && rightNeighbor.type != self.type)
        pushFacePoint(positions, colors, orientation, pos, RIGHT, self.color);
    if (backwardNeighbor.color.a != 255 && backwardNeighbor.type != self.type)
        pushFacePoint(positions, colors, orientation, pos, BACKWARD, self.color);
}


inline void WorldSim::pushChunkCube(MatrixXf& positions, MatrixXf& colors, Chunk* chunk, vec3 posInChunk) {
    if (chunk->getCell(posInChunk).color.a == 0) {
        return;
    }
    vec3 pos = chunk->getChunkPos() * CHUNK_SIZE + posInChunk;  // global pos
    cell& self = chunk->getCell(posInChunk);
    cell& topNeighbor = chunk->getCell(posInChunk + vec3(0, 1, 0));
    cell& botNeighbor = chunk->getCell(posInChunk + vec3(0, -1, 0));
    cell& leftNeighbor = chunk->getCell(posInChunk + vec3(-1, 0, 0));
    cell& rightNeighbor = chunk->getCell(posInChunk + vec3(1, 0, 0));
    cell& forwardNeighbor = chunk->getCell(posInChunk + vec3(0, 0, 1));
    cell& backwardNeighbor = chunk->getCell(posInChunk + vec3(0, 0, -1));

    if (leftNeighbor.color.a != 255 && leftNeighbor.type != self.type)
        pushFace(positions, colors, pos, LEFT, self.color);
    if (botNeighbor.color.a != 255 && botNeighbor.type != self.type)
        pushFace(positions, colors, pos, BOT, self.color);
    if (forwardNeighbor.color.a != 255 && forwardNeighbor.type != self.type)
        pushFace(positions, colors, pos, FORWARD, self.color);
    if (topNeighbor.color.a != 255 && topNeighbor.type != self.type)
        pushFace(positions, colors, pos, TOP, self.color);
    if (rightNeighbor.color.a != 255 && rightNeighbor.type != self.type)
        pushFace(positions, colors, pos, RIGHT, self.color);
    if (backwardNeighbor.color.a != 255 && backwardNeighbor.type != self.type)
        pushFace(positions, colors, pos, BACKWARD, self.color);
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
    color outline_color = { 255, 0, 0, 90 };
    for (int x = from.x; x < to.x; x++) {
        for (int y = from.y; y < to.y; y++) {
            for (int z = from.z; z < to.z; z++) {
                if (x == from.x || x == to.x - 1 || y == from.y || y == to.y - 1 || z == from.z || z == to.z - 1) {
                    pushCube(positions, colors, chunk->getChunkPos() * CHUNK_SIZE + vec3(x, y, z), outline_color);
                }
            }
        }
    }
}

void WorldSim::pushChunk(Chunk* chunk, mesh& mesh) {
    
    //std::cout << "pushing chunk x y z " << chunk->getChunkPos()[0]
    //    << " " << chunk->getChunkPos()[1]
    //    << " " << chunk->getChunkPos()[2] << std::endl;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                //if (chunk->getCell(vec3(x, y, z)).type != AIR) {
                //    pushCube(positions, colors, chunk->getChunkPos() * CHUNK_SIZE + vec3(x, y, z), chunk->getCell(vec3(x, y, z)).color);
                //}
                if (chunk->getCell(vec3(x, y, z)).color.a == 255) {
                    pushChunkCubePoint(mesh.positions, mesh.colors, mesh.orientation, chunk, vec3(x, y, z));
                } else {
                    pushChunkCubePoint(mesh.positions_transparent, mesh.colors_transparent, mesh.orientation_transparent, chunk, vec3(x, y, z));
                }
                
            }
        }
    }
}

inline void WorldSim::pushChunks(std::vector<Chunk*>& chunks) {
    for (Chunk* chunk : chunks) {
        mesh& mesh = chunk_meshes[world->getChunkIndex(chunk->getChunkPos() * CHUNK_SIZE)];
        mesh.positions = MatrixXf(4, 0);
        mesh.orientation = MatrixXf(4, 0);
        mesh.colors = MatrixXf(4, 0);

        mesh.positions_transparent = MatrixXf(4, 0);
        mesh.orientation_transparent = MatrixXf(4, 0);
        mesh.colors_transparent = MatrixXf(4, 0);
        pushChunk(chunk, mesh);
    }
}

void WorldSim::updateWorld() {
    std::vector<Chunk*> updated_chunks = world->update();
    pushChunks(updated_chunks);
}

void WorldSim::simulate() {
    if (w_held) {
        camera.move_by(0, 0, -move_constant, canonical_view_distance);
    }
    if (a_held) {
        camera.move_by(-move_constant, 0, canonical_view_distance);
    }
    if (s_held) {
        camera.move_by(0, 0, move_constant, canonical_view_distance);
    }
    if (d_held) {
        camera.move_by(move_constant, 0, canonical_view_distance);
    }
    if (shift_held) {
        camera.move_by(0, -250, canonical_view_distance);
    }
    if (space_held) {
        camera.move_by(0, 250, canonical_view_distance);
    }
    if (!is_paused) {
        updateWorld();
    }
    if (camera_falling) {
        vec3 camera_position = vec3d2vec3(camera.position());
        auto cur_chunk = world->getChunkAtBlock(camera_position);
        if (cur_chunk != NULL && cur_chunk->getCell(camera_position + vec3(0, -1, 0)).type != AIR) {
            falling_speed = 0.0f;
        }
        camera.move_by(0, -min(falling_speed, max_falling_speed), canonical_view_distance);
        falling_speed += falling_acceleration;
        //check if intersects something
    }
}

inline vec3 WorldSim::vec3d2vec3(Vector3D vec3D) {
    return vec3(vec3D.x, vec3D.y, vec3D.z);
}

inline std::vector<mesh*> WorldSim::getChunkMeshes() {
    std::vector<mesh*> ret;
    int i = 0;
    for (auto entry = chunk_meshes.begin(); entry != chunk_meshes.end(); entry++) {
        ret.push_back(&entry->second);
    }
    return ret;
}

void WorldSim::drawContents() {
    glEnable(GL_DEPTH_TEST);
    
                            
    Matrix4f model;
    model.setIdentity();

    Matrix4f view = getViewMatrix();
    Matrix4f projection = getProjectionMatrix();
    Matrix4f viewProjection = projection * view;

    std::vector<mesh*> meshes = getChunkMeshes();

    shader.bind();
    shader.setUniform("u_model", model);
    shader.setUniform("u_view_projection", viewProjection);
    glDisable(GL_BLEND);

    for (mesh* mesh : meshes) {
        if (mesh->positions.cols() == 0) {
            continue;
        }
        shader.uploadAttrib("in_position", mesh->positions, false);
        shader.uploadAttrib("in_orientations", mesh->orientation, false);
        shader.uploadAttrib("in_colors", mesh->colors, false);
        shader.drawArray(GL_POINTS, 0, mesh->positions.cols());
    }
    
    
    glDepthMask(false);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (mesh* mesh : meshes) {
        if (mesh->positions_transparent.cols() == 0) {
            continue;
        }
        shader.uploadAttrib("in_position", mesh->positions_transparent, false);
        shader.uploadAttrib("in_orientations", mesh->orientation_transparent, false);
        shader.uploadAttrib("in_colors", mesh->colors_transparent, false);
        shader.drawArray(GL_POINTS, 0, mesh->positions_transparent.cols());
    }
    glDepthMask(true);
     
    
    
    if (draw_cursor) {
        MatrixXf lookpos(4, 0);
        MatrixXf lookcol(4, 0);
        color red = { 255, 0, 0 , 255 };
        vec3 temp = getLookBlockPos();
        if (brush_size > 1) {
            int half_size = (brush_size - 1) / 2;
            int startZ = temp.z - half_size;
            int endZ = temp.z + half_size;
            int startY = temp.y - half_size;
            int endY = temp.y + half_size;
            int startX = temp.x - half_size;
            int endX = temp.x + half_size;
            //top and bottom cubes first
            for (int x = startX; x <= endX; x++) {
                for (int y = startY; y <= endY; y++) {
                    pushCube(lookpos, lookcol, vec3(x, y, startZ), red);
                    pushCube(lookpos, lookcol, vec3(x, y, endZ), red);
                }
            }
            //left and right cubes
            for (int z = startZ; z <= endZ; z++) {
                for (int y = startY; y <= endY; y++) {
                    pushCube(lookpos, lookcol, vec3(startX, y, z), red);
                    pushCube(lookpos, lookcol, vec3(endX, y, z), red);
                }
            }
            //forward backward cubes
            for (int z = startZ; z <= endZ; z++) {
                for (int x = startX; x <= endX; x++) {
                    pushCube(lookpos, lookcol, vec3(x, startY, z), red);
                    pushCube(lookpos, lookcol, vec3(x, endY, z), red);
                }
            }
        }
        else {
            pushCube(lookpos, lookcol, temp, red);
        }
        shaderCursor.bind();
        shaderCursor.setUniform("u_model", model);
        shaderCursor.setUniform("u_view_projection", viewProjection);
        shaderCursor.uploadAttrib("in_position", lookpos, false);
        shaderCursor.uploadAttrib("in_colors", lookcol, false);
        shaderCursor.drawArray(GL_LINES, 0, lookpos.cols());
    }

    bool draw_bbox_outline = false;
    if (draw_bbox_outline) {
        MatrixXf positions(4, 0);
        MatrixXf colors(4, 0);
        std::vector<Chunk*> chunks = world->getChunks();

        for (Chunk* chunk : chunks) {
            pushChunkBbox(chunk, positions, colors);
        }
        shaderCursor.bind();
        shaderCursor.setUniform("u_model", model);
        shaderCursor.setUniform("u_view_projection", viewProjection);
        shaderCursor.uploadAttrib("in_position", positions, false);
        shaderCursor.uploadAttrib("in_colors", colors, false);

        shaderCursor.drawArray(GL_LINES, 0, positions.cols());
    }
    
}