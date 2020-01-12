#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <camera.h>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);


bool firstMouse = true;
float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
float fov = 45.0f;


float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


//camera practise
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	/*  Functions  */
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(int shader);
private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(int shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		glUniform1f(glGetUniformLocation(shader, ("material." + name + number).c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


class Model
{
public:
	/*  Functions   */
	Model(const char *path)
	{
		loadModel(path);
	}
	void Draw(int shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}
private:
	/*  Model Data  */
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	/*  Functions   */
	void loadModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode *node, const aiScene *scene)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			// process vertex positions, normals and texture coordinates
			//...
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		// process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		//...
			// process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material,
				aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // add to loaded textures
			}
		}
		return textures;
	}
};

int main()
{

		// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	int ourShader = gen_shader("shaders/model_loading.fs.txt", "shaders/model_loading.vs.txt");
	Model ourModel("nanosuit/nanosuit.obj");
	//int lampShaderProgram = gen_shader("shaders/lamp.frag.txt", "shaders/lamp.vs.txt");
	
	// as we only have a single shader, we could also just activate our shader once beforehand if we want to 
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	//float vertices[] = {
	//	// positions          // normals           // texture coords
	//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	//	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	//};
	//
	//glm::vec3 cubePositions[] = {
 // glm::vec3(0.0f,  0.0f,  0.0f),
 // glm::vec3(2.0f,  5.0f, -15.0f),
 // glm::vec3(-1.5f, -2.2f, -2.5f),
 // glm::vec3(-3.8f, -2.0f, -12.3f),
 // glm::vec3(2.4f, -0.4f, -3.5f),
 // glm::vec3(-1.7f,  3.0f, -7.5f),
 // glm::vec3(1.3f, -2.0f, -2.5f),
 // glm::vec3(1.5f,  2.0f, -2.5f),
 // glm::vec3(1.5f,  0.2f, -1.5f),
 // glm::vec3(-1.3f,  1.0f, -1.5f)
	//};

	//// positions of the point lights
	//glm::vec3 pointLightPositions[] = {
	//	glm::vec3(0.7f,  0.2f,  2.0f),
	//	glm::vec3(2.3f, -3.3f, -4.0f),
	//	glm::vec3(-4.0f,  2.0f, -12.0f),
	//	glm::vec3(0.0f,  0.0f, -3.0f)
	//};
	//unsigned int VBO, cubeVAO;
	//glGenVertexArrays(1, &cubeVAO);
	//glGenBuffers(1, &VBO);


	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindVertexArray(cubeVAO);

	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//// color attribute
	//
	//unsigned int lightVAO;
	//glGenVertexArrays(1, &lightVAO);
	//glBindVertexArray(lightVAO);

	//// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//unsigned int texture;
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// load image, create texture and generate mipmaps
	//int width, height, nrChannels;
	//unsigned char *data = stbi_load("textures/container2.png", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);

	//}
	//else
	//{
	//	std::cout << "Failed to load texture." << std::endl;
	//}
	//stbi_image_free(data);

	//unsigned int specularMap;
	//glGenTextures(1, &specularMap);
	//glBindTexture(GL_TEXTURE_2D, specularMap);
	//// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// load image, create texture and generate mipmaps
	//unsigned char *data1 = stbi_load("textures/container2_specular.png", &width, &height, &nrChannels, 0);
	//if (data1)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
	//	glGenerateMipmap(GL_TEXTURE_2D);

	//}
	//else
	//{
	//	std::cout << "Failed to load texture." << std::endl;
	//}
	//stbi_image_free(data1);
	//glEnable(GL_DEPTH_TEST);
	////light position
	//glUseProgram(lightShaderProgram);
	//glUniform1i(glGetUniformLocation(lightShaderProgram, "material.diffuse"), 0);
	//glUniform1i(glGetUniformLocation(lightShaderProgram, "material.specular"), 1);

		// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		//glUseProgram(lightShaderProgram);
		//glUniform3fv(glGetUniformLocation(lightShaderProgram, "viewPos"), 1, &cameraPos[0]);

		//glUniform1f(glGetUniformLocation(lightShaderProgram, "material.shininess"), 32.0f);

		//glUniform3f(glGetUniformLocation(lightShaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		//glUniform3fv(glGetUniformLocation(lightShaderProgram, "pointLights[0].position"),1, &pointLightPositions[0][0]);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[0].constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[0].linear"), 0.09);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[0].quadratic"), 0.032);


		//glUniform3fv(glGetUniformLocation(lightShaderProgram, "pointLights[1].position"), 1, &pointLightPositions[1][0]);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[1].constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[1].linear"), 0.09);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[1].quadratic"), 0.032);


		//glUniform3fv(glGetUniformLocation(lightShaderProgram, "pointLights[2].position"), 1, &pointLightPositions[2][0]);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[2].constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[2].linear"), 0.09);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[2].quadratic"), 0.032);


		//glUniform3fv(glGetUniformLocation(lightShaderProgram, "pointLights[3].position"), 1, &pointLightPositions[3][0]);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[3].constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[3].linear"), 0.09);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "pointLights[3].quadratic"), 0.032);

		//glUniform3fv(glGetUniformLocation(lightShaderProgram,"spotLight.position"),1, &cameraPos[0]);
		//glUniform3fv(glGetUniformLocation(lightShaderProgram, "spotLight.direction"),1, &cameraFront[0]);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(lightShaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "spotLight.constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "spotLight.linear"), 0.09);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "spotLight.quadratic"), 0.032);
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		//glUniform1f(glGetUniformLocation(lightShaderProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		//
		//

		//glm::mat4 view= glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//int viewLoc = glGetUniformLocation(lightShaderProgram, "view");
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//int w, h;
		//glfwGetWindowSize(window, &w, &h);
		//float aspect_ratio = (float)w / (float)h;
		//glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
		//int projectionLoc = glGetUniformLocation(lightShaderProgram, "projection");
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture);

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, specularMap);

		//glBindVertexArray(cubeVAO);
		//int modelLoc = glGetUniformLocation(lightShaderProgram, "model");
		//for (unsigned int i = 0; i < 10; i++)
		//{
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, cubePositions[i]);
		//	float angle = 20.0f * i;
		//	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}


		//glUseProgram(lampShaderProgram);
		// projectionLoc = glGetUniformLocation(lampShaderProgram, "projection");
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// viewLoc = glGetUniformLocation(lampShaderProgram, "view");
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//glBindVertexArray(lightVAO);
		//modelLoc = glGetUniformLocation(lampShaderProgram, "model");
		//for (unsigned int i = 0; i < 4; i++)
		//{
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, pointLightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(ourShader);
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		float aspect_ratio = (float)w / (float)h;
		glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
		int projectionLoc = glGetUniformLocation(ourShader, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		int viewLoc = glGetUniformLocation(ourShader, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		int modelLoc = glGetUniformLocation(ourShader, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		ourModel.Draw(ourShader);

		//}
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &cubeVAO);
	//glDeleteVertexArrays(1, &lightVAO);

	//glDeleteBuffers(1, &VBO);
	

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = path;
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}