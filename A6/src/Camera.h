#pragma once

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "Ray3D.h"

class Camera {
public:
	// Location, rotation, and fov args are optional, usually set by reading a scene file
	Camera(int imageWidth, int imageHeight,
		glm::dvec4 _pos = glm::dvec4(0, 0, 5, 1), 
		glm::dvec3 _rot = glm::dvec3(0, 0, 0), 
		double _fovY = (45.0 * glm::pi<double>() / 180.0));

	// Create a ray from the camera position to the center of a given pixel
	Ray3D CreateCameraRay(int rowNum, int colNum);
	
	// Setter functions
	void SetPosition(glm::dvec4 _pos) { pos = _pos; }
	void SetRotation(glm::dvec3 _rot) { rot = _rot; }
	void SetFOV(double _fov);

private:
	glm::dvec4 pos;
	glm::dvec3 rot;
	double fovY;
	double aspect;
	int imageWidth;
	int imageHeight;
	double imagePlaneDist;
};