#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec4 fragColor;

void main() {
    vec4 worldPosition = Model * vec4(v_position, 1.0);
    vec4 viewPosition = View * worldPosition;
    gl_Position = Projection * viewPosition;

    fragColor = vec4(0.3, 0.0, 0.0, 0.4); // rosu transparent
}
