// TODO: add 'const' to functions when applicable, pass vec4's  by reference when possible
// TODO: check bracket placement everywhere

#include <iostream>
#include <string>
#include <limits>
#include <memory>

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

// TODO: remove
void PrintMat4(dmat4 mat) {
	cout << mat[3][3] << " " << mat[3][2] << " " << mat[3][1] << " " << mat[3][0] << endl;
	cout << mat[2][3] << " " << mat[2][2] << " " << mat[2][1] << " " << mat[2][0] << endl;
	cout << mat[1][3] << " " << mat[1][2] << " " << mat[1][1] << " " << mat[1][0] << endl;
	cout << mat[0][3] << " " << mat[0][2] << " " << mat[0][1] << " " << mat[0][0] << endl;
}

void PrintVec4(dvec4 vec) {
	cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << endl;
}

int main(int argc, char **argv)
{
	// TODO: make background color an editable property 

	if(argc < 4) {
		cout << "Usage: ./A6 <SCENENUMBER> <IMAGE SIZE> <IMAGE FILENAME>" << endl;
		return 0;
	}
	// TODO: maybe remove this?
	int sceneNum = atoi(argv[1]);
	int height = atoi(argv[2]);
	int width = height;
	string fileName(argv[3]);
	shared_ptr<Image> outputImage = make_shared<Image>(width, height);

	Camera camera (width, height);

	Scene scene;
	scene.BuildSceneFromFile("null");
	
	// Generate rays from the camera to the center of each pixel
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			Ray3D newRay = camera.CreateCameraRay(row, col);
			dvec3 rayColor = scene.ComputeRayColor(newRay);
			outputImage->setPixel(col, row, 255 * rayColor.r, 255 * rayColor.g, 255 * rayColor.b);
		}
	}
	outputImage->writeToFile(fileName);
	
	return 0;
}
