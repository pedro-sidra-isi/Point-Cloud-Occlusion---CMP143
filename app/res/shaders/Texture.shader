#shader vertex
#version 330 core

// Calculate lights etc. on Vertex Shader

layout( location = 0 ) in vec2 vPosition;
layout( location = 1 ) in vec2 vTexCoords;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);
	TexCoord = vec2(vTexCoords.x, vTexCoords.y);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoord);
}
