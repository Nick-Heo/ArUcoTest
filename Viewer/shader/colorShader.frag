#version 430 core

in vec3 vColor;

out vec4 color;

void main()
{
	 //gl_FragColor = vec4(vColor, 1.0);
	 color = vec4(vColor, 1.0);
}	