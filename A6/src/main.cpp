#include <iostream>
#include <string>
#include <limits>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"
#include "MatrixStack.h"

using namespace std;
using namespace glm;

struct Transform {
	vec3 location = vec3(0, 0, 0);
	vec3 rotation = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);
};

struct Ray3D {
	vec3 start;
	vec3 dir;
	Ray3D(vec3 _start, vec3 _dir) : start(_start), dir(_dir) {}
};

struct HitResult {
	// Initilialize as largest possible double value
	double tMin = numeric_limits<double>::max();
	vec3 loc;
	vec3 nor;
	vec3 color;
	// TODO: Material properties
};

struct Camera {
	vec3 pos = vec3(0, 0, 0);
	double fovY = 90;
	double aspect = 1.0;
	Camera(vec3 _pos, double _fov, int width, int height)
		: pos(_pos), fovY(_fov), aspect(width / (double)height) {}
};

class SceneObject3D {
public:
	SceneObject3D() = default;

	// TODO: add epsilon to this intersection function, and discard any t values below it
	virtual bool Intersect(Ray3D ray, HitResult& outHit) = 0;
	// TODO: add another function for finding shadow intersections. Makes a new ray
	// tracing loop over all objects, but initializes the hit result's t to the distance to the light,
	// and stops after the first new tmin is found

private:
	Transform transform;
};

class Sphere : public SceneObject3D {
public:
	Sphere() = default;

	bool Intersect(Ray3D ray, HitResult& outHit) override {
		// Assume ray direction is normalized, and sphere is at origin with radius 1
		// a = 1
		double b = 2.0 * dot(ray.dir, ray.start);
		double c = dot(ray.start, ray.start) - 1.0;
		double d2 = pow(b, 2) - (4 * c);
		if (d2 > 0) {
			double t1 = (-1.0 * b + sqrt(d2)) / 2.0;
			double t2 = (-1.0 * b - sqrt(d2)) / 2.0;
			if (t1 > 0 && t1 < t2) {
				cout << "Hit at time " << t1 << endl;
				return true;
			}
			else if (t2 > 0 && t2 < t1) {
				cout << "Hit at time " << t1 << endl;
				return true;
			}
			else return false;
		}
		else if (d2 == 0) {
			double t = -1.0 * b / 2.0;
			cout << "Hit at time " << t << endl;
			return true;
		}
		else return false;
	}
};

int main(int argc, char **argv)
{
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
		vec3(0, 0, 5),
		45.0 * pi<double>() / 180.0, 
		width, height);
	
	// Hard-code a sphere into the middle of the scene
	shared_ptr<SceneObject3D> testSphere = make_shared<Sphere>();

	// Find distance to image plane so that it goes from -1 to 1 in the y direction
	double camDist = 1 / tan(camera.fovY / 2.0);
	//imageSize -= 1;
	// Generate rays from the camera to the center of each pixel
	for (int row = 0; row < height; row++) {
		// u and v are in normalized image coords, -1 to 1
		double v = (2.0 * ((double)row + 0.5) / (double)height) - 1.0;
		for (int col = 0; col < width; col++) {
			double u = (2.0 * ((double)col + 0.5) / (double)width) - 1.0;
			//cout << "u: " << u << endl;
			//cout << "v: " << v << endl;
			Ray3D newRay = Ray3D(vec3(camera.pos), vec3(u * camera.aspect, v, -1.0 * camDist));
			newRay.dir = normalize(newRay.dir);
			//cout << newRay.dir.x << endl << newRay.dir.y << endl << newRay.dir.z << endl << endl;
			HitResult hit;
			if (testSphere->Intersect(newRay, hit)) {
				//cout << "Hit at point (" << hit.loc.x << ", " << hit.loc.y << ", " << hit.loc.z << ")" << endl;
				outputImage->setPixel(col, row, 255, 0, 0);
			}
		}
	}
	outputImage->writeToFile(fileName);
	
	return 0;
}
