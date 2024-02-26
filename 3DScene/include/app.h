/*
* Defines the app class that is responsible for intializing the app, setting u p the scene, update,
* and drawing the scene and setting up camera
*/


#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <mesh.h>
#include <shader.h>
#include "camera.h" 
class App {
public:
	App(std::string WindowTitle, int width, int height); //window title, width, and height
	void Run(); //starts the main loop of the app

	// methods for opening window, setting up the scene, updating the app, rendering the scene, and initializing camera controls
private:
	bool openWindow();
	void setupScene();
	void initializeCameraControls(); // Sets up camera input callbacks
	void handleCameraMovement(float deltaTime); // Processes camera input 
	bool update();
	bool draw();

	// private members store window dimensions, point to glfw window, mesh bojects, shader, and state of application
private:
	std::string appName{};
	int _width{};
	int _height{};
	GLFWwindow* window{ nullptr };

	std::vector<Mesh> meshes;
	Shader shader;
	bool running{ false };
	bool _isOrthographic{ false }; //Projection mode

	Camera _camera; //Camera object

	GLuint woodtilesTexture;
	GLuint silverTexture;
	GLuint quartzTexture;
	GLuint spongeTexture;

	// Lighting variables
	glm::vec3 keyLightDir;
	glm::vec3 keyLightColor;
	float keyLightIntensity; 

	// Transformation matrices for sponge, pyramid, and sphere/cylinder
	glm::mat4 spongeTransform;
	glm::mat4 pyramidTransform;
	glm::mat4 sphereCylinderTransform; 

};