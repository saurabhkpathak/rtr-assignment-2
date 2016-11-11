// Std. Includes
#include <string>
#include <algorithm>
using namespace std;

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include <lgl/shader.h>
#include <lgl/camera.h>
#include <lgl/model.h>
#include <lgl/mesh.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

//namespace cubemap {
// Properties
	GLuint screenWidth = 1800, screenHeight = 1200;

	// Function prototypes
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void Do_Movement();
	GLuint loadTexture(GLchar const * path);
	GLuint loadCubemap(std::vector<std::string> faces);
	void RenderQuad();

	// Camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	bool keys[1024];
	GLfloat lastX = 400, lastY = 300;
	bool firstMouse = true;

	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	void setShader(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
		shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(shader.Program, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	}

	// The MAIN function, from here we start our application and run our Game loop
	int main()
	{
		// Init GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Transmittance", nullptr, nullptr); // Windowed
		glfwMakeContextCurrent(window);

		// Set the required callback functions
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// Options
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Initialize GLEW to setup the OpenGL Function pointers
		glewExperimental = GL_TRUE;
		glewInit();

		// Define the viewport dimensions
		glViewport(0, 0, screenWidth, screenHeight);

		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Setup and compile our shaders
		Shader shader("shaders/cubemaps_refract.vs", "shaders/cubemaps_refract.frag");
		Shader reflectionShader("shaders/cubemaps.vs", "shaders/cubemaps.frag");
		Shader fresnelShader("shaders/cubemaps_refract.vs", "shaders/text.frag");
		Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.frag");
		Shader botShader("bot.vs", "bot.frag");

#pragma region "object_initialization"
		// Set the object data (buffers, vertex attributes)
		GLfloat cubeVertices[] = {
			// Positions          // Normals
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
		};
		GLfloat skyboxVertices[] = {
			// Positions          
			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,

			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};

		// Setup cube VAO
		GLuint cubeVAO, cubeVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
		// Setup skybox VAO
		GLuint skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);

#pragma endregion

		// Cubemap (Skybox)
		std::vector<std::string> faces;
		faces.push_back("skybox/right1.jpg");
		faces.push_back("skybox/left1.jpg");
		faces.push_back("skybox/top1.jpg");
		faces.push_back("skybox/bottom1.jpg");
		faces.push_back("skybox/back1.jpg");
		faces.push_back("skybox/front1.jpg");
		GLuint skyboxTexture = loadCubemap(faces);

		// Load nanosuit using our model loader
		Model nanosuit("objects/nanosuit.obj");
		Model cyborg("objects/cyborg.obj");

		// Draw as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#pragma region "mapping"

		// Setup and compile our shaders
		Shader mappingShader("shaders/normal_mapping.vs", "shaders/normal_mapping.frag");

		// Load textures
		GLuint diffuseMap = loadTexture("images/brickwall.jpg");
		GLuint normalMap = loadTexture("images/brickwall_normal.jpg");

		// Set texture units 
		mappingShader.Use();
		glUniform1i(glGetUniformLocation(mappingShader.Program, "diffuseMap"), 0);
		glUniform1i(glGetUniformLocation(mappingShader.Program, "normalMap"), 1);

		// Light position
		glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

#pragma endregion

		// Game loop
		while (!glfwWindowShouldClose(window))
		{
			// Set frame time
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Check and call events
			glfwPollEvents();
			Do_Movement();

			// Clear buffers
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// Draw scene as normal
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (GLfloat)glfwGetTime(), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
			setShader(reflectionShader, model, view, projection);

			// Cubes
			glBindVertexArray(cubeVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			glm::mat4 refractionModel;
			refractionModel = glm::translate(refractionModel, glm::vec3(-3.0f, 0.0f, 0.0f));
			refractionModel = glm::rotate(refractionModel, (GLfloat)glfwGetTime(), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
			glm::mat4 refractionView = camera.GetViewMatrix();
			glm::mat4 refractionProjection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 5.0f, 100.0f);
			setShader(shader, refractionModel, refractionView, refractionProjection);

			// Cubes
			glBindVertexArray(cubeVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			// bot
			glm::mat4 botModel;
			botModel = glm::translate(botModel, glm::vec3(6.0f, 0.0f, 0.0f));
			botModel = glm::rotate(botModel, (GLfloat)glfwGetTime(), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
			glm::mat4 botView = camera.GetViewMatrix();
			glm::mat4 botProjection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
			setShader(botShader, botModel, botView, botProjection);
			glActiveTexture(GL_TEXTURE3); // We already have 3 texture units active (in this shader) so set the skybox as the 4th texture unit (texture units are 0 based so index number 3)
			glUniform1i(glGetUniformLocation(botShader.Program, "skybox"), 3);
			// Now draw the nanosuit
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			nanosuit.Draw(botShader);

			// cyborg
			glm::mat4 cyborgModel;
			cyborgModel = glm::translate(cyborgModel, glm::vec3(-6.0f, 0.0f, 0.0f));
			cyborgModel = glm::rotate(cyborgModel, (GLfloat)glfwGetTime(), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
			glm::mat4 cyborgView = camera.GetViewMatrix();
			glm::mat4 cyborgProjection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
			setShader(fresnelShader, cyborgModel, cyborgView, cyborgProjection);
			glActiveTexture(GL_TEXTURE10); // We already have 3 texture units active (in this shader) so set the skybox as the 4th texture unit (texture units are 0 based so index number 3)
			glUniform1i(glGetUniformLocation(fresnelShader.Program, "skybox"), 3);
			// Now draw the nanosuit
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			cyborg.Draw(fresnelShader);

			// Draw skybox as last
			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader.Use();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			// skybox cube
			glBindVertexArray(skyboxVAO);

			// bot
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(fresnelShader.Program, "skybox"), 0);

			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // Set depth function back to default



			// Configure view/projection matrices
			mappingShader.Use();
			glm::mat4 mappingView = camera.GetViewMatrix();
			glm::mat4 mappingProjection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(mappingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(mappingView));
			glUniformMatrix4fv(glGetUniformLocation(mappingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(mappingProjection));
			// Render normal-mapped quad
			glm::mat4 mappingModel;
			mappingModel = glm::translate(mappingModel, glm::vec3(3.0f, -3.0f, 0.0f));
			mappingModel = glm::rotate(mappingModel, (GLfloat)glfwGetTime(), glm::normalize(glm::vec3(0.0, 1.0, 0.0))); // Rotates the quad to show normal mapping works in all directions
			glUniformMatrix4fv(glGetUniformLocation(mappingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(mappingModel));
			glUniform3fv(glGetUniformLocation(mappingShader.Program, "lightPos"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(mappingShader.Program, "viewPos"), 1, &camera.Position[0]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
			RenderQuad();

			// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
			mappingModel = glm::mat4();
			mappingModel = glm::translate(mappingModel, lightPos);
			mappingModel = glm::scale(mappingModel, glm::vec3(0.1f));
			glUniformMatrix4fv(glGetUniformLocation(mappingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(mappingModel));
			RenderQuad();


			// Swap the buffers
			glfwSwapBuffers(window);
		}

		glfwTerminate();
		return 0;
	}

	// Loads a cubemap texture from 6 individual texture faces
	// Order should be:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front)? (CHECK THIS)
	// -Z (back)?
	GLuint loadCubemap(std::vector<std::string> faces)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);

		int width, height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for (GLuint i = 0; i < faces.size(); i++)
		{
			image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return textureID;
	}


	// RenderQuad() Renders a 1x1 quad in NDC
	GLuint quadVAO = 0;
	GLuint quadVBO;
	void RenderQuad()
	{
		if (quadVAO == 0)
		{
			// positions
			glm::vec3 pos1(-1.0, 1.0, 0.0);
			glm::vec3 pos2(-1.0, -1.0, 0.0);
			glm::vec3 pos3(1.0, -1.0, 0.0);
			glm::vec3 pos4(1.0, 1.0, 0.0);
			// texture coordinates
			glm::vec2 uv1(0.0, 1.0);
			glm::vec2 uv2(0.0, 0.0);
			glm::vec2 uv3(1.0, 0.0);
			glm::vec2 uv4(1.0, 1.0);
			// normal vector
			glm::vec3 nm(0.0, 0.0, 1.0);

			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1, bitangent1;
			glm::vec3 tangent2, bitangent2;
			// - triangle 1
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent1 = glm::normalize(bitangent1);

			// - triangle 2
			edge1 = pos3 - pos1;
			edge2 = pos4 - pos1;
			deltaUV1 = uv3 - uv1;
			deltaUV2 = uv4 - uv1;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent2 = glm::normalize(tangent2);


			bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent2 = glm::normalize(bitangent2);


			GLfloat quadVertices[] = {
				// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
				pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
				pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
			};
			// Setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}


	// This function loads a texture from file. Note: texture loading functions like these are usually 
	// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
	// For learning purposes we'll just define it as a utility function.
	GLuint loadTexture(GLchar const * path)
	{
		//Generate texture ID and load texture data 
		GLuint textureID;
		glGenTextures(1, &textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);
		return textureID;
	}

#pragma region "User input"

	// Moves/alters the camera positions based on user input
	void Do_Movement()
	{
		// Camera controls
		if (keys[GLFW_KEY_W])
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (keys[GLFW_KEY_S])
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (keys[GLFW_KEY_A])
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (keys[GLFW_KEY_D])
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	// Is called whenever a key is pressed/released via GLFW
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(yoffset);
	}

#pragma endregion
//}