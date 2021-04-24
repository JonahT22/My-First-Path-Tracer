#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "SceneObject.h"
#include "Sphere.h"
#include "Ray3D.h"

class Scene {
public:
	Scene() = default;
	
	// Iterate over all objects/lights in the scene to find the color of the given ray, returns vec3 with rgb values from 0 to 1
	glm::vec3 ComputeRayColor(Ray3D ray);
	void BuildSceneFromFile(std::string filename);

	// TODO: add another function for finding shadow intersections. Makes a new ray
	// tracing loop over all objects, but initializes the hit result's t to the distance to the light,
	// and stops after the first new tmin is found
	// This function won't be on the sceneobject class, it'll be on the scene class, and will be implemented
	// using this Hit() function

private:
	std::vector<std::shared_ptr<SceneObject> > allObjects;
	// TODO: vector of lights
};