#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "Ray3D.h"

class Camera {
public:
	static void ClampColor(glm::dvec3& color);
	
	// Location, rotation, and fov args are optional, usually set by reading a scene file. FOV is in degrees
	Camera(int imageWidth, int imageHeight,
		glm::dvec4 _pos = glm::dvec4(0, 0, 5, 1), 
		glm::dvec3 _rot = glm::dvec3(0, 0, 0), 
		double _fov = 45.0);

	// Create a ray from the camera position to the center of a given pixel
	Ray3D CreateCameraRay(int rowNum, int colNum);
	
	// Create a transformation matrix from the pos/rot, and calculate the image plane distance
	void Setup();

	// Setter functions - NOTE: need to call Setup after running any of these
	void SetPosition(glm::dvec4& _pos) { pos = _pos; }
	void SetRotationDegrees(glm::dvec3& _rot) { rot = _rot * glm::pi<double>() / 180.0; }
	void SetFOVDegrees(double _fov) { fovY = _fov * glm::pi<double>() / 180.0; }
	

private:
	static void ClampDouble(double& num, double min, double max);

	glm::dvec4 pos;
	glm::dvec3 rot;
	// Don't need to store the whole view mtx, only the inverse mtx for rotations
	glm::dmat4 inv_rotMtx;
	double fovY;
	double aspect;
	int imageWidth;
	int imageHeight;
	double imagePlaneDist;
};