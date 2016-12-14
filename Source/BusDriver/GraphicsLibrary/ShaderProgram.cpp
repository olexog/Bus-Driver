#include "ShaderProgram.h"

namespace GraphicsLibrary
{
	ShaderProgram::ShaderProgram(string vertexShaderFileName, string fragmentShaderFileName)
	{
		ifstream vertexShaderFile = ifstream(vertexShaderFileName);
		string vertexShaderText = string((istreambuf_iterator<char>(vertexShaderFile)), istreambuf_iterator<char>());
		const char* vertexShaderTextPointer = vertexShaderText.c_str();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderTextPointer, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cerr << "Vertex shader creation has failed in file " << vertexShaderFileName << ". Info log:" << infoLog << std::endl;
		}

		ifstream fragmentShaderFile = ifstream(fragmentShaderFileName);
		string fragmentShaderText = string((istreambuf_iterator<char>(fragmentShaderFile)), istreambuf_iterator<char>());
		const char* fragmentShaderTextPointer = fragmentShaderText.c_str();

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderTextPointer, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cerr << "Fragment shader creation has failed in file " << fragmentShaderFileName << ". Info log:" << infoLog << std::endl;
		}

		this->id = glCreateProgram();
		glAttachShader(this->id, vertexShader);
		glAttachShader(this->id, fragmentShader);
		glLinkProgram(this->id);

		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->id, 512, NULL, infoLog);
			std::cerr << "Shader program linking has failed. Info log:" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(this->id);
	}

	void ShaderProgram::Use()
	{
		glUseProgram(this->id);
	}

	void ShaderProgram::Disable()
	{
		glUseProgram(0);
	}

	void ShaderProgram::SetUniform(string name, glm::vec3 vector)
	{
		this->Use();
		GLuint transformLocation = glGetUniformLocation(this->id, name.c_str());
		glUniform3f(transformLocation, vector.x, vector.y, vector.z);
		ShaderProgram::Disable();
	}

	void ShaderProgram::SetUniform(string name, glm::mat4 matrix)
	{
		this->Use();
		GLuint transformLocation = glGetUniformLocation(this->id, name.c_str());
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
		ShaderProgram::Disable();
	}
	void ShaderProgram::SetUniform(string name, int value)
	{
		this->Use();
		GLuint transformLocation = glGetUniformLocation(this->id, name.c_str());
		glUniform1i(transformLocation, value);
		ShaderProgram::Disable();
	}

	void ShaderProgram::SetUniform(string name, float value)
	{
		this->Use();
		GLuint transformLocation = glGetUniformLocation(this->id, name.c_str());
		glUniform1f(transformLocation, value);
		ShaderProgram::Disable();
	}
}
