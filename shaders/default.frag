#version 330

in vec4 in_position;
in vec4 in_normal;
in vec4 in_tangent;
in vec2 in_uv;

out vec4 out_color;

void main() {
  out_color = vec4(0.7f, 0.4f, 0.1f, 1.0f);
}

