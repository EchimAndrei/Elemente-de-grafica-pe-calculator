#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform int second_texture;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 color;
    if (second_texture == 1) {
        vec4 color1 = texture(texture_1, texcoord);
        vec4 color2 = texture(texture_2, texcoord);
        color = mix(color1, color2, 0.5f);
    } else {
        color = texture(texture_1, texcoord);
    }
    if (color.a < 0.5f) {
        discard;
    }
    out_color = color;

}
