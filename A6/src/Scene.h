#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "SceneObject.h"
#include "Sphere.h"
#include "Ray3D.h"
#include "Material.h"
#include "PointLight.h"

class Scene {
public:
	Scene() = default;
	
	// Iterate over all objects/lights in the scene to find the color of the given ray, returns dvec3 with rgb values from 0 to 1
	glm::dvec3 ComputeRayColor(Ray3D ray);
	void BuildSceneFromFile(std::string filename);

	// TODO: add another function for finding shadow intersections. Makes a new ray
	// tracing loop over all objects, but initializes the hit result's t to the distance to the light,
	// and stops after the first new tmin is found
	// This function won't be on the sceneobject class, it'll be on the scene class, and will be implemented
	// using this Hit() function

private:
	// This is a vector of shared_ptrs since the hitResult object needs to be able to point to them
	std::vector<std::shared_ptr<SceneObject> > allObjects;
	std::vector<PointLight> allLights;

	const double epsilon = 0.00001; // 1e-5

	// Run an intersection check on the ray to a given light, but return false immediately if a hit is found
	bool IsPointInShadow(glm::dvec4& hitLoc, PointLight& light) const;
	// Clamps the value of each component of the given ddvec3
	void ClampVector(glm::dvec3& vec, double min, double max);
	// Clamps an individual double value to a range
	void ClampDouble(double& num, double min, double max);
};