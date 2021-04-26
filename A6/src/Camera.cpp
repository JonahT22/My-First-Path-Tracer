#pragma once
#include "Camera.h"

using namespace glm;

Camera::Camera(int imageWidth, int imageHeight, glm::dvec4 location, double fovy) :
	pos(location),
	fovY(fovy),
	aspect(imageWidth / imageHeight),
	imageWidth(imageWidth),
	imageHeight(imageHeight),
	// Set the distance to the image plane so that the image coords go from -1 to 1
	imagePlaneDist(1.0 / tan(fovY / 2.0)) {}

// Create a ray from the camera position to the center of a given pixel
Ray3D Camera::CreateCameraRay(int rowNum, int colNum) {
	// u and v are in normalized image coords, -1 to 1
	double v = (2.0 * ((double)rowNum + 0.5) / (double)imageHeight) - 1.0;
	double u = (2.0 * ((double)colNum + 0.5) / (double)imageWidth) - 1.0;
	dvec4 rayDir = normalize(dvec4(u * aspect, v, -1.0 * imagePlaneDist, 0.0f));
	return Ray3D(pos, rayDir);
}

void Camera::SetFOV(double _fov)
{
	// Make sure to update the new image plane distance to keep image plane coords normalized
	fovY = _fov;
	imagePlaneDist = 1.0 / tan(fovY / 2.0);
}
