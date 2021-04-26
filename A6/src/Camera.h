#pragma once

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "Ray3D.h"

class Camera {
public:
	// Location, rotation, and fov args are optional, usually set by reading a scene file
	Camera(int imageWidth, int imageHeight,
		glm::dvec4 location = glm::dvec4(0, 0, 5, 1), double fovy = (45.0 * glm::pi<double>() / 180.0));

	// Create a ray from the camera position to the center of a given pixel
	Ray3D CreateCameraRay(int rowNum, int colNum);
	
	// Setter functions
	void SetPosition(glm::dvec4 _pos) { pos = _pos; }
	// TODO: camera rotation
	void SetFOV(double _fov);

private:
	glm::dvec4 pos;
	double fovY;
	double aspect;
	int imageWidth;
	int imageHeight;
	double imagePlaneDist;
};