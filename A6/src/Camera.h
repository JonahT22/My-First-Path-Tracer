#pragma once

#include <glm/glm.hpp>
#include "HelperStructs.h"

class Camera {
public:
	Camera(glm::vec4 location, double fovy, int imageWidth, int imageHeight);

	// Create a ray from the camera position to the center of a given pixel
	Ray3D CreateCameraRay(int rowNum, int colNum);

private:
	glm::vec4 pos;
	double fovY;
	double aspect;
	int imageWidth;
	int imageHeight;
	double imagePlaneDist;
};