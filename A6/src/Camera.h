#pragma once

#include <glm/glm.hpp>
#include "Ray3D.h"

class Camera {
public:
	Camera(glm::dvec4 location, double fovy, int imageWidth, int imageHeight);

	// Create a ray from the camera position to the center of a given pixel
	Ray3D CreateCameraRay(int rowNum, int colNum);

private:
	glm::dvec4 pos;
	double fovY;
	double aspect;
	int imageWidth;
	int imageHeight;
	double imagePlaneDist;
};