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

	bool Hit(Ray3D ray, HitResult& outHit);

	// TODO: add epsilon to this intersection function, and discard any t values below it
	virtual bool IntersectLocal(Ray3D ray, HitResult& outHit) = 0;

	// TODO: add another function for finding shadow intersections. Makes a new ray
	// tracing loop over all objects, but initializes the hit result's t to the distance to the light,
	// and stops after the first new tmin is found
	// This function won't be on the sceneobject class, it'll be on the scene class, and will be implemented
	// using this Hit() function

private:
	// The transformation matrix to convert this object from local->world space
	glm::mat4 transMtx = glm::mat4(1.0f);
	// TODO: is this variable necessary after finding transMtx?
	Transform transform;
};