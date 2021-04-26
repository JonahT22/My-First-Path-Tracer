#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Camera.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "Ray3D.h"
#include "Material.h"
#include "PointLight.h"
#include "Plane.h"

class Scene {
public:
	Scene() = default;
	
	// Iterate over all objects/lights in the scene to find the color of the given ray, returns dvec3 with rgb values from 0 to 1
	glm::dvec3 ComputeRayColor(Ray3D ray);
	void BuildSceneFromFile(std::string filename, Camera& camera);

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

	// Reads the next value and places it into the provided type
	template<class T> T ReadValue(std::istringstream& stream);
	// Reads the next 3 values from the stream and places them into a dvec3
	glm::dvec3 ReadVec3(std::istringstream& stream);
	// Reads the parameters for SceneObject construction, then adds it to the allobjects list
	template<class ObjectType> std::shared_ptr<ObjectType> ReadObject(std::istringstream& stream);
};

template<class T>
inline T Scene::ReadValue(std::istringstream& stream) {
	T temp;
	stream >> temp;
	return temp;
}

template<class ObjectType>
inline std::shared_ptr<ObjectType> Scene::ReadObject(std::istringstream& stream) {
	shared_ptr<ObjectType> temp = std::make_shared<ObjectType>(
		ReadValue<std::string>(stream), // Name
		Transform(
			glm::dvec4(ReadVec3(stream), 1), // Position
			ReadVec3(stream),                // Rotation
			ReadVec3(stream)                 // Scale
		),
		Material(
			ReadVec3(stream),         // Kd
			ReadVec3(stream),         // Ks
			ReadVec3(stream),         // Ka
			ReadValue<double>(stream) // Exp
		)
	);
	return temp;
}