#pragma once
#include "Scene.h"

using namespace glm;
using namespace std;
using json = nlohmann::json;

// Main render loop
glm::dvec3 Scene::ComputeRayColor(Ray3D& ray) {
	dvec3 outputColor(0.0);
	// Stores the filtered color of each surface as we bounce off of them (i.e. bounce of a red surface, throughput is now 1, 0, 0)
	dvec3 throughput(1.0);
	// This variable is true on the first loop so that initial hits on emissive objects return the proper color
	bool specularBounce = true;
	
	for (int i = 0; i < maxBounces; i++) {
		// Find the nearest object
		HitResult hit; // default tMin = infinity
		for (auto& object : allObjects) {
			if (object->Hit(ray, hit, epsilon)) {
				// If hit was successful (i.e. found a new tMin), store a reference to the object
				hit.hitObject = object;
			}
		}

		// Check if the ray actually hit anything
		if (hit.hitObject != nullptr) {
			// Calculate the hit's properties
			// New world-space position from tMin
			hit.loc = ray.FindLocAtTime(hit.t);
			// During intersection checks, hit.nor is filled with local-space normal. Now, convert to world space
			hit.nor = hit.hitObject->GetInverseTranspose() * hit.nor;
			// Multiplying by inverse transpose makes the w component nonzero, so reset it here before normalizing
			hit.nor.w = 0.0;
			hit.nor = normalize(hit.nor);

			shared_ptr<Material> mat = hit.hitObject->GetMaterial();

			// Emissive Color
			// Only add this on the first bounce, or if this ray was created from a specular bounce
			// Necessary since we're using explicit light sampling
			// (See https://computergraphics.stackexchange.com/questions/5152/progressive-path-tracing-with-explicit-light-sampling/5153#5153)
			if (specularBounce) {
				outputColor += throughput * mat->ke;
			}

			// Randomly choose between specular and diffuse rays, depending on the material's reflectance
			if ((rand() / (double)RAND_MAX) < mat->reflectance) {
				// Glossy reflection (glossiness - based on 'roughness' value)
				throughput = throughput * mat->ks;
				// Next ray is a reflection ray
				specularBounce = true;
				// Create a new ray with the reflection direction
				ray = Ray3D(hit.loc, GetReflectionRay(ray.dir, hit.nor, 0.5));
			}
			else {
				// Non-specular Lighting = Direct Lighting + Ambient Lighting

				// Direct Lighting
				// Explicitly sample each light
				for (auto& light : allLights) {
					double pdf;
					// Since light location is randomized for emissive objects, need to precompute it and pass to the next 2 functions
					dvec4 lightLoc = light->GetLocation(pdf);
					if (!IsPointInShadow(hit.loc, lightLoc, light->GetObject())) {
						outputColor += throughput * mat->ShadeDiffuse(ray, hit, light, lightLoc) / pdf;
					}
				}

				// Ambient Lighting = 1/N * sum from 1->N of ( 1/p * (f * L * cos(theta)))
				// Where f = BRDF = kd/pi (use perfect diffuse shading for this model,
				//     so albedo = kd https://computergraphics.stackexchange.com/questions/350/albedo-vs-diffuse
				// L = incoming light, theta = angle btwn incoming (constant) and outgoing (randomized) light rays

				// Next ray hit will be a diffuse bounce
				ray = Ray3D(hit.loc, GetRandomRayInHemisphere(hit.nor));
				specularBounce = false;

				// Attenuate further rays by BRDF / PDF
				// Constant (lambertian) BRDF. Usually albedo / pi, but the pi cancels out w/pdf
				const dvec3 BRDF = mat->kd;
				// Store 1/p to save some operations
				// Proper PDF is cos(theta) / pi, but both the cos and the pi cancel out with the BRDF / lighting equation
				constexpr double invPdf = 1.0;
				throughput *= BRDF * invPdf;
				// NOTE: normally, throughput would be multiplied by cos(theta), where theta is angle btwn new and old rays
				// The random ray generation uses a cosine-weighted model, where PDF = cos(theta) / pi
				// Therefore, when dividing by p the cos(theta) would cancel out with the full eq so we don't mult by it here
			}
		}
		else {
			// Hit the background, so add the bg color and stop bouncing
			outputColor += throughput * backgroundColor;
			return outputColor;
		}
	}
	return outputColor;
}

bool Scene::IsPointInShadow(dvec4& hitLoc, dvec4& lightLoc, std::shared_ptr<SceneObject> lightObj) const {
	HitResult shadowHit;
	// Shadow ray is located at the hit position, goes to the light
	Ray3D shadowRay(hitLoc, glm::normalize(lightLoc - hitLoc));
	// Maximum distance that shadow rays should travel
	double lightDist = glm::length(lightLoc - hitLoc);

	// Check to see if there are any objects between the hit location and the light
	for (auto& object : allObjects) {
		// First, make sure this object isn't the object that belongs to the light that we're testing
		// (we don't want to collide with the light source itself)
		if (object != lightObj) {
			// Set tMin to epsilon to avoid self-shadowing, and set tMax to the light's distance
			if (object->Hit(shadowRay, shadowHit, epsilon, lightDist)) {
				// If I hit anything, immediately return true, no further action required
				return true;
			}
		}
	}

	/* NOTE: Another way to accomplish the tMax behavior in Hit() (i.e. don't count intersections past the light location) 
	could be to initialize shadowHit.t = <light distance>. The Hit function only returns true if the HitResult actually finds
	a new t value that is smaller than its current t value. This would reduce the complexity of the code a bit (no longer
	need to check for tMax in intersectLocal). But that's a bit harder to understand at a glance, so I decided to keep 
	the tMax parameter for the sake of readability */

	return false;
}



glm::dvec4 Scene::GetRandomRayInHemisphere(glm::dvec4& normal)
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
	if (localUp == dvec3(normal)) return dvec4(localDir, 0);
	if (localUp == -1.0 * dvec3(normal)) return dvec4(-1.0 * localDir, 0);

	// dot(u, v) = cos(theta) for unit vectors
	double angle = acos(dot(localUp, dvec3(normal)));
	dvec3 axis = cross(localUp, dvec3(normal));

	return dvec4(rotate(localDir, angle, axis), 0);
}

glm::dvec4 Scene::GetReflectionRay(glm::dvec4& rayDir, glm::dvec4& hitNor, double roughness)
{
	// Find the ideal specular reflection direction
	dvec4 idealDir = glm::reflect(rayDir, hitNor);
	// Find a random, uniform reflection direction
	dvec4 diffuseDir = GetRandomRayInHemisphere(hitNor);
	// Since the random generation function is cosine-weighted, apply a scaling factor

	// 
	return idealDir;
}

void Scene::BuildSceneFromFile(std::string filename, Camera& camera) {
	std::cout << "Reading scene data from " << filename << " ... ";

	ifstream file(filename);
	if (!file.good()) {
		cerr << endl << "ERROR: Unable to find a description for the provided scene file: " << filename << endl;
		return;
	}

	try {
		// Read the json object from the file
		json j;
		file >> j;

		// Set up the Camera
		json cam = j.at("Camera");
		camera.SetPosition(dvec4(ReadVec3(cam.at("Transform").at("Translation")), 1));
		camera.SetRotationDegrees(ReadVec3(cam.at("Transform").at("Rotation")));
		camera.SetFOVDegrees(cam.at("Fov").get<double>());
		camera.Setup();

		// Loop through the lights in the file
		for (auto& light : j.at("Lights")) {
			string lightType = light.at("LightType").get<string>();
			
			if (lightType == "PointLight") {
				allLights.push_back(make_shared<PointLight>(
					light.at("Name").get<string>(),
					light.at("Linear").get<double>(),
					light.at("Quadratic").get<double>(),
					light.at("FalloffDistance").get<double>(),
					dvec4(ReadVec3(light.at("Translation")), 1),
					ReadVec3(light.at("Color"))
					)
				);
			}
		}

		// Loop through the sceneobjects in the file
		for (auto& object : j.at("SceneObjects")) {
			// Construct Object based on its subclass
			string objectType = object.at("ObjectType").get<string>();
			
			if (objectType == "Sphere") {
				allObjects.push_back(ReadObject<Sphere>(object));
			}
			else if (objectType == "Plane") {
				allObjects.push_back(ReadObject<Plane>(object));
			}
			else if (objectType == "Square") {
				allObjects.push_back(ReadObject<Square>(object));
			}
			else if (objectType == "Box") {
				allObjects.push_back(ReadObject<Box>(object));
			}
			else if (objectType == "TriangleMesh") {
				shared_ptr<TriangleMesh> newMesh = ReadObject<TriangleMesh>(object);
				newMesh->LoadMeshFile(object.at("FileName").get<string>());
				allObjects.push_back(newMesh);
			}
			// If object is emissive, also construct a light
			if (object.at("Material").at("IsEmissive").get<bool>()) {
				// Get a reference to the object that was just added
				shared_ptr<SceneObject> topObj = allObjects.back();
				// Extract the emissive light's properties
				json lightData = object.at("Material").at("EmissiveProperties");
				allLights.push_back(make_shared<EmissiveLight>(
					topObj->name + "_EmissiveLight",
					lightData.at("Linear").get<double>(),
					lightData.at("Quadratic").get<double>(),
					lightData.at("FalloffDistance").get<double>(),
					topObj)
				);
				// Send a warning message if the object doesn't have a way to generate random points on its surface
				if (!topObj->hasRandomPointMethodDefined) {
					cerr << endl << "WARNING: emissive color defined for SceneObject \"" << topObj->name;
					cerr << "\", but no random point generation method defined. Using the object origin instead." << endl;
				}
			}
		}
	}
	catch (json::exception& e) {
		cerr << endl << "ERROR: " << e.what() << endl;
		return;
	}
	std::cout << "done!" << endl;
}

glm::dvec3 Scene::ReadVec3(const json& j) {
	return dvec3(
		j.at(0).get<double>(),
		j.at(1).get<double>(),
		j.at(2).get<double>()
	);
}

Transform Scene::ReadTransform(const json& j) {
	return Transform(
		dvec4(ReadVec3(j.at("Translation")), 1),
		ReadVec3(j.at("Rotation")),
		ReadVec3(j.at("Scale"))
	);
}

shared_ptr<Material> Scene::ReadMaterial(const json& j) {
	dvec3 ke(0, 0, 0);
	// The emissive color is not included for non-emissive objects, so check before reading emissive color
	if (j.at("IsEmissive").get<bool>()) {
		ke = Camera::ColorSRGBToLinear(ReadVec3(j.at("EmissiveProperties").at("EmissiveColor")));
	}
	return make_shared<Material>(
		Camera::ColorSRGBToLinear(ReadVec3(j.at("DiffuseColor"))),
		Camera::ColorSRGBToLinear(ReadVec3(j.at("SpecularColor"))),
		ke,
		j.at("Reflectance"),
		j.at("SpecularExp")
	);
}
