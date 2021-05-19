#pragma once
#include "Camera.h"

using namespace glm;
using namespace std;

void Camera::ClampColor(glm::dvec3& color) {
	ClampDouble(color.x, 0.0, 1.0);
	ClampDouble(color.y, 0.0, 1.0);
	ClampDouble(color.z, 0.0, 1.0);
}

Camera::Camera(int imageWidth, int imageHeight, glm::dvec4 _pos, glm::dvec3 _rot, double _fov) :
	imageWidth(imageWidth),
	imageHeight(imageHeight),
	aspect(imageWidth / (double)imageHeight),
	pos(_pos),
	rot(_rot)
{
	Setup();
}

Ray3D Camera::CreateCameraRay(int rowNum, int colNum) {
	// u and v are in normalized image coords, -1 to 1
	double v = (2.0 * ((double)rowNum + (rand() / (double)RAND_MAX)) / (double)imageHeight) - 1.0;
	double u = (2.0 * ((double)colNum + (rand() / (double)RAND_MAX)) / (double)imageWidth) - 1.0;
	dvec4 rayDir = normalize(dvec4(u * aspect, v, -1.0 * imagePlaneDist, 0.0f));
	rayDir = rayDir * inv_rotMtx;
	return Ray3D(pos, rayDir);
}

void Camera::Setup() {
	imagePlaneDist = 1.0 / tan(fovY / 2.0);
	inv_rotMtx = inverse(eulerAngleXYZ(rot.x, rot.y, rot.z));
}

void Camera::ClampDouble(double& num, double min, double max) {
	if (num < min) num = min;
	if (num > max) num = max;
}