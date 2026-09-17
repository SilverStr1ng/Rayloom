// RayTracing.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "vec3.h"
#include "color.h"

#include <iostream>


void ppm()
{
	 // Image
	int image_width{256};
	int image_height{256};

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j{ 0 }; j < image_height; ++j) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i{ 0 }; i < image_width; ++i) {
			auto pixel_color = color(static_cast<double>(i) / (image_width - 1), static_cast<double>(j) / (image_height - 1), 0.0);
			write_color(std::cout, pixel_color);
		}
	}

	std::clog << "\rDone.                 \n";
}
