/*
* Defines the shader class that is responsible for managing the shader program
*
*/
#pragma once

#include <filesystem>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp> 

using Path = std::filesystem::path;

class Shader {
	// Creates shader, binding, and sets shader uniforms
public:
	Shader() = default;
	Shader(const std::string& vertexSource, const std::string& fragmentSource);
	Shader(const Path& vertexPath, const Path& fragmentPath);

	void SetVec3(const std::string& name, const glm::vec3& value);
	void Bind();

	void SetMat4(const std::string& uniformName, const glm::mat4& mat4);

	//methods handle shaderl loading and get the unfirom location
private:
	void load(const std::string& vertexSource, const std::string& fragmentSource);
	GLint getUniformLocation(const std::string& uniformName);

private:
	GLuint shaderProgram;

};