#pragma once
#include "Camera.h"

// Create a ray from the camera position to the center of a given pixel
Ray3D Camera::CreateCameraRay(int rowNum, int colNum) {
	// u and v are in normalized image coords, -1 to 1
	double v = (2.0 * ((double)rowNum + 0.5) / (double)imageHeight) - 1.0;
	double u = (2.0 * ((double)colNum + 0.5) / (double)imageWidth) - 1.0;
	glm::vec4 rayDir = glm::normalize(glm:: vec4(u * aspect, v, -1.0 * imagePlaneDist, 0.0f));
	return Ray3D(pos, rayDir);
}