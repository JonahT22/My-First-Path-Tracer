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
			// TODO problem: do I need a tMax in the Hit function if this only returns true when it finds something smaller
			// than the default value of t in the hit? I.e. instead of setting tMax at a number, just set the default value of t
			// to that number
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

		// Calculate contribution of each light using simple diffuse shading model
		for (auto& light : allLights) {
			//float diffuseFactor = std::max(0.0f, dot(hit.nor, normalize(light->pos - hit.loc)));
			//color += mat.kd * diffuseFactor * light->intensity;
			color += mat.ShadeBlinnPhong(hit, ray, light);
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
