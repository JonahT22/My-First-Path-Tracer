#pragma once
#include <iostream>
#include "Camera.h"

using namespace glm;
using namespace std;

dvec3 Camera::ClampColor(glm::dvec3& color) {
	return dvec3(
		ClampDouble(color.x, 0.0, 1.0),
		ClampDouble(color.y, 0.0, 1.0),
		ClampDouble(color.z, 0.0, 1.0)
	);

}

Camera::Camera(int imageWidth, int imageHeight, glm::dvec4 _pos, glm::dvec3 _rot, double _fov, double _exposure) :
	imageWidth(imageWidth),
	imageHeight(imageHeight),
	aspect(imageWidth / (double)imageHeight),
	pos(_pos),
	rot(_rot),
	exposure(_exposure)
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

dvec3 Camera::ApplyTonemapping(glm::dvec3& color, Tonemapper tonemapper)
{
	switch (tonemapper) {
	case Tonemapper::ACES_APPROX:
		return ACESApprox(color);
		break;
	case Tonemapper::SIMPLE_CLAMP:
	default:
		return ClampColor(color);
		break;
	}
}

dvec3 Camera::ColorLinearToSRGB(glm::dvec3& linearColor)
{
	return dvec3(
		DoubleLinearToSRGB(linearColor.x),
		DoubleLinearToSRGB(linearColor.y),
		DoubleLinearToSRGB(linearColor.z)
	);
}

dvec3 Camera::ColorSRGBToLinear(glm::dvec3& srgbColor)
{
	return dvec3(
		DoubleSRGBToLinear(srgbColor.x),
		DoubleSRGBToLinear(srgbColor.y),
		DoubleSRGBToLinear(srgbColor.z)
	);
}

double Camera::DoubleLinearToSRGB(double val)
{
	// Linear->SRGB formula from https://www.nayuki.io/page/srgb-transform-library
	if (val < 0.0031308) {
		return val / 12.92;
	}
	else {
		return (1.055 * pow(val, 1.0 / 2.4)) - 0.055;
	}
}

double Camera::DoubleSRGBToLinear(double val)
{
	// SRGB->Linear formula from https://www.nayuki.io/page/srgb-transform-library
	if (val < 0.04045) {
		return val / 12.92;
	}
	else {
		return pow((val + 0.055) / 1.055, 2.4);
	}
}

double Camera::ClampDouble(double num, double min, double max) {
	if (num < min) return min;
	if (num > max) return max;
	return num;
}

dvec3 Camera::ACESApprox(glm::dvec3& color)
{
	// Using an approximation of the ACES curve by Krzysztof Narkowicz https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
	double a = 2.51;
	double b = 0.03;
	double c = 2.43;
	double d = 0.59;
	double e = 0.14;
	dvec3 testColor =  (color * (a * color + b)) / (color * (c * color + d) + e);
	return ClampColor(testColor);
}
