#shader vertex
#version 400 core

layout( location = 0 ) in vec4 vPosition;

uniform vec3 color;
out vec3 vertColor;

void
main()
{
    gl_Position = vPosition;
    vertColor = color;
}


#shader fragment
#version 400 core

in vec3 vertColor;
out vec4 fColor;

void main()
{
    fColor = vec4(vertColor, 1.0);
}
