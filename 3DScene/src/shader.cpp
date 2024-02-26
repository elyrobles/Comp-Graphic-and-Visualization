/*
*
* Defines a shader class to handle compilation, linking, and managing of a shader program
*
*/

#include <shader.h> 
#include <iostream> 
#include <fstream> 
#include <glm/gtc/type_ptr.hpp>

// initializes shader w/ vertex and frag shader source code
Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
	load(vertexSource, fragmentSource);
}
// loads vertexand fragment shader from files
Shader::Shader(const Path& vertexPath, const Path& fragmentPath)
{

	try {
		//load sources from files
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		//load our shader

		load(vShaderStream.str(), fShaderStream.str());
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

}
// This function is designed to update a vec3 uniform variable in the shader program
void Shader::SetVec3(const std::string& name, const glm::vec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc != -1) {
		glUniform3fv(loc, 1, glm::value_ptr(value));
	}
	else {
		std::cerr << "Uniform '" << name << "' not found in shader program." << std::endl;
	}
}
// activates the shader program
void Shader::Bind()
{
	// use our triangle shader
	glUseProgram(shaderProgram);
}

//compiles and links shaders
void Shader::load(const std::string& vertexSource, const std::string& fragmentSource)
{
	const char* vShaderCode = vertexSource.c_str();
	const char* fShaderCode = fragmentSource.c_str();
	auto vertexShader = glCreateShader(GL_VERTEX_SHADER);//creates shaders and compiles them
	glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_fAILED\n" << infoLog << std::endl;
	}

	auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//creates shaders and compiles them
	glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_fAILED\n" << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::LINK_fAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
//Gets the location ofa a uniform variable
GLint Shader::getUniformLocation(const std::string& uniformName)
{
	return glGetUniformLocation(shaderProgram, uniformName.c_str());
}

//This function updates a mat4 uniform variable
void Shader::SetMat4(const std::string& uniformName, const glm::mat4& mat4)
{
	auto uniformLoc = getUniformLocation(uniformName);

	if (uniformLoc != -1) {
		Bind();
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mat4));
	}

}
