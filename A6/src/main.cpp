#include <iostream>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

using namespace std;
using namespace glm;

struct Ray3D {
	vec3 start;
	vec3 dir;
	Ray3D(vec3 _start, vec3 _dir) : start(_start), dir(_dir) {}
};

int main(int argc, char **argv)
{
	if(argc < 4) {
		cout << "Usage: ./A6 <SCENENUMBER> <IMAGE SIZE> <IMAGE FILENAME>" << endl;
		return 0;
	}
	int sceneNum = atoi(argv[1]);
	int imageSize = atoi(argv[2]);
	string fileName(argv[3]);
	shared_ptr<Image> outputImage = make_shared<Image>(imageSize, imageSize);

	vec3 camPos = vec3(0, 0, 5);
	constexpr double camFOVy = 45.0 * pi<double>() / 180.0;
	// aspect ratio, which gets multiplied by the X value (since our fov is in respect to the y direction)
	double camAspect = 1.0f;
	
	// Hard-code a sphere into the middle of the scene

	// Find distance to image plane so that it goes from -1 to 1 in the y direction
	double camDist = 1 / tan(camFOVy / 2.0);
	//imageSize -= 1;
	// Generate rays from the camera to the center of each pixel
	for (int row = 1; row < 2 * imageSize; row += 2) {
		// u and v are in normalized image coords, -1 to 1
		double v = (row / (double)imageSize) - 1.0;
		for (int col = 1; col < 2 * imageSize; col += 2) {
			double u = (col / (double)imageSize) - 1.0;
			Ray3D newRay = Ray3D(vec3(camPos), vec3(u * camAspect, v, -1.0 * camDist));
			newRay.dir = normalize(newRay.dir);
		}
	}
	
	return 0;
}
