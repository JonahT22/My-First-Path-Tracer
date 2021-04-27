#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "tiny_obj_loader.h"

#include "SceneObject.h"
#include "Ray3D.h"
#include "HitResult.h"
#include "Sphere.h"
#include "Triangle.h"

class TriangleMesh : public SceneObject {
public:
	// Call parent constructor to create transform matrix and apply material
	TriangleMesh(std::string _name, Transform _transf, Material _mat) : SceneObject(_name, _transf, _mat) {}
	void LoadMeshFile(std::string filename);

	bool IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) override;

private:

	// List of triangles
	std::vector<std::shared_ptr<Triangle> > allTriangles;
	std::unique_ptr<Sphere> boundingSphere;
};