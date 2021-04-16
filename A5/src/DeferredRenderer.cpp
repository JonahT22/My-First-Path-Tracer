#include "DeferredRenderer.h"

DeferredRenderer::DeferredRenderer(std::shared_ptr<Shape> squareShape, int width, int height)
	: shape_unitsquare(squareShape), textureWidth(width), textureHeight(height)
{
	// Set the distance to camera based on its fov
	// assume a hard-coded value of 45 degrees
	float fovy = 45.0f;
	float distance = 1 / tan(fovy / 2.0f);
	translation = glm::vec3(0, 0, -1.0f * distance);
}

void DeferredRenderer::InitTextures()
{
	// Set up the offscreen framebuffer
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	// from now on, any state changes will apply to our offscreen framebuffer

	// Create position texture
	glGenTextures(1, &tex_posID);
	glBindTexture(GL_TEXTURE_2D, tex_posID);
	// Set up the texture, but don't provide a source file
	// the internal format is RGBA8 (8 RGB color attachments)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Set the framebuffer to output to this texture on attachment 0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_posID, 0);

	// Create normal texture
	glGenTextures(1, &tex_norID);
	glBindTexture(GL_TEXTURE_2D, tex_norID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Set the framebuffer to output to this texture on attachment 1
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_norID, 0);

	// Create ke texture
	glGenTextures(1, &tex_keID);
	glBindTexture(GL_TEXTURE_2D, tex_keID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex_keID, 0);

	// Create kd texture
	glGenTextures(1, &tex_kdID);
	glBindTexture(GL_TEXTURE_2D, tex_kdID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tex_kdID, 0);

	// Create the depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureWidth, textureHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set the two textures to be the output of the frame buffer
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// Error checking
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not ok" << std::endl;
	}

	// finally, reset the framebuffer to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::SetupPass1(GLFWwindow* window)
{
	// Set the frame buffer to our texture objects instead of the screen
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	// Set viewport to texture dimensions, instead of window size
	glViewport(0, 0, textureWidth, textureHeight);
	// Reset the depth/color buffers
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::SetupPass2(GLFWwindow* window)
{
	// Return the framebuffer back to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	// Reset the depth/color buffers
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Doing this every frame for now, but it would be better move this to the glfw window resize callback in main
	float aspect = (float)width / (float)height;
	scale = glm::vec3(aspect, 1.0f, 1.0f);
}

void DeferredRenderer::RenderPass2(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog)
{
	MV->pushMatrix();
	MV->translate(translation);
	MV->scale(scale);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

	// Setup textures
	// Bind tex_pos to unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_posID);
	// Bind tex_nor to unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_norID);
	// Bind tex_ke to unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_keID);
	// Bind tex_kd to unit 3
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex_kdID);


	shape_unitsquare->draw(prog);

	glActiveTexture(GL_TEXTURE0);

	MV->popMatrix();
}

