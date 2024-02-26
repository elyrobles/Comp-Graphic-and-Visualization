/*
*
*
*
* Final Project by Elizabeth Robles
* February 24th, 2024
*
*
*/


#include "app.h"
#include "camera.h"
#include <iostream>
#include <objects.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h> 


Camera camera(glm::vec3(0.0f, 1.f, 3.0f)); // Camera object

// Mouse position tracking
float lastX = 400, lastY = 300;
bool firstMouse = true;

// light source setup
App::App(std::string WindowTitle, int width, int height)
	: appName{ WindowTitle }, _width{ width }, _height{ height }, _camera(glm::vec3(0.0f, 0.0f, 3.0f)),
	keyLightDir(glm::vec3(0.0f, 0.3f, 0.3f)), // sunlight coming from an angle
	keyLightColor(glm::vec3(1.0f, 0.9f, 0.8f)), // color of sunlight
	keyLightIntensity(1.5f) // brightness of the light source
{
	keyLightColor *= keyLightIntensity; 
}


void App::Run() {
	if (!openWindow()) {
		return;
	}

	initializeCameraControls();
	running = true;
	setupScene();

	float lastFrame = 0.0f;

	while (running) {
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (glfwWindowShouldClose(window)) {
			running = false;
			break;
		}

		handleCameraMovement(deltaTime);
		update();
		draw();
	}

	glfwTerminate();
}

void App::initializeCameraControls() {
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; 

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		// this will adjust the camera movement speed based on yoffset
		camera.MovementSpeed += static_cast<float>(yoffset);
		if (camera.MovementSpeed < 1.0f)
			camera.MovementSpeed = 1.0f;
		if (camera.MovementSpeed > 10.0f)
			camera.MovementSpeed = 10.0f;
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		// this will adjust the camera movement speed based on yoffset
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
		});

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
// camera movement with WSAD, QE, and P keys
void App::handleCameraMovement(float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);


	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.Position += glm::vec3(0.0f, deltaTime * camera.MovementSpeed, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.Position -= glm::vec3(0.0f, deltaTime * camera.MovementSpeed, 0.0f);

	// Projection mode with P key
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		_isOrthographic = !_isOrthographic;
	}
	// Exit the scene when Escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

bool App::openWindow()
{
	//GLFW and window setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "3D Scene by Elizabeth Robles", nullptr, nullptr);

	if (!window) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);


	glfwSetWindowUserPointer(window, (void*)this);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);

		auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
		app->_width = width;
		app->_height = height;
		});

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	glEnable(GL_DEPTH_TEST);

	return true;
}

// Sets up the scene and includes meshes, shaders, transformations, and textures
void App::setupScene()
{
	// mesh for the sphere and cylinder object
	meshes.emplace_back(Shapes::bothVertices, Shapes::bothIndices);

	// mesh for the plane
	meshes.emplace_back(ShapesTwo::planeVertices, ShapesTwo::planeIndices);

	//mesh for the pyramid
	meshes.emplace_back(ShapesThree::pyramidVertices, ShapesThree::pyramidElements); 

	//mesh for the cube
	meshes.emplace_back(ShapesFour::cubeVertices, ShapesFour::cubeElements); 

	//loads shaders from .frag and .vert files
	Path shaderPath = std::filesystem::current_path() / "assets" / "shaders";
	shader = Shader(shaderPath / "shader.vert", shaderPath / "shader.frag");

	/*
	* 
	* Define transformations for each object
	* 
	*/ 

	// Defining the scale factor for the sponge
	float spongeScaleFactor = 0.5f; // scale factor
	glm::mat4 spongeScale = glm::scale(glm::mat4(1.0f), glm::vec3(spongeScaleFactor, spongeScaleFactor, spongeScaleFactor));

	// Defining the rotation for the sponge
	float rotationAngleDegrees = 125.0f; //rotation angle
	float rotationAngleRadians = glm::radians(rotationAngleDegrees);
	glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the y-axis
	glm::mat4 spongeRotation = glm::rotate(glm::mat4(1.0f), rotationAngleRadians, rotationAxis);

	// Define the translation for the sponge
	float spongeBottomOffset = 0.25f * spongeScaleFactor; // This is half the height of the sponge after scaling
	glm::vec3 spongePosition(-1.0f, -0.5f + spongeBottomOffset, 0.5f); // Raise the bottom to sit on the plane
	glm::mat4 spongeTranslation = glm::translate(glm::mat4(1.0f), spongePosition);

	// Combining the transformations
	spongeTransform = glm::translate(glm::mat4(1.0f), spongePosition) * spongeRotation * spongeScale;


	// Scaling down pyramid
	float pyramidScaleFactor = 0.6f; 
	float pyramidHeight = 1.0f; // height of the pyramid before scaling
	float pyramidBottomOffset = (pyramidHeight * pyramidScaleFactor) / 2.0f; // Half the height after scaling
	glm::mat4 pyramidScale = glm::scale(glm::mat4(1.0f), glm::vec3(pyramidScaleFactor, pyramidScaleFactor, pyramidScaleFactor));
	pyramidTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f + pyramidBottomOffset, 0.5f)) * pyramidScale;

	// not scaling this object
	sphereCylinderTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));

	// Load the wood tiles texture
	{
		Path texturePath = std::filesystem::current_path() / "assets" / "textures";
		auto woodtilesPath = (texturePath / "woodtiles.jpg").string();

		int woodtilesWidth, woodtilesHeight, woodtilesChannels;
		unsigned char* woodtilesData = stbi_load(woodtilesPath.c_str(), &woodtilesWidth, &woodtilesHeight, &woodtilesChannels, STBI_rgb_alpha);

		glGenTextures(1, &woodtilesTexture);
		glBindTexture(GL_TEXTURE_2D, woodtilesTexture);

		if (woodtilesData) {
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, woodtilesWidth, woodtilesHeight);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, woodtilesWidth, woodtilesHeight, GL_RGBA, GL_UNSIGNED_BYTE, woodtilesData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cerr << "Failed to load texture at path: " << woodtilesPath << std::endl;
		}
		stbi_image_free(woodtilesData);
	}

	// Load the silver texture for the cap/pink body
	{
		Path texturePath = std::filesystem::current_path() / "assets" / "textures";
		auto silverPath = (texturePath / "silver.jpg").string();

		int silverWidth, silverHeight, silverChannels;
		unsigned char* silverData = stbi_load(silverPath.c_str(), &silverWidth, &silverHeight, &silverChannels, STBI_rgb_alpha);

		glGenTextures(1, &silverTexture);
		glBindTexture(GL_TEXTURE_2D, silverTexture);

		if (silverData) {
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, silverWidth, silverHeight);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, silverWidth, silverHeight, GL_RGBA, GL_UNSIGNED_BYTE, silverData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cerr << "Failed to load texture at path: " << silverPath << std::endl;
		}
		stbi_image_free(silverData);
	}

	{
		// Load the quartz texture
		Path texturePath = std::filesystem::current_path() / "assets" / "textures";
		auto quartzPath = (texturePath / "quartz.jpg").string();

		int width, height, numChannels;
		unsigned char* quartzData = stbi_load(quartzPath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

		glGenTextures(1, &quartzTexture);
		glBindTexture(GL_TEXTURE_2D, quartzTexture);

		if (quartzData) {
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, quartzData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cerr << "Failed to load texture at path: " << quartzPath << std::endl;
		}
		stbi_image_free(quartzData);

	}

	{
		// Load the sponge texture
		Path texturePath = std::filesystem::current_path() / "assets" / "textures";
		auto spongePath = (texturePath / "sponge.png").string();

		int width, height, numChannels;
		unsigned char* spongeData = stbi_load(spongePath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

		glGenTextures(1, &spongeTexture);
		glBindTexture(GL_TEXTURE_2D, spongeTexture);

		if (spongeData) {
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, spongeData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cerr << "Failed to load texture at path: " << spongePath << std::endl;
		}
		stbi_image_free(spongeData);
	}

}

bool App::update()
{
	return false;
}

// draw() renders the scene
bool App::draw()
{
	glClearColor(0.71f, 0.71f, 0.61f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 projection;
	if (_isOrthographic) {
		// Orthographic projection
		float aspectRatio = static_cast<float>(_width) / static_cast<float>(_height);
		projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
	}
	else {
		// Perspective projection
		projection = glm::perspective(glm::radians(75.f), static_cast<float>(_width) / static_cast<float>(_height), 0.1f, 100.f);
	}

	glm::mat4 view = camera.GetViewMatrix();
	shader.Bind();

	// Set light properties
	shader.SetVec3("keyLightDir", keyLightDir);
	shader.SetVec3("keyLightColor", keyLightColor);
	shader.SetVec3("viewPos", camera.Position); // pass the camera position for specular lighting
	shader.SetMat4("projection", projection);
	shader.SetMat4("view", view);

	if (!meshes.empty()) {
		// Sphere and cylinder
		shader.SetMat4("model", sphereCylinderTransform);
		glBindTexture(GL_TEXTURE_2D, silverTexture); // bind texture to each object
		meshes[0].Draw();

		// Plane
		glBindTexture(GL_TEXTURE_2D, woodtilesTexture);
		shader.SetMat4("model", glm::mat4(1.0f)); 
		meshes[1].Draw();

		// Pyramid
		shader.SetMat4("model", pyramidTransform);
		glBindTexture(GL_TEXTURE_2D, quartzTexture);
		meshes[2].Draw();

		// Sponge
		shader.SetMat4("model", spongeTransform);
		glBindTexture(GL_TEXTURE_2D, spongeTexture);
		meshes[3].Draw();
	}


	glfwSwapBuffers(window);
	glfwPollEvents();

	return false;
}
