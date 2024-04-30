#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
  
in vec4 v_color[];
in vec4 v_orientation[];
uniform mat4 u_model;
uniform mat4 u_view_projection;


out vec4 v_colors;

void main() {
    
    // top right corner
    //positions.col((idx+1) * 3) << x + dx, y + dc, z, 1.0;
    
    // top left corner
    //positions.col((idx+1) * 3 + 1) << x, y + dy, z + dz, 1.0;
    
    // dy + dc can't be 2, dy + dc <= 1
    // top right corner
    //positions.col((idx+1) * 3 + 2) << x + dx, y + dy + dc, z + dz, 1.0;

    float dx = v_orientation[0][0];
    float dy = v_orientation[0][1];
    float dz = v_orientation[0][2];
    float dc = v_orientation[0][3];




    gl_Position = u_view_projection * u_model * gl_in[0].gl_Position; // + vec4(-0.1, 0.0, 0.0, 0.0); 
    v_colors = v_color[0];
    EmitVertex();

    gl_Position = u_view_projection * u_model * (gl_in[0].gl_Position + vec4(dx, dc, 0.0, 0.0));  //dxdc00
    v_colors = v_color[0];
    EmitVertex();

    gl_Position = u_view_projection * u_model * (gl_in[0].gl_Position + vec4( 0.0, dy, dz, 0.0));   //0dydz0
    v_colors = v_color[0];
    EmitVertex();

    gl_Position = u_view_projection * u_model * (gl_in[0].gl_Position + vec4( dx, dy+dc, dz, 0.0)); //dx dy+dc dz 0
    v_colors = v_color[0];
    EmitVertex();
    
    EndPrimitive();
}   