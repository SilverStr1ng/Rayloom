#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// 常量
constexpr double infinity{ std::numeric_limits<double>::infinity() };
constexpr double pi{ 3.1415926535897932385 };

// 工具函数
inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

// Common Headers

#include "color.h"
#include "ray.h"
#include "vec3.h"