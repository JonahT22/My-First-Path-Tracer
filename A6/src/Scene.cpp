#pragma once
#include "Scene.h"

using namespace glm;
using namespace std;

// Main render loop
glm::dvec3 Scene::ComputeRayColor(Ray3D& ray, int depth) {
	// Before anything else, make sure I haven't passed the recursion depth
	if (depth > maxRecursionDepth) {
		return backgroundColor;
	}

	// Find the nearest object
	HitResult hit;
	for (auto& object : allObjects) {
		if (object->Hit(ray, hit, epsilon)) {
			// If hit was successful (i.e. found a new tMin), store a reference to the object
			hit.hitObject = object;
		}
	}

	// Check if the ray actually hit anything
	if (hit.hitObject != nullptr) {
		// Calculate the hit's properties, now that we know that this hit is the closest one 
		// (this way, we only have to do these calculations once)

		// New world-space position from tMin
		hit.loc = ray.FindLocAtTime(hit.t);

		// During intersection checks, hit.nor is filled with local-space normal. Now, convert to world space
		hit.nor = hit.hitObject->GetInverseTranspose() * hit.nor;
		// Multiplying by inverse transpose makes the w component nonzero, so reset it here before normalizing
		hit.nor.w = 0.0;
		hit.nor = normalize(hit.nor);

		Material mat = hit.hitObject->GetMaterial();
		dvec3 color(0, 0, 0);

		// Emissive Color
		color += mat.ke;
		
		// Blinn-phong color (don't need to do this if the material is ~100% reflective)
		if (mat.reflective < (1.0 - reflectiveThreshold)) {
			// Calculate contribution from each light
			for (auto& light : allLights) {
				if (!IsPointInShadow(hit.loc, light)) {
					color += (1.0 - mat.reflective) * mat.ShadeBlinnPhong(ray, hit, light);
				}
			}
		}

		// Reflection ray contributions (don't do this if the material is ~0% reflective)
		if (mat.reflective > (0.0 + reflectiveThreshold)) {
			// Create a ray with the new reflection direction
			Ray3D reflectionRay(hit.loc, glm::reflect(ray.dir, hit.nor));
			color += mat.ks * mat.reflective * ComputeRayColor(reflectionRay, depth + 1);
		}

		// Find the ambient (global) illumination using just 1 extra ray (path tracing)
		// Full equation: ambient light = 1/N * sum from 1->N of ( 1/p * (f * L * cos(theta)))
		// Where f = BRDF = kd/pi (use perfect diffuse shading for this model,
		//     so albedo = kd https://computergraphics.stackexchange.com/questions/350/albedo-vs-diffuse)
		// L = incoming light, theta = angle btwn incoming (constant) and outgoing (randomized) light rays

		Ray3D ambientRay(hit.loc, dvec4(RandomRayInHemisphere(hit.nor), 0));
		// The random ray generation uses a cosine-weighted model, where p = cos(theta) / pi
		// Therefore, when dividing by p the cos(theta) would cancel out with the full eq so we don't mult by cos here
		dvec3 ambientGI = ComputeRayColor(ambientRay, depth + 1);

		// Since BRDF is a constant value for this model, just multiply it at the end
		const dvec3 BRDF = mat.kd / pi<double>();
		ambientGI *= BRDF;

		// p is a constant value for this model (after canceling out cos(theta)), so we can wait to divide by p until the end
		// Store 1/p to save a division op. Note that the cos(theta) term already canceled out earlier, so it's not included here
		constexpr double p = 1 / pi<double>();
		// TODO: optimize p to avoid the double division
		ambientGI /= p;

		color += ambientGI;

		// Make sure the color isn't clipping
		ClampVector(color, 0.0f, 1.0f);
		return color;
	}
	else return backgroundColor;
}

bool Scene::IsPointInShadow(dvec4& hitLoc, PointLight& light) const {
	HitResult shadowHit;
	// Shadow ray is located at the hit position, goes to the light
	Ray3D shadowRay(hitLoc, glm::normalize(light.pos - hitLoc));

	// Check to see if there are any objects between the hit location and the light
	for (auto& object : allObjects) {
		// Set tMin to epsilon to avoid self-shadowing, and set tMax to the light's distance
		if (object->Hit(shadowRay, shadowHit, epsilon, glm::length(light.pos - hitLoc))) {
			// If I hit anything, immediately return true, no further action required
			return true;
		}
	}

	/* NOTE: Another way to accomplish the tMax behavior in Hit() (i.e. don't count intersections past the light location) 
	could be to initialize shadowHit.t = <light distance>. The Hit function only returns true if the HitResult actually finds
	a new t value that is smaller than its current t value. This would reduce the complexity of the code a bit (no longer
	need to check for tMax in intersectLocal). But that's a bit harder to understand at a glance, so I decided to keep 
	the tMax parameter for the sake of readability */

	return false;
}

void Scene::BuildSceneFromFile(std::string filename, Camera& camera) {
	// File format:
	// Camera <Pos X/Y/Z> <Rot X/Y/Z> <FovY>
	// SceneObject <Subclass> <Name> <Pos X/Y/Z> <Rot X/Y/Z> <Scale X/Y/Z> <Kd R/G/B> <Ks R/G/B> <Ke R/G/B> <Reflectance> <Specular Exp>
	// Light <Name> <Pos X/Y/Z> <Intensity>

	// (X/Y/Z) means 3 space-separated entries for X, Y, and Z
	// Items can be in any order, rotations and fov in degrees
	// There should only be 1 entry for camera, but SceneObjects and Lights can be repeated as necessary

	cout << "Reading scene data from " << filename << " ... ";

	ifstream file(filename);
	if (!file.good()) {
		cerr << endl << "ERROR: Unable to find a description for the provided scene file: " << filename << endl;
		return;
	}

	char buf[1024];
	while (file.getline(buf, 1024)) {
		istringstream ss(buf);
		string objectType = ReadValue<string>(ss);
		if (objectType == "Camera") {
			camera.SetPosition(dvec4(ReadVec3(ss), 1));
			camera.SetRotationDegrees(ReadVec3(ss));
			camera.SetFOVDegrees(ReadValue<double>(ss));
			camera.Setup();
		}
		else if (objectType == "SceneObject") {
			string subclass = ReadValue<string>(ss);
			if (subclass == "Sphere") {
				allObjects.push_back(ReadObject<Sphere>(ss));
			}
			else if (subclass == "Plane") {
				allObjects.push_back(ReadObject<Plane>(ss));
			}
			else if (subclass == "TriangleMesh") {
				allObjects.push_back(ReadObject<TriangleMesh>(ss));
				dynamic_pointer_cast<TriangleMesh>(allObjects.back())->LoadMeshFile(ReadValue<string>(ss));
			}
		}
		else if (objectType == "Light") {
			string name = ReadValue<string>(ss);
			dvec4 pos = dvec4(ReadVec3(ss), 1);
			double intensity = ReadValue<double>(ss);
			allLights.push_back(PointLight(name, pos, intensity));
		}
		else if (objectType == "#") continue; // Do nothing for comment lines
	}
	cout << "done!" << endl;
}

void Scene::ClampVector(glm::dvec3& vec, double min, double max) {
	ClampDouble(vec.x, min, max);
	ClampDouble(vec.y, min, max);
	ClampDouble(vec.z, min, max);
}

void Scene::ClampDouble(double& num, double min, double max) {
	if (num < min) num = min;
	if (num > max) num = max;
}

glm::dvec3 Scene::RandomRayInHemisphere(glm::dvec4& normal)
{
	// Use a cosine-weighted point generation method from https://graphicscompendium.com/raytracing/19-monte-carlo
	// TODO: cite in readme
	
	// Basic idea: generate points uniformly on a disc, then project up onto the hemisphere
	double u = rand() / (float)RAND_MAX;
	double v = rand() / (float)RAND_MAX;
	double r = sqrt(u);
	double theta = 2.0 * glm::pi<double>() * v;

	// This ray assumes a normal of <0, 1, 0>. Next, transform using the actual normal
	dvec3 localDir(r * cos(theta), sqrt(1 - u), r * sin(theta));

	// Before rotating, check if the local normal and world normal are parallel
	dvec3 localUp(0, 1, 0);
	if (localUp == dvec3(normal)) return localDir;
	if (localUp == -1.0 * dvec3(normal)) return -1.0 * localDir;

	// dot(u, v) = cos(theta) for unit vectors
	double angle = acos(dot(localUp, dvec3(normal)));
	dvec3 axis = cross(localUp, dvec3(normal));

	return rotate(localDir, angle, axis);
}
