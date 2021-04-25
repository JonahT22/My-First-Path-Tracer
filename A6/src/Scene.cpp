#pragma once
#include "Scene.h"

#include <iostream>
using namespace glm;
using namespace std;

// Main render loop
glm::vec3 Scene::ComputeRayColor(Ray3D ray)
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
		hit.nor = normalize(hit.hitObject->GetInverseTranspose() * hit.nor);

		// Start with ambient component
		Material mat = hit.hitObject->GetMaterial();
		vec3 color = mat.ka;

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
		//return vec3(r, g, b);
	}
	// Background color
	else return glm::vec3(0, 0, 0);
}

bool Scene::IsPointInShadow(vec4& hitLoc, PointLight& light) const
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
	// Red sphere
	allObjects.push_back(
		make_shared<Sphere>(
			Transform(
				vec4(-.5, -1, 1, 1),
				vec3(0, 0, 0),
				vec3(1, 1, 1)
			),
			Material(
				vec3(1.0, 0.0, 0.0),
				vec3(1.0, 1.0, 0.5),
				vec3(0.1, 0.1, 0.1),
				100.0f
			)
		)
	);
	// Green sphere
	allObjects.push_back(
		make_shared<Sphere>(
			Transform(
				vec4(.5, -1, -1, 1),
				vec3(0, 0, 0),
				vec3(1, 1, 1)
			),
			Material(
				vec3(0.0, 1.0, 0.0),
				vec3(1.0, 1.0, 0.5),
				vec3(0.1, 0.1, 0.1),
				100.0f
			)
		)
	);
	// Blue sphere
	allObjects.push_back(
		make_shared<Sphere>(
			Transform(
				vec4(0, 1, 0, 1),
				vec3(0, 0, 0),
				vec3(1, 1, 1)
			),
			Material(
				vec3(0.0, 0.0, 1.0),
				vec3(1.0, 1.0, 0.5),
				vec3(0.1, 0.1, 0.1),
				100.0f
			)
			)
	);
	
	allLights.push_back(PointLight(vec4(-2, 1, 1, 1), 1.0));
}

void Scene::ClampVector(glm::vec3& vec, float min, float max)
{
	ClampFloat(vec.x, min, max);
	ClampFloat(vec.y, min, max);
	ClampFloat(vec.z, min, max);
}

void Scene::ClampFloat(float& num, float min, float max)
{
	if (num < min) num = min;
	if (num > max) num = max;
}
