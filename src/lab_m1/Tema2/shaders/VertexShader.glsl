#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float frequency;
uniform float amplitude;

out vec3 color;

float noise(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 position = v_position;

    float height = noise(position.xz * frequency) * amplitude;

    position.y += height;

    gl_Position = Projection * View * Model * vec4(position, 1.0);

    float red = 220.0 / 225.f;
    float green = 121.0 / 225.f;
    float blue = 58 / 225.f;
    vec3 color1 = vec3(red, green, blue);

    red = 41 / 225.f;
    green = 100 / 225.f;
    blue = 57 / 225.f;
    vec3 color2 = vec3(red, green, blue);

    float noise_value = height / amplitude;

    color = mix(color2, color1, noise_value);
}
