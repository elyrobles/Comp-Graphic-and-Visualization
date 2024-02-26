/*
Defines the mesh class that is responsible for representing the 3D cylinder/sphere
*
*/

#pragma once
#include <vector>
#include <objects.h> // including the object vertices
#include <glad\glad.h>

class Mesh {

	// Initializes the mesh 3D cylinder/sphere with vertices and indices, renders the mesh, and a matrix for translation, rotation, and scale
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	void Draw();

	glm::mat4 Transform{ 1.f };

	// members elementCount count the vertices and indices, buffer and shader objects
private:

	uint32_t elementCount{ 0 };
	GLuint VBO{};
	GLuint shaderProgram{};
	GLuint VAO{};
	GLuint EBO{};
};