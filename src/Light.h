#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <iostream>
#include "SceneObject.h"

class Light {
public:
	Light() = default;
	Light(std::string _name, double _L, double _Q, double _falloffDistance) :
		name(_name),
		L(_L),
		Q(_Q),
		distance(_falloffDistance),
		sqrdDist(std::pow(distance, 2.0)) {}

	virtual glm::dvec3 GetColor() = 0;
	virtual glm::dvec4 GetLocation(double& pdf) = 0;
	// If this light is attached to a sceneobject (i.e. emissive lights), return it. Else, return nullptr
	virtual std::shared_ptr<SceneObject> GetObject() = 0;
	double GetAttenuation(double r) {
		double linear = distance / (distance + L * r);
		double quad = sqrdDist / (sqrdDist + Q * std::pow(r, 2.0));
		return linear * quad;
	}
	std::string name;

private:
	// Use the blender model of light attenuation: A = (dist / (dist + L * r)) * (dist^2 / (dist^2 + Q * r^2))
	// https://docs.blender.org/manual/en/2.79/render/blender_render/lighting/lights/attenuation.html
	// Linear attenuation factor
	double L = 0.5;
	// Quadratic (physical) attenuation factor
	double Q = 1;
	// Falloff distance factor
	double distance = 100;

	// Store the squared values of some parameters to avoid unecessary calculations later
	double sqrdDist = 0;

	// L=0, Q=0 - No attenuation
	// L=0, Q=1 - physics
};