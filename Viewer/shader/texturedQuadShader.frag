#version 430 core

uniform sampler2D texture;

in vec2 vUV;

out vec4 color;

void main()
{
	color = texture2D(texture, vUV);
}	