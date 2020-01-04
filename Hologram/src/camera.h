#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Mouse
extern bool firstMouse ;
extern float lastX, lastY ;
extern float yaw, pitch;
extern float fov ;


extern float deltaTime ;	// Time between current frame and last frame
extern float lastFrame ; // Time of last frame


//camera practise
extern glm::vec3 cameraPos ;
extern glm::vec3 cameraFront ;
extern glm::vec3 cameraUp ;


void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);