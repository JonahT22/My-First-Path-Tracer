#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "HelperStructs.h"

class SceneObject {
public:
	SceneObject() = default;

	// Calcualte the transformation matrix for this object
	void CreateTransformMtx(Transform _transf);

	// By default, hits go from 0 to inf unless override is specified
	bool Hit(Ray3D ray, HitResult& outHit, double tMin = 0, double tMax = std::numeric_limits<double>::max());

	// TODO: add epsilon to this intersection function, and discard any t values below it
	virtual bool IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) = 0;

	// TODO: add another function for finding shadow intersections. Makes a new ray
	// tracing loop over all objects, but initializes the hit result's t to the distance to the light,
	// and stops after the first new tmin is found
	// This function won't be on the sceneobject class, it'll be on the scene class, and will be implemented
	// using this Hit() function

protected:
	bool SelectSmallestInRange(double a, double b, double min, double max, double& result);

	// The transformation matrix to convert this object from local->world space
	glm::mat4 transMtx = glm::mat4(1.0f);
	// TODO: is this variable necessary after finding transMtx?
	Transform transform;
};