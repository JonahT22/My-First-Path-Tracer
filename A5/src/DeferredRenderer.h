#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "GLSL.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Camera.h"

// Base class for any object that will be rendered in the scene - contains transform, material, texture, and rendering functions
class DeferredRenderer
{
public:

	DeferredRenderer(std::shared_ptr<Shape> squareShape, int width, int height);
	
	void InitTextures();
	void SetupPass1(GLFWwindow* window);
	void SetupPass2(GLFWwindow* window);
	void RenderPass2(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog);

private:
	int textureWidth = 1024;
	int textureHeight = 1024;
	std::shared_ptr<Shape> shape_unitsquare;
	glm::vec3 translation = glm::vec3(0, 0, -1);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	GLuint framebufferID = 0;
	GLuint tex_posID = 0;
	GLuint tex_norID = 0;
	GLuint tex_keID = 0;
	GLuint tex_kdID = 0;
};
