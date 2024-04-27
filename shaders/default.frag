#version 330

in vec4 v_position;
in vec4 v_normal;
in vec4 v_colors;
in vec4 v_tangent;

out vec4 out_color;

void main() {
	float val = v_normal[0] * 1.0 + v_normal[1] * 0.9 + v_normal[2] * 0.8;
	vec4 vmod = vec4(val, val, val, v_normal[3]);
	out_color = v_colors * vmod;
	out_color.a = 1;
}

