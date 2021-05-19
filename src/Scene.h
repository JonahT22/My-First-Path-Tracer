#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "nlohmann/json.hpp"
#include "Camera.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "Ray3D.h"
#include "Material.h"
#include "PointLight.h"
#include "Plane.h"
#include "TriangleMesh.h"
#include "Square.h"
#include "Box.h"
#include "Light.h"
#include "PointLight.h"
#include "EmissiveLight.h"

class Scene {
public:
	Scene(glm::dvec3 _bgColor) : backgroundColor(_bgColor) {}
	
	// Iterate over all objects/lights in the scene to find the color of the given ray, returns dvec3 with rgb values from 0 to 1
	glm::dvec3 ComputeRayColor(Ray3D& ray, int depth = 0, bool specularRay = false);
	void BuildSceneFromFile(std::string filename, Camera& camera);

private:
	// This is a vector of shared_ptrs since the hitResult object needs to be able to point to them
	std::vector<std::shared_ptr<SceneObject> > allObjects;
	std::vector<std::shared_ptr<Light> > allLights;

	glm::dvec3 backgroundColor = glm::dvec3(0, 0, 0);

	// "Fudge Factor" to avoid self-intersection on shadow/reflection ray hits
	const double epsilon = 0.00001; // 1e-5
	// Maximum number of times the ComputeRayColor can recurse before forcibly returning
	const int maxRecursionDepth = 3;
	
	// Run an intersection check on the ray to a given light, but return false immediately if a hit is found
	bool IsPointInShadow(glm::dvec4& hitLoc, glm::dvec4& lightLoc, std::shared_ptr<SceneObject> lightObj = nullptr) const;
	// Clamps the value of each component of the given ddvec3
	void ClampVector(glm::dvec3& vec, double min, double max);
	// Clamps an individual double value to a range
	void ClampDouble(double& num, double min, double max);
	// Find a random unit vector from center->surface of a hemisphere with the given normal
	glm::dvec4 GetRandomRayInHemisphere(glm::dvec4& normal);
	// Find a reflection vector, optionally perturbed by a 'roughness' factor (for glossy reflections)
	glm::dvec4 GetReflectionRay(glm::dvec4& rayDir, glm::dvec4& hitNor, double roughness = 0.0);

	// Reads the next 3 values from the stream and places them into a dvec3
	glm::dvec3 ReadVec3(const nlohmann::json& j);
	// Reads location, rotation, and scale vectors from a json transform object
	Transform ReadTransform(const nlohmann::json& j);
	// Reads diffuse, specular, and emissive colors from a json material object
	std::shared_ptr<Material> ReadMaterial(const nlohmann::json& j);
	// Reads the parameters for SceneObject construction, then adds it to the allobjects list
	template<class ObjectType> std::shared_ptr<ObjectType> ReadObject(const nlohmann::json& j);
};

template<class ObjectType>
inline std::shared_ptr<ObjectType> Scene::ReadObject(const nlohmann::json& j) {
	shared_ptr<ObjectType> temp = std::make_shared<ObjectType>(
		j.at("Name").get<std::string>(), 
		ReadTransform(j.at("Transform")),
		ReadMaterial(j.at("Material"))
		);
	return temp;
}