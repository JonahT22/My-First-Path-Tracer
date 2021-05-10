// TODO: credit triangle intersection code in README
// TODO: credit hemisphere point generation in README
#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <chrono>

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

int main(int argc, char **argv) {
	auto startTime = chrono::high_resolution_clock::now();
	if(argc < 4) {
		cout << "Usage: ./A6 <SCENENUMBER> <IMAGE SIZE> <IMAGE FILENAME>" << endl;
		return 0;
	}
	int sceneNum = atoi(argv[1]);
	int height = atoi(argv[2]);
	int width = height;
	string fileName(argv[3]);
	shared_ptr<Image> outputImage = make_shared<Image>(width, height);

	// Provide image dimensions to camera for aspect ratio & ray calculations
	Camera camera (width, height);

	// Build a scene with a black background color
	Scene scene(dvec3(0, 0, 0));
	scene.BuildSceneFromFile("../resources/scene" + string(argv[1]) + ".txt", camera);
	
	srand(time(NULL));

	// Number of samples per pixel for path tracing
	const int numSamples = 10;
	int prevPercent = 0;

	// Iterate over every pixel
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			// Generate rays from the camera to the center of the current pixel
			Ray3D newRay = camera.CreateCameraRay(row, col);
			// Iterate multiple times over each pixel for path tracing
			dvec3 rayColor(0, 0, 0);
			for (int i = 0; i < numSamples; i++) {
				// Iterate over every item in the scene to find the intersection/color of the ray
				rayColor += scene.ComputeRayColor(newRay);
			}
			rayColor /= (double)numSamples;

			// Store color value
			outputImage->setPixel(col, row, 255 * rayColor.r, 255 * rayColor.g, 255 * rayColor.b);

			// Print a status update (to the nearest 10%)
			int percent = std::floor(100 * (row / (float)height));
			if (percent > prevPercent) {
				prevPercent = percent;
				cout << percent << "%" << endl;
			}
		}
	}
	outputImage->writeToFile(fileName);

	auto stopTime = chrono::high_resolution_clock::now();
	cout << "Completed in " << chrono::duration_cast<chrono::milliseconds>(stopTime - startTime).count() << " ms" << endl;

	return 0;
}
