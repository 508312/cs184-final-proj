#version 330

// Uniform variables are constant throughout the entire shader
// execution. They are also read-only to enable parallelization.
uniform mat4 u_model;
uniform mat4 u_view_projection;

// In a vertex shader, the "in" variables are read-only per-vertex 
// properties. An example of this was shown in the rasterizer project, 
// where each vertex had an associated "color" or "uv" value which we 
// would later interpolate using barycentric coordinates.
in vec4 in_position;
in vec4 in_normal;
in vec4 in_colors;
in vec4 in_orientations;

// In a vertex shader, the "out" variables are per-vertex properties
// that are read/write. These properties allow us to communicate
// information from the vertex shader to the fragment shader.
// That is, in the linked fragment shader, these values become the 
// "in" variables.
out vec4 v_position;
out vec4 v_color;
out vec4 v_orientation;

// Every shader features a "main" function.
// This is typically where we write to the "out" variables that the
// fragment shaders get to use. It is also where "gl_Position" is set,
// which is the final screen-space location of this vertex which the
// GPU's triangle rasterizer takes in.
void main() {
  v_color = in_colors;
  v_orientation = in_orientations;
  
  gl_Position = in_position;
}
