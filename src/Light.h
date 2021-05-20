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
	
	// If this is an area/emissive light, store a random point on the light's surface, and return the pdf
	virtual double RandomizeLocation() = 0;
	// Get the light's loc, or the most recently chosen random position on an area light
	virtual glm::dvec4 GetLocation() = 0;
	// Find this light's color contribution, given the most recently-sampled loc on the light and a point in the world
	virtual glm::dvec3 SampleLight(glm::dvec4 hitLocation) = 0;
	// If this light is attached to a sceneobject (i.e. emissive lights), return it. Else, return nullptr
	virtual std::shared_ptr<SceneObject> GetObject() = 0;
	virtual glm::dvec3 GetColor() = 0;
	std::string name;

protected:
	// Use the blender model of light attenuation
	double GetDistanceAttenuation(double r) {
		double linear = distance / (distance + L * r);
		double quad = sqrdDist / (sqrdDist + Q * std::pow(r, 2.0));
		return linear * quad;
	}

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