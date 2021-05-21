#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <string>
#include <memory>
#include <iostream>
#include "Ray3D.h"
#include "Transform.h"
#include "HitResult.h"
#include "Material.h"

class SceneObject {
public:
	// Assign material and calculate the transformation matrix
	SceneObject(std::string _name, Transform _transf, std::shared_ptr<Material> _mat);

	std::shared_ptr<Material> GetMaterial() { return mat; }
	glm::dvec4 GetLocation() { return transf.translation; }
	glm::dmat4 GetInverseTranspose() { return invTranspMtx; }

	// By default, hits go from 0 to inf unless override is specified
	bool Hit(Ray3D& ray, HitResult& outHit, double tMin = 0, double tMax = std::numeric_limits<double>::max());

	// Check intersection in local space, return true if found an intersection that is closer than the hit's t value
	virtual bool IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) = 0;
	// Returns the world-space location of a random point on the object's surface, and return the pdf by reference
	virtual glm::dvec4 GetRandomPointOnSurface(double& pdf, glm::dvec4& normal) = 0;

	std::string name;
	bool hasRandomPointMethodDefined = false;
protected:
	// Checks 2 numbers against a given range, returns index of smaller # in the range, or -1 if neither are in the range
	int SelectSmallestInRange(double vals[2], double min, double max);
	// Matrix for converting points from local->world space
	glm::dmat4 modelMtx;
	// Matrix for converting rays from world->local space (inverse of model matrix)
	glm::dmat4 invMtx;
	// Matrix for converting normals from local->world space
	glm::dmat4 invTranspMtx;
	// Transformations applied to this object
	Transform transf;
	// Material properties - Store the material as a shared_ptr since other classes reference it frequently
	std::shared_ptr<Material> mat;
};