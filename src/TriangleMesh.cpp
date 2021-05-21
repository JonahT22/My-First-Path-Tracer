#pragma once
#include "TriangleMesh.h"

using namespace std;
using namespace glm;

bool TriangleMesh::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Run a collision check against the bounding sphere, immediately exit if it did not hit
	// junkHit = temporary variable required for storing the output from the sphere intersection, discarded once hit is done
	HitResult junkHit;
	if (!boundingSphere->Hit(ray, junkHit)) {
		return false;
	}

	// Since we need to check over ALL tri's (don't just return immediately when the 1st hit is found), we'll use this temp variable
	bool foundNewHit = false;
	// Iterate over each triangle in the list, running its intersection function
	// Get the t value from the intersection function, and check if its within the bounds/smaller than hit's minT
	for (auto& tri : allTriangles) {
		// t = distance to ray
		// u, v = barycentric coords corresponding to vert1, vert2
		double t, u, v;
		if (tri->IntersectTriangle(ray, t, u, v)) {
			if (tMin < t && t < tMax) {
				if (outHit.UpdateTMin(t)) {
					// If the new t is valid, and it is less than the current tmin...
					double coords[3] = { (1.0 - u - v), u, v };
					outHit.nor = tri->BaryInterpNorm(coords);
					foundNewHit = true;
				}
			}
		}
	}

	// return false if:
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated)
	return foundNewHit;
}

glm::dvec4 TriangleMesh::GetRandomPointOnSurface(double& pdf, dvec4& normal)
{
	return transf.translation;
}

void TriangleMesh::LoadMeshFile(std::string filename) {
	//LOAD GEOMETRY
	//v Starter Code begins here v
	vector<float> posBuf; // list of vertex positions
	vector<float> norBuf; // list of vertex normals
	vector<float> texBuf; // list of vertex texture coords
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, filename.c_str());
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuf.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuf.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuf.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	//^ Starter code ends here ^

	// Find the bounds of the model
	dvec2 xbound(posBuf.at(0), posBuf.at(0));
	dvec2 ybound(posBuf.at(1), posBuf.at(1));
	dvec2 zbound(posBuf.at(2), posBuf.at(2));
	for (int i = 0; i < posBuf.size(); i += 3) {
		xbound = dvec2(std::min(posBuf.at(i + 0), (float)xbound.x), std::max(posBuf.at(i + 0), (float)xbound.y));
		ybound = dvec2(std::min(posBuf.at(i + 1), (float)ybound.x), std::max(posBuf.at(i + 1), (float)ybound.y));
		zbound = dvec2(std::min(posBuf.at(i + 2), (float)zbound.x), std::max(posBuf.at(i + 2), (float)zbound.y));
	}
	dvec4 middlePoint((xbound.x + xbound.y) / 2.0, (ybound.x + ybound.y) / 2.0, (zbound.x + zbound.y) / 2.0, 1);

	// Make another trip through the posbuf, finding the furthest vertex from the middle
	double radius = 0;
	for (int i = 0; i < posBuf.size(); i += 3) {
		radius = std::max(radius, length(middlePoint - dvec4(posBuf.at(i + 0), posBuf.at(i + 1), posBuf.at(i + 2), 1)));
	}
	// Create a bounding sphere - hard-coded to wrap tightly around the bunny mesh
	objFile = filename;
	boundingSphere = make_unique<Sphere>(
		"Bounding_Sphere",
		Transform(middlePoint, dvec3(0, 0, 0), dvec3(radius, radius, radius)),
		make_shared<Material>(dvec3(0, 0, 0), dvec3(0, 0, 0), dvec3(0, 0, 0), 1, 0, 1)
		);

	// Read through the pos buf, creating the triangles
	for (int i = 0; i < posBuf.size(); i += 9) {
		shared_ptr<Triangle> newTri = make_shared<Triangle>(); //create a new triangle

		//Set the new triangle's vertex positions
		newTri->SetLocations(
			dvec4(posBuf.at(i + 0), posBuf.at(i + 1), posBuf.at(i + 2), 1),
			dvec4(posBuf.at(i + 3), posBuf.at(i + 4), posBuf.at(i + 5), 1),
			dvec4(posBuf.at(i + 6), posBuf.at(i + 7), posBuf.at(i + 8), 1)
		);
		newTri->SetNorms(
			dvec4(norBuf.at(i + 0), norBuf.at(i + 1), norBuf.at(i + 2), 0),
			dvec4(norBuf.at(i + 3), norBuf.at(i + 4), norBuf.at(i + 5), 0),
			dvec4(norBuf.at(i + 6), norBuf.at(i + 7), norBuf.at(i + 8), 0)
		);
		allTriangles.push_back(newTri);
	}
}
