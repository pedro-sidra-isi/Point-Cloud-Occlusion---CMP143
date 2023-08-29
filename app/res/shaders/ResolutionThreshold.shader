#shader vertex
#version 400 core

// Calculate lights etc. on Vertex Shader

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec3 vResProperties;

uniform vec3 max_value;
uniform vec3 points_per_area_threshold;

// typedef struct {
//0  float coordinate[4];
//1  float normal[3];
//2  uint material_index;
//3  uint point_count;
//4  float surface_area;
//5  float vResolution;
// } VertexWithResolution;

// Model-View-Proj
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Lights
struct Light {
    vec3 position;
    vec3 color;
    float strength;
};
uniform Light light;

// Vertex color
varying vec3 v_pos;
varying vec3 normal;
flat out uint materialIdx;
flat out Light light_transformed;

flat out vec3 face_color;

void main()
{
    vec4 v_pos4 = view * model * vPosition;
    v_pos = vec3(v_pos4);

    normal = normalize( transpose(inverse(mat3(view * model))) * vec3(vNormal) );

    gl_Position =  projection * v_pos4;

    light_transformed = light;
    light_transformed.position  = vec3(vec4(light.position,1.0));

    // Viridis yellow and blue
    vec3 c0 = vec3(0.26,  0.0, 0.33);
    vec3 c1 = vec3(0.99,  0.91, 0.15);

    vec3 c;
    if (vResProperties.x > points_per_area_threshold.x)
    {
        c = c0 + 0.5*(c1-c0);
    }
    else{
        c = c0;
    }
    //vec3 c = vec3(vResolution, vResolution, vResolution);
    face_color = max(dot(normal, normalize(light_transformed.position - v_pos)), 0.0) * c * light_transformed.color * light_transformed.strength;
}

// Fragment shader doesn't do much since Gourad shading
#shader fragment
#version 400 core

struct Material {
  vec3 ambient; 
  vec3 diffuse;
  vec3 specular;
  float shine;
};
uniform Material materials[5];

flat in vec3 face_color;

varying vec3 v_pos;
varying vec3 normal;

flat in uint materialIdx;
// Lights
struct Light {
    vec3 position;
    vec3 color;
    float strength;
};
flat in Light light_transformed;


out vec4 fColor;

void main()
{
    // fColor = vec4(vertColor, 1.0);

    vec3 v_to_light = normalize(light_transformed.position - v_pos);

    vec3 v_to_eye = normalize(0-v_pos);
    vec3 v_reflection = normalize(-reflect(v_to_light,normal));

    vec3 ambient_color = materials[materialIdx].ambient;
    vec3 diffuse_color = materials[materialIdx].diffuse;
    vec3 specular_color = materials[materialIdx].specular;
    float shine = materials[materialIdx].shine;

    vec3 ambient_light = ambient_color;
    vec3 diffuse_light = max(dot(normal, v_to_light), 0.0) * diffuse_color * light_transformed.color * light_transformed.strength;
    vec3 specular_light = pow(max(dot(v_reflection, v_to_eye), 0.0), shine) * specular_color * light_transformed.color * light_transformed.strength;

    fColor = vec4(face_color,1.0);

}
