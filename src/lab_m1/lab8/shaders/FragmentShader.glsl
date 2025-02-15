#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 light_direction2;
uniform vec3 light_position2;

uniform float material_kd2;
uniform float material_ks2;
uniform int material_shininess2;

// TODO(student): Declare any other uniforms
uniform int typeLight;
uniform float cut_off_angle;
uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 3) out vec4 out_color2;


void main()
{
    // TODO(student): Define ambient, diffuse and specular light components
    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);
    vec3 L2 = normalize(light_position2 - world_position);
    vec3 H2 = normalize(L2 + V);
    
    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(normalize(N), L), 0);
    float specular_light = 0;

    float ambient_light2 = 0.25;
    float diffuse_light2 = material_kd2 * max(dot(normalize(N), L2), 0);
    float specular_light2 = 0;


    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(normalize(N), H), 0), material_shininess);
    }

    if(diffuse_light2 > 0){
        specular_light2 = material_ks2 * pow(max(dot(normalize(N), H2), 0), material_shininess2);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.

    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
    
    float total_light = 0;
    float total_light2 = 0;

    if (typeLight == 1) {
        float cut_off_rad = radians(cut_off_angle);
		float spot_light = dot(-L, light_direction);
        float spot_light2 = dot(-L2, light_direction2);
		float spot_light_limit	= cos(cut_off_rad);
        float spot_light_limit2	= cos(cut_off_rad);
		
		if (spot_light > spot_light_limit) {
			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor	= pow(linear_att, 2);
			total_light	= ambient_light + light_att_factor * (diffuse_light + specular_light);
		} else 
            total_light = ambient_light;
        
        if (spot_light2 > spot_light_limit2){
            float linear_att2 = (spot_light2 - spot_light_limit2) / (1 - spot_light_limit2);
			float light_att_factor2	= pow(linear_att2, 2);
            total_light2 = ambient_light2 + light_att_factor2 * (diffuse_light2 + specular_light2);
        }
        else {
            total_light2 = ambient_light2;
        }
    } else {
        total_light = ambient_light + diffuse_light + specular_light;
        total_light2 = ambient_light2 + diffuse_light2 + specular_light2;
    }


    // TODO(student): Write pixel out color
    vec3 color = object_color * total_light;
    vec3 color2 = object_color * total_light2;

    out_color = vec4(color2 + color, 1);

}
