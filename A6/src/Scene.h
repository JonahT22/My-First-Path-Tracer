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
#include "TriangleMesh.h"

class Scene {
public:
	Scene(glm::dvec3 _bgColor) : backgroundColor(_bgColor) {}
	
	// Iterate over all objects/lights in the scene to find the color of the given ray, returns dvec3 with rgb values from 0 to 1
	glm::dvec3 ComputeRayColor(Ray3D& ray, int depth = 0);
	void BuildSceneFromFile(std::string filename, Camera& camera);

private:
	// This is a vector of shared_ptrs since the hitResult object needs to be able to point to them
	std::vector<std::shared_ptr<SceneObject> > allObjects;
	std::vector<PointLight> allLights;

	glm::dvec3 backgroundColor = glm::dvec3(0, 0, 0);

	// "Fudge Factor" to avoid self-intersection on shadow/reflection ray hits
	const double epsilon = 0.00001; // 1e-5
	// Threshold for skipping blinnphong/reflection calculations in ComputeRayColor()
	const double reflectiveThreshold = 0.00001; //1e-5
	// Maximum number of times the ComputeRayColor can recurse before forcibly returning
	const int maxReflectionDepth = 5;

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
	// Need to store each of these values in a temporary varible before calling the object's constructor.
	// Since the order of argument evaluation is undefined in c++, you can't pass the Read(stream) functions straight to the constructor
	std::string name = ReadValue<std::string>(stream);
	dvec4 pos = dvec4(ReadVec3(stream), 1);
	dvec3 rot = ReadVec3(stream);
	dvec3 scale = ReadVec3(stream);
	dvec3 kd = ReadVec3(stream);
	dvec3 ks = ReadVec3(stream);
	dvec3 ka = ReadVec3(stream);
	double roughness = ReadValue<double>(stream);
	double specular = ReadValue<double>(stream);
	
	shared_ptr<ObjectType> temp = std::make_shared<ObjectType>(
		name, Transform(pos, rot, scale), Material(kd, ks, ka, roughness, specular));
	return temp;
}