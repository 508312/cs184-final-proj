#include <iostream>
#include <fstream>
#include <nanogui/nanogui.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include "misc/getopt.h" // getopt for windows
#else
#include <getopt.h>
#include <unistd.h>
#endif
#include <unordered_set>
#include <stdlib.h> // atoi for getopt inputs

#include "CGL/CGL.h"
#include "json.hpp"
#include "misc/file_utils.h"

typedef uint32_t gid_t;

using namespace std;
using namespace nanogui;

using json = nlohmann::json;

GLFWwindow *window = nullptr;
Screen *screen = nullptr;

void error_callback(int error, const char* description) {
  puts(description);
}

void createGLContexts() {
  if (!glfwInit()) {
    return;
  }

  glfwSetTime(0);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create a GLFWwindow object
  window = glfwCreateWindow(800, 800, "Cloth Simulator", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Could not initialize GLAD!");
  }
  glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM

  glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Create a nanogui screen and pass the glfw pointer to initialize
  screen = new Screen();
  screen->initialize(window, true);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glfwSwapInterval(1);
  glfwSwapBuffers(window);
}

void setGLFWCallbacks() {
  glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
    if (!screen->cursorPosCallbackEvent(x, y)) {
      
    }
  });

  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *, int button, int action, int modifiers) {
        if (!screen->mouseButtonCallbackEvent(button, action, modifiers) ||
            action == GLFW_RELEASE) {
          
        }
      });

  glfwSetKeyCallback(
      window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
        if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
          
        }
      });

  glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback(window,
                      [](GLFWwindow *, int count, const char **filenames) {
                        screen->dropCallbackEvent(count, filenames);
                        
                      });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    if (!screen->scrollCallbackEvent(x, y)) {
      
    }
  });

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *, int width, int height) {
                                   screen->resizeCallbackEvent(width, height);
                                   
                                 });
}

void usageError(const char *binaryName) {
  printf("Usage: %s [options]\n", binaryName);
  printf("Required program options:\n");
  printf("  -f     <STRING>    Filename of scene\n");
  printf("  -r     <STRING>    Project root.\n");
  printf("                     Should contain \"shaders/Default.vert\".\n");
  printf("                     Automatically searched for by default.\n");
  printf("  -a     <INT>       Sphere vertices latitude direction.\n");
  printf("  -o     <INT>       Sphere vertices longitude direction.\n");
  printf("\n");
  exit(-1);
}

void incompleteObjectError(const char *object, const char *attribute) {
  cout << "Incomplete " << object << " definition, missing " << attribute << endl;
  exit(-1);
}

bool is_valid_project_root(const std::string& search_path) {
    return true;
}

// Attempt to locate the project root automatically
bool find_project_root(const std::vector<std::string>& search_paths, std::string& retval) {
  
  for (std::string search_path : search_paths) {
    if (is_valid_project_root(search_path)) {
      retval = search_path;
      return true;
    }
  }
  return false;
}

int main(int argc, char **argv) {
  // Attempt to find project root
  std::vector<std::string> search_paths = {
    ".",
    "..",
    "../..",
    "../../.."
  };
  std::string project_root;
  bool found_project_root = find_project_root(search_paths, project_root);
  
  int c;
  
  std::string file_to_load_from;
  bool file_specified = false;
  
  while ((c = getopt (argc, argv, "f:r:a:o:")) != -1) {
    switch (c) {
      case 'f': {
        file_to_load_from = optarg;
        file_specified = true;
        break;
      }
      case 'r': {
        project_root = optarg;
        if (!is_valid_project_root(project_root)) {
          std::cout << "Warn: Could not find required file \"shaders/Default.vert\" in specified project root: " << project_root << std::endl;
        }
        found_project_root = true;
        break;
      }
    }
  }
  
  if (!found_project_root) {
    std::cout << "Error: Could not find required file \"shaders/Default.vert\" anywhere!" << std::endl;
    return -1;
  } else {
    std::cout << "Loading files starting from: " << project_root << std::endl;
  }

  if (!file_specified) { // No arguments, default initialization
    std::stringstream def_fname;
    def_fname << project_root;
    def_fname << "/scene/pinned2.json";
    file_to_load_from = def_fname.str();
  }

  glfwSetErrorCallback(error_callback);

  createGLContexts();

  // Initialize the ClothSimulator object

  // Call this after all the widgets have been defined

  screen->setVisible(true);
  screen->performLayout();

  // Attach callbacks to the GLFW window

  setGLFWCallbacks();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw nanogui
    screen->drawContents();
    screen->drawWidgets();

    glfwSwapBuffers(window);

    //if (!app->isAlive()) {
    //  glfwSetWindowShouldClose(window, 1);
    //}
  }

  return 0;
}
