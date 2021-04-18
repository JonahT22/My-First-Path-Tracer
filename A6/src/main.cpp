#include <iostream>
#include <string>
#include <limits>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

using namespace std;
using namespace glm;

struct Transform {
	vec4 loc = vec4(0, 0, 0, 1);
	vec3 rot = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);
	Transform() = default;
	Transform(vec4 _loc, vec3 _rot, vec3 _scale)
		: loc(_loc), rot(_rot), scale(_scale) {}
};

struct Ray3D {
	vec4 start;
	vec4 dir;
	Ray3D(vec4 _start, vec4 _dir) : start(_start), dir(_dir) {}
};

struct HitResult {
	// Initilialize as largest possible double value
	double tMin = numeric_limits<double>::max();
	vec4 loc;
	vec4 nor;
	vec3 color;
	// TODO: Material properties
};

struct Camera {
	vec4 pos = vec4(0, 0, 0, 1);
	double fovY = 90;
	double aspect = 1.0;
	Camera(vec4 _pos, double _fov, int width, int height)
		: pos(_pos), fovY(_fov), aspect(width / (double)height) {}
};

class SceneObject {
public:
	SceneObject() = default;

	// Calcualte the transformation matrix for this object
	void CreateTransformMtx(Transform _transf) {
		// Since all objects in this project are static and independent, the MatrixStack class is not required
		// (We can just calculate the transformations once, no need for hierarchies or dynamic transf calculations)
		transform = _transf;
		transMtx *= glm::translate(glm::mat4(1.0f), vec3(_transf.loc));
		transMtx *= glm::eulerAngleXYZ(_transf.rot.x, _transf.rot.y, _transf.rot.z);
		transMtx *= glm::scale(glm::mat4(1.0f), _transf.scale);
	}

	bool Hit(Ray3D ray, HitResult& outHit) {
		// Apply transformations to the ray to change it to local space
		mat4 invMtx = inverse(transMtx);
		Ray3D localRay(invMtx * ray.start, invMtx * ray.dir);
		// Check intersection (NOTE: ray direction is NOT normalized)
		return IntersectLocal(localRay, outHit);
	}

	// TODO: add epsilon to this intersection function, and discard any t values below it
	virtual bool IntersectLocal(Ray3D ray, HitResult& outHit) = 0;
	
	// TODO: add another function for finding shadow intersections. Makes a new ray
	// tracing loop over all objects, but initializes the hit result's t to the distance to the light,
	// and stops after the first new tmin is found

private:
	// The transformation matrix to convert this object from local->world space
	glm::mat4 transMtx = glm::mat4(1.0f);
	// TODO: is this variable necessary after finding transMtx?
	Transform transform;
};

class Sphere : public SceneObject {
public:
	Sphere() = default;

	bool IntersectLocal(Ray3D ray, HitResult& outHit) override {
		// Assume sphere is at origin with radius 1
		double a =       dot(vec3(ray.dir),   vec3(ray.dir));
		double b = 2.0 * dot(vec3(ray.dir),   vec3(ray.start));
		double c =       dot(vec3(ray.start), vec3(ray.start)) - 1.0;
		double d2 = pow(b, 2) - (4 * a * c);
		if (d2 > 0) {
			double t1 = (-1.0 * b + sqrt(d2)) / (2.0 * a);
			double t2 = (-1.0 * b - sqrt(d2)) / (2.0 * a);
			if (t1 > 0 && t1 < t2) {
				//cout << "Hit at time " << t1 << endl;
				return true;
			}
			else if (t2 > 0 && t2 < t1) {
				//cout << "Hit at time " << t1 << endl;
				return true;
			}
			else return false;
		}
		else if (d2 == 0) {
			double t = -1.0 * b / (2.0 * a);
			//cout << "Hit at time " << t << endl;
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
		vec4(0, 0, 5, 1),
		45.0 * pi<double>() / 180.0, 
		width, height);
	
	// Hard-code a sphere into the middle of the scene
	shared_ptr<SceneObject> testSphere = make_shared<Sphere>();
	testSphere->CreateTransformMtx(Transform(
		vec4(0, 0, 0, 1),
		vec3(0, 0, 0.75),
		vec3(0.5, 1, 1)));
	
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
			Ray3D newRay = Ray3D(camera.pos, vec4(u * camera.aspect, v, -1.0 * camDist, 0.0f));
			newRay.dir = normalize(newRay.dir);
			HitResult hit;
			if (testSphere->Hit(newRay, hit)) {
				//cout << "Hit at point (" << hit.loc.x << ", " << hit.loc.y << ", " << hit.loc.z << ")" << endl;
				outputImage->setPixel(col, row, 255, 0, 0);
			}
		}
	}
	outputImage->writeToFile(fileName);
	
	return 0;
}
