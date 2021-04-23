#include <iostream>
#include <string>
#include <limits>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

#include "Camera.h"
#include "HelperStructs.h"
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
	
	// Hard-code a sphere into the middle of the scene
	shared_ptr<SceneObject> testSphere = make_shared<Sphere>();
	testSphere->CreateTransformMtx(Transform(
		vec4(0, 0, 0, 1),
		vec3(0, 0, 0.75),
		vec3(0.5, 1, 1)));
	
	// Generate rays from the camera to the center of each pixel
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			Ray3D newRay = camera.CreateCameraRay(row, col);
			HitResult hit;
			if (testSphere->Hit(newRay, hit)) {
				//cout << "Hit at point (" << hit.loc.x << ", " << hit.loc.y << ", " << hit.loc.z << ")" << endl;
				outputImage->setPixel(col, row, 0, 255, 0);
			}
		}
	}
	outputImage->writeToFile(fileName);
	
	return 0;
}
