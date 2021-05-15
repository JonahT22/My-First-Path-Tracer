#pragma once
#include "Scene.h"

using namespace glm;
using namespace std;
using json = nlohmann::json;

// Main render loop
glm::dvec3 Scene::ComputeRayColor(Ray3D& ray, int depth, bool specularRay) {
	// Before anything else, make sure I haven't passed the recursion depth
	if (depth > maxRecursionDepth) {
		return backgroundColor;
	}

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
		// Only add this on the first bounce, or if this ray was created from a specular bounce
		// For diffuse rays (created from the global illumination stuff below), this prevents double-dipping the light 
		// (i.e. if I'm already calculating contributions from each light in the "Blinn-phong color" loop, and all emissive materials 
		// are treated as a light, then I don't want to double-dip my sample by also getting the emissive color from my bounce rays)
		// For reflective rays (created when the mat's reflectance is > 0), I don't actually sample all the lights before sending out 
		// the bounce rays. Because of this, I still want to get the emissive color of anything that I hit (otherwise, lights
		// will appear black in mirrors)
		if (depth == 0 || specularRay) {
			color += mat.ke;
		}
		
		// Reflection ray contributions (don't do this if the material is ~0% reflective)
		if (mat.reflective > (0.0 + reflectiveThreshold)) {
			// Create a ray with the new reflection direction
			Ray3D reflectionRay(hit.loc, glm::reflect(ray.dir, hit.nor));
			// Send a new reflection ray, and indicate that the ray was created from a mirror reflection
			color += mat.ks * mat.reflective * ComputeRayColor(reflectionRay, depth + 1, true);
		}
		
		// Blinn-phong color (don't need to do this if the material is ~100% reflective)
		if (mat.reflective < (1.0 - reflectiveThreshold)) {
			// Calculate contribution from each light
			for (auto& light : allLights) {
				if (!IsPointInShadow(hit.loc, light->GetLocation(), light->GetObject())) {
					color += (1.0 - mat.reflective) * mat.ShadeBlinnPhong(ray, hit, light);
				}
			}
		}

		// GLOBAL ILLUMINATION

		// Full equation: ambient light = 1/N * sum from 1->N of ( 1/p * (f * L * cos(theta)))
		// Where f = BRDF = kd/pi (use perfect diffuse shading for this model,
		//     so albedo = kd https://computergraphics.stackexchange.com/questions/350/albedo-vs-diffuse
		// L = incoming light, theta = angle btwn incoming (constant) and outgoing (randomized) light rays
		// Using path tracing, so only sending out a single ray

		Ray3D ambientRay(hit.loc, dvec4(RandomRayInHemisphere(hit.nor), 0));
		// The random ray generation uses a cosine-weighted model, where PDF = cos(theta) / pi
		// Therefore, when dividing by p the cos(theta) would cancel out with the full eq so we don't mult by cos here
		dvec3 ambientGI = ComputeRayColor(ambientRay, depth + 1);
		// Constant (lambertian) BRDF
		const dvec3 BRDF = mat.kd / pi<double>();
		ambientGI *= BRDF;
		// Store 1/p to save a division op. Note that the cos(theta) term already canceled out earlier, so it's not included here
		constexpr double pRecip = pi<double>();
		// Divide by p
		ambientGI *= pRecip;
		color += ambientGI;

		// Make sure the color isn't clipping
		ClampVector(color, 0.0f, 1.0f);
		return color;
	}
	else return backgroundColor;
}

bool Scene::IsPointInShadow(dvec4& hitLoc, dvec4& lightLoc, std::shared_ptr<SceneObject> lightObj) const {
	HitResult shadowHit;
	// Shadow ray is located at the hit position, goes to the light
	Ray3D shadowRay(hitLoc, glm::normalize(lightLoc - hitLoc));

	// Check to see if there are any objects between the hit location and the light
	for (auto& object : allObjects) {
		// First, make sure this object isn't the object that belongs to the light that we're testing
		// (we don't want to collide with the light source itself)
		if (object != lightObj) {
			// Set tMin to epsilon to avoid self-shadowing, and set tMax to the light's distance
			// TODO: maybe precompute glm::length(lightLoc - hitLoc)?
			if (object->Hit(shadowRay, shadowHit, epsilon, glm::length(lightLoc - hitLoc))) {
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

void Scene::BuildSceneFromFile(std::string filename, Camera& camera) {
	std::cout << "Reading scene data from " << filename << " ... " << endl;

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
			else if (objectType == "TriangleMesh") {
				shared_ptr<TriangleMesh> newMesh = ReadObject<TriangleMesh>(object);
				newMesh->LoadMeshFile(object.at("MeshFile").get<string>());
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
			}
		}
	}
	catch (json::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
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

Material Scene::ReadMaterial(const json& j) {
	dvec3 ke(0, 0, 0);
	// The emissive color is not included for non-emissive objects, so check before reading emissive color
	if (j.at("IsEmissive").get<bool>()) {
		ke = ReadVec3(j.at("EmissiveProperties").at("EmissiveColor"));
	}
	return Material(
		ReadVec3(j.at("DiffuseColor")),
		ReadVec3(j.at("SpecularColor")),
		ke,
		j.at("Reflectance"),
		j.at("SpecularExp")
	);
}
