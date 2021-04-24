#include <iostream>
#include <string>
#include <limits>
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

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

int main(int argc, char **argv)
{
	// TODO: make background color an editable property 

	if(argc < 4) {
		cout << "Usage: ./A6 <SCENENUMBER> <IMAGE SIZE> <IMAGE FILENAME>" << endl;
		return 0;
	}
	int sceneNum = atoi(argv[1]);
	int height = atoi(argv[2]);
	int width = height;
	string fileName(argv[3]);
	shared_ptr<Image> outputImage = make_shared<Image>(width, height);

	Camera camera (
		vec4(0, 0, 5, 1),
		45.0 * pi<double>() / 180.0, 
		width, height);

	Scene scene;
	scene.BuildSceneFromFile("null");
	
	// Generate rays from the camera to the center of each pixel
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			Ray3D newRay = camera.CreateCameraRay(row, col);
			vec3 rayColor = scene.ComputeRayColor(newRay);
			outputImage->setPixel(col, row, 255 * rayColor.r, 255 * rayColor.g, 255 * rayColor.b);
		}
	}
	outputImage->writeToFile(fileName);
	
	return 0;
}
