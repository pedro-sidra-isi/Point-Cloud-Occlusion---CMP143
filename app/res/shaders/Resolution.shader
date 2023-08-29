#shader vertex
#version 400 core

// Calculate lights etc. on Vertex Shader

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in uint vMaterialIdx;

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

struct Material {
  vec3 ambient; 
  vec3 diffuse;
  vec3 specular;
  float shine;
};
uniform Material materials[5];

// Vertex color
out vec3 vertColor;

void main()
{
    vec4 v_pos4 = view * model * vec4(vPosition,1.0);
    vec3 v_pos = vec3(v_pos4);
    vec3 normal = normalize( transpose(inverse(mat3(view * model))) * vec3(vNormal) );

    vec3 light_pos = vec3(view*vec4(light.position,1.0));
    vec3 v_to_light = normalize(light_pos-v_pos);

    vec3 v_to_eye = normalize(0-v_pos);
    vec3 v_reflection = normalize(-reflect(v_to_light,normal));

    vec3 ambient_color = 0.1*vec3(float(gl_VertexID)/4000, float(gl_VertexID)/4000,float(gl_VertexID)/4000);
    vec3 diffuse_color = 0.5*vec3(float(gl_VertexID)/4000, float(gl_VertexID)/4000,float(gl_VertexID)/4000);
    vec3 specular_color = 0.3*vec3(float(gl_VertexID)/4000, float(gl_VertexID)/4000,float(gl_VertexID)/4000);
    float shine = 60;

    vec3 ambient_light = ambient_color;
    vec3 diffuse_light = max(dot(normal, v_to_light), 0.0) * diffuse_color * light.color * light.strength;
    vec3 specular_light = pow(max(dot(v_reflection, v_to_eye), 0.0), shine) * specular_color * light.color * light.strength;

    vertColor = ambient_light + diffuse_light + specular_light;

    gl_Position =  projection * v_pos4;
}

// Fragment shader doesn't do much since Gourad shading
#shader fragment
#version 400 core

in vec3 vertColor;
out vec4 fColor;

void main()
{
    fColor = vec4(vertColor, 1.0);
}
