#pragma once
#include "Camera.h"

using namespace glm;

Camera::Camera(int imageWidth, int imageHeight, glm::dvec4 _pos, glm::dvec3 _rot, double _fovY) :
	imageWidth(imageWidth),
	imageHeight(imageHeight),
	aspect(imageWidth / (double)imageHeight),
	pos(_pos),
	rot(_rot),
	fovY(_fovY),
	// Set the distance to the image plane so that the image coords go from -1 to 1
	imagePlaneDist(1.0 / tan(fovY / 2.0)) {}

Ray3D Camera::CreateCameraRay(int rowNum, int colNum) {
	// TODO: need to take rotation into account
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
