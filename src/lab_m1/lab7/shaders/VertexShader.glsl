#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;
uniform vec3 light_color;

// Output value to fragment shader
out vec3 color;


void main()
{
    float ke = 1;
    float kl = 1;
    float kq = 1;

    // TODO(student): Compute world space vectors
    vec3 world_pos = (Model * vec4(v_position,1)).xyz;
    vec3 world_normal = normalize( mat3(Model) * v_normal);
    vec3 L = normalize( light_position - world_pos );
    vec3 V = normalize( eye_position - world_pos );


    // TODO(student): Define ambient light component
    // float ambient_light = 0.25;
    vec3 ambient_color = 0.25 * light_color;

    // TODO(student): Compute diffuse light component
    // float diffuse_light = material_kd * max(dot(world_normal, L), 0);
    vec3 diffuse_color = material_kd * light_color * max(dot(world_normal, L), 0);

    // TODO(student): Compute specular light component
    // float specular_light = 0;
    vec3 specular_light_color = vec3(0);

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Gouraud (1971) shading method. There is also the Phong (1975) shading
    // method, which we'll use in the future. Don't mix them up!
    if (diffuse_color.x > 0 && diffuse_color.y > 0 && diffuse_color.z > 0)
    {
        // specular_light = material_ks * pow(max(dot(world_normal, L), 0), material_shininess);
        specular_light_color = material_ks * light_color * pow(max(dot(world_normal, L), 0), material_shininess);

    }

    float dist = distance(world_pos, light_position);
    float attenuation = 1.0 / (ke + kl * dist + kq * dist * dist);

    // TODO(student): Compute light
    // float light = ambient_light + attenuation * (diffuse_light + specular_light);
    vec3 light_vec = ambient_color + attenuation * (diffuse_color + specular_light_color);

    // TODO(student): Send color light output to fragment shader
    // color = light * object_color;
    color = light_vec * object_color;

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
