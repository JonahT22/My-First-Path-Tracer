// TODO: credit triangle intersection code in README
// TODO: credit hemisphere point generation in README
#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <iomanip>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"

#include "Camera.h"
#include "HitResult.h"
#include "Ray3D.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Sphere.h"

using namespace std;
using namespace glm;

double FindSecondsSince(chrono::time_point<chrono::steady_clock> startTime) {
	auto stopTime = chrono::high_resolution_clock::now();
	return chrono::duration_cast<chrono::milliseconds>(stopTime - startTime).count() / 1000.0;
}

int main(int argc, char **argv) {
	auto startTime = chrono::high_resolution_clock::now();
	if(argc < 4) {
		cout << "Usage: ./my-first-pathtracer <SCENE NAME> <IMAGE SIZE> <NUM SAMPLES> <IMAGE FILENAME>" << endl;
		return 0;
	}
	string sceneName(argv[1]);
	int height = atoi(argv[2]);
	int width = height;
	int numSamples = atoi(argv[3]);
	string fileName(argv[4]);
	shared_ptr<Image> outputImage = make_shared<Image>(width, height);

	// Provide image dimensions to camera for aspect ratio & ray calculations
	Camera camera (width, height, dvec4(0, 0, -5, 1), dvec3(0, 0, 0), 45, 0.4);

	// Build a scene with a black background color
	Scene scene(dvec3(0, 0, 0));
	scene.BuildSceneFromFile("../resources/" + sceneName + ".json", camera);
	
	srand(time(NULL));

	// Used for counting percentage completion
	int prevPercent = 0;

	// Iterate over every pixel
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			// Iterate multiple times over each pixel for path tracing
			dvec3 rayColor(0, 0, 0);
			for (int i = 0; i < numSamples; i++) {
				// Generate random ray directions within the current pixel (for antialiasing)
				Ray3D newRay = camera.CreateCameraRay(row, col);
				// Iterate over every item in the scene to find the intersection/color of the ray
				rayColor += scene.ComputeRayColor(newRay);
			}
			rayColor /= (double)numSamples;

			// Image processing
			camera.ApplyExposure(rayColor);
			camera.ApplyTonemapping(rayColor, Camera::Tonemapper::ACES_APPROX);
			camera.ColorLinearToSRGB(rayColor);

			// Store color value
			outputImage->setPixel(col, row, 255 * rayColor.r, 255 * rayColor.g, 255 * rayColor.b);

			// Print a status update (to the nearest 1%)
			int percent = std::floor(100 * (row / (float)height));
			if (percent > prevPercent) {
				prevPercent = percent;
				double estRem = (FindSecondsSince(startTime) / (double)percent) * (100 - percent);
				cout << setw(2) << percent << "%, est: " << estRem << " s" << endl;
			}
		}
	}

	double duration = FindSecondsSince(startTime);
	cout << "Completed in " << duration << " s" << endl;

	outputImage->writeToFile(fileName);
	//outputImage->writeToFile(fileName + to_string(duration) + ".png");

	return 0;
}
