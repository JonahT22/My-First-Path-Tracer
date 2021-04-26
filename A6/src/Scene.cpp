#pragma once
#include "Scene.h"

using namespace glm;
using namespace std;

// Main render loop
glm::dvec3 Scene::ComputeRayColor(Ray3D ray)
{
	HitResult hit;
	for (auto& object : allObjects) {
		if (object->Hit(ray, hit)) {
			// If hit was successful (i.e. found a new tMin), store a reference to the object
			hit.hitObject = object;
		}
	}

	// Check if the ray actually hit anything
	if (hit.hitObject != nullptr) {
		// Calculate the hit's properties, now that we know that this hit is the closest one 
		// (this way, we only have to do these calculations once)

		// New position from tMin
		hit.loc = ray.FindLocAtTime(hit.t);

		// During intersection checks, hit.nor is filled with local-space normal. Now, convert to world space
		hit.nor = hit.hitObject->GetInverseTranspose() * hit.nor;
		// Multiplying by inverse transpose makes the w component nonzero, so reset it here before normalizing
		hit.nor.w = 0.0;
		hit.nor = normalize(hit.nor);

		// Start with ambient component
		Material mat = hit.hitObject->GetMaterial();
		dvec3 color = mat.ka;

		// Calculate contribution of each light using Blinn-phong shading
		for (auto& light : allLights) {
			if (!IsPointInShadow(hit.loc, light)) {
				color += mat.ShadeBlinnPhong(ray, hit, light);
			}
		}

		// Make sure the color isn't clipping
		ClampVector(color, 0.0f, 1.0f);
		return color;

		//// Color with normals
		//// Normal -> color conversion taken from assigment handout
		//double r = 0.5 * hit.nor.x + 0.5;
		//double g = 0.5 * hit.nor.y + 0.5;
		//double b = 0.5 * hit.nor.z + 0.5;
		//return dvec3(r, g, b);
	}
	// Background color
	else return glm::dvec3(0, 0, 0);
}

bool Scene::IsPointInShadow(dvec4& hitLoc, PointLight& light) const
{
	HitResult shadowHit;
	// Start the tval at the light distance, so nothing past the light will count as a hit
	shadowHit.t = glm::length(light.pos - hitLoc);
	// TODO problem: do I need a tMax in the Hit function if it only returns true when it finds something smaller
	// than the default value of t in the hit? I.e. instead of setting tMax to light distance, just set the 
	// default value of t to that number

	// Shadow ray is located at the hit position, goes to the light
	Ray3D shadowRay(hitLoc, glm::normalize(light.pos - hitLoc));
	for (auto& object : allObjects) {
		// Add an epsilon to this hit calculation to avoid self-shadowing
		if (object->Hit(shadowRay, shadowHit, epsilon)) {
			// If I hit anything, immediately return true, no further action required
			return true;
		}
	}

	return false;
}

void Scene::BuildSceneFromFile(std::string filename)
{
	//// Red sphere
	//allObjects.push_back(
	//	make_shared<Sphere>(
	//		Transform(
	//			dvec4(-0.5, -1, 1, 1),
	//			dvec3(0, 0, 0),
	//			dvec3(1, 1, 1)
	//		),
	//		Material(
	//			dvec3(1.0, 0.0, 0.0),
	//			dvec3(1.0, 1.0, 0.5),
	//			dvec3(0.1, 0.1, 0.1),
	//			100.0
	//		)
	//	)
	//);
	//// Green sphere
	//allObjects.push_back(
	//	make_shared<Sphere>(
	//		Transform(
	//			dvec4(.5, -1, -1, 1),
	//			dvec3(0, 0, 0),
	//			dvec3(1, 1, 1)
	//		),
	//		Material(
	//			dvec3(0.0, 1.0, 0.0),
	//			dvec3(1.0, 1.0, 0.5),
	//			dvec3(0.1, 0.1, 0.1),
	//			100.0
	//		)
	//	)
	//);
	//// Blue sphere
	//allObjects.push_back(
	//	make_shared<Sphere>(
	//		Transform(
	//			dvec4(0, 1, 0, 1),
	//			dvec3(0, 0, 0),
	//			dvec3(1, 1, 1)
	//		),
	//		Material(
	//			dvec3(0.0, 0.0, 1.0),
	//			dvec3(1.0, 1.0, 0.5),
	//			dvec3(0.1, 0.1, 0.1),
	//			100.0
	//		)
	//	)
	//);

	//allLights.push_back(PointLight(dvec4(-2, 1, 1, 1), 1.0));
	
	// Red ellipsoid
	allObjects.push_back(
		make_shared<Sphere>(
			Transform(
				dvec4(0.5, 0, 0.5, 1),
				dvec3(0, 0, 0),
				dvec3(0.5, 0.6, 0.2)
			),
			Material(
				dvec3(1.0, 0.0, 0.0),
				dvec3(1.0, 1.0, 0.5),
				dvec3(0.1, 0.1, 0.1),
				100.0
			)
		)
	);
	// Green sphere
	allObjects.push_back(
		make_shared<Sphere>(
			Transform(
				dvec4(-0.5, 0, -0.5, 1),
				dvec3(0, 0, 0),
				dvec3(1, 1, 1)
			),
			Material(
				dvec3(0.0, 1.0, 0.0),
				dvec3(1.0, 1.0, 0.5),
				dvec3(0.1, 0.1, 0.1),
				100.0
			)
		)
	);
	// White Plane
	allObjects.push_back(
		make_shared<Plane>(
			Transform(
				dvec4(0, -1, 0, 1),
				dvec3(0, 0, 0),
				dvec3(1, 1, 1)
			),
			Material(
				dvec3(1.0, 1.0, 1.0),
				dvec3(0.0, 0.0, 0.0),
				dvec3(0.1, 0.1, 0.1),
				0.0
			)
		)
	);
	
	allLights.push_back(PointLight(dvec4(1, 2, 2, 1), 0.5));
	allLights.push_back(PointLight(dvec4(-1, 2, -1, 1), 0.5));
}

void Scene::ClampVector(glm::dvec3& vec, double min, double max)
{
	ClampDouble(vec.x, min, max);
	ClampDouble(vec.y, min, max);
	ClampDouble(vec.z, min, max);
}

void Scene::ClampDouble(double& num, double min, double max)
{
	if (num < min) num = min;
	if (num > max) num = max;
}
