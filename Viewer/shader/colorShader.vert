#version 430 core

layout (location = 0) in vec3 vertexModel;
layout (location = 1) in vec3 vertexColor;

out vec3 vColor;

uniform mat4 MVPMatrix;

void main() 
{
	gl_Position = MVPMatrix * vec4(vertexModel, 1.0);
	vColor = vertexColor;
}