#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h>

class Shader{
public:
	Shader(){};
	Shader(std::string v, std::string f){
		init(v, f);
	};
	void init(std::string v, std::string f){
		vertexshader = loadShader(GL_VERTEX_SHADER, v);
		fragmentshader = loadShader(GL_FRAGMENT_SHADER, f);

		shaderId = glCreateProgram();
		
		glAttachShader(shaderId, vertexshader);
		glAttachShader(shaderId, fragmentshader);
		glLinkProgram(shaderId);
	};
	int loadShader(GLenum type, std::string filename){
		GLuint shader;
		GLint compiled;

		shader = glCreateShader(type);
		if (shader == 0){
			std::cout << "Shader 생성 오류" << std::endl;
			return 0;
		}
		std::ifstream shaderStream(filename, std::ios::in);
		if (!shaderStream.is_open()){
			std::cout << filename << " 파일 읽기 오류" << std::endl;
			return 0;
		}
		std::string shaderCode;
		std::string Line = "";
		while (getline(shaderStream, Line)){
			shaderCode += "\n" + Line;
		}
		shaderStream.close();

		const char* code = shaderCode.c_str();

		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled){
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen > 1){
				char* infoLog = new char[infoLen];
				glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
				std::cout << "Shader compile 오류 " << infoLog << std::endl;
				delete infoLog;
			}
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	};
	GLuint getShaderId(){
		return shaderId;
	};
	void bind(){
		glUseProgram(shaderId);
	};
	void unbind(){
		glUseProgram(0);
	};
	~Shader(){
		glDetachShader(shaderId, vertexshader);
		glDetachShader(shaderId, fragmentshader);

		glDeleteShader(vertexshader);
		glDeleteShader(fragmentshader);
		glDeleteProgram(shaderId);
	};
private:
	GLuint shaderId;
	GLuint vertexshader;
	GLuint fragmentshader;
};

