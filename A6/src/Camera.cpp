#pragma once
#include "Camera.h"

using namespace glm;
using namespace std;

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
	double v = (2.0 * ((double)rowNum + 0.5) / (double)imageHeight) - 1.0;
	double u = (2.0 * ((double)colNum + 0.5) / (double)imageWidth) - 1.0;
	dvec4 rayDir = normalize(dvec4(u * aspect, v, -1.0 * imagePlaneDist, 0.0f));
	rayDir = rayDir * inv_rotMtx;
	return Ray3D(pos, rayDir);
}

void Camera::Setup()
{
	imagePlaneDist = 1.0 / tan(fovY / 2.0);
	inv_rotMtx = inverse(eulerAngleXYZ(rot.x, rot.y, rot.z));
}

