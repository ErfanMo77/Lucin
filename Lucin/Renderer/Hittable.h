#pragma once
#ifndef HITTABLE_H
#define HITTABLE_H

#include "Renderer/ray.h"
#include "math/vec3.h"

struct hitRecord
{
	point3 p;
	vec3 normal;
	double t;
};

class hittable
{
public:
	virtual void hit(const ray&, double t_min, double t_max, hitRecord& rec) const = 0;
};

#endif
