#shader vertex
#version 400 core

// Calculate lights etc. on Vertex Shader

layout( location = 0 ) in vec3 vPosition;

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

uniform vec3 color;

// Vertex color
out vec3 vertColor;

void main()
{
    vertColor = color;

    gl_Position =  projection * view * model * vec4(vPosition,1.0);
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
