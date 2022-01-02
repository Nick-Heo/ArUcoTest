#version 430 core

layout (location = 0) in vec3 vertexModel;
layout (location = 1) in vec2 vertexUV;

uniform mat4 MVPMatrix;

out vec2 vUV;

void main() 
{
	gl_Position = MVPMatrix * vec4(vertexModel, 1.0);
	vUV = vertexUV;
}