#include "vec3.h"
#include "color.h"
#include "ray.h"

#include <iostream>

int main() {
	//  设置宽高比
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;

	// 计算图像高度，并且确保它至少大于1
	int image_height = static_cast<int>(image_width / aspect_ratio);
	image_height = image_height > 1 ? image_height : 1;

	// 相机
	auto focal_length = 1.0;

	// 视口宽度可以小于1，因为他们是实数
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (static_cast<double>(image_width) / static_cast<double>(image_height));

	// 原点
	auto camera_center = point3(0, 0, 0);

	// 计算横跨视口的水平和垂直向量
	auto viewport_u = vec3(viewport_width, 0, 0);
	auto viewport_v = vec3(0, viewport_height, 0);

	// 计算每个像素对应的变化的delta_x和delta_y
	auto pixel_delta_x = viewport_u / image_width;
	auto pixel_delta_y = viewport_v / image_height;







	return 0;
}