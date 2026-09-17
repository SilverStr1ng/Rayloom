#include "vec3.h"
#include "color.h"
#include "ray.h"

#include <iostream>

static double hit_sphere(const point3& center, double radius, const ray& r)
{
	vec3 oc = center - r.origin();

	// 将射线方程 P(t) = Q + td 代入球面方程 |P-C|² = R²，
	// 得到关于 t 的一元二次方程：
	//
	// (d·d)t² - 2[d·(C-Q)]t + (C-Q)·(C-Q) - R² = 0
	//
	// 因此：
	// a = d·d
	// b = -2[d·(C-Q)]
	// c = (C-Q)·(C-Q) - R²

	auto a{ dot(r.direction(), r.direction()) };
	auto b{ -2.0 * dot(r.direction(), oc) };
	auto c{ dot(oc, oc) - radius * radius };

	auto discriminant{ b * b - 4 * a * c };

	if (discriminant < 0) {
		return -1.0;
	}
	else {
		// 使用求根公式，取较小的根，即通常离射线原点更近的交点
		return (-b - std::sqrt(discriminant)) / (2.0 * a);
	}
}

static color ray_color(const ray& r) {
	auto t{ hit_sphere(point3(0, 0, -1), 0.5, r) };
	if (t > 0.0) {
		vec3 N{ unit_vector(r.at(t) - vec3(0,0,-1)) };
		// 单位法线 N 的每个分量范围为：
		//
		//     [-1, 1]
		//
		// 颜色 RGB 希望位于：
		//
		//     [0, 1]
		//
		// 所以做映射：
		//
		//     [-1,1] → [0,1]
		//
		// 即：
		//
		//     x → 0.5 * (x + 1)
		//
		// 这里直接对三个分量做同样的事情，
		// 因此球表面的法线方向会显示成不同颜色。
		return 0.5 * color(
			N.x() + 1,
			N.y() + 1,
			N.z() + 1
		);
	}


	vec3 unit_direction = unit_vector(r.direction());

	auto a = 0.5 * (unit_direction.y() + 1.0);

	return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
	//  设置宽高比
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;

	// 计算图像高度，并且确保它至少大于1
	int image_height = static_cast<int>(image_width / aspect_ratio);
	image_height = image_height > 1 ? image_height : 1;

	// 相机配置

	auto focal_length = 1.0;

	// 视口宽度可以小于1，因为他们是实数
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (static_cast<double>(image_width) / static_cast<double>(image_height));

	// 相机原点
	auto camera_center = point3(0, 0, 0);

	// 计算横跨视口的水平和垂直向量
	auto viewport_u = vec3(viewport_width, 0, 0);
	auto viewport_v = vec3(0, -viewport_height, 0);

	// 计算每个像素对应的变化的delta_x和delta_y
	auto pixel_delta_u = viewport_u / image_width;
	auto pixel_delta_v = viewport_v / image_height;

	// 计算左上角坐标的位置
	auto viewport_upper_left = camera_center
		- vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	/**
	*                  y
	*             ↑
	*
	*    (-2,1)         (2,1)
	*       ┌─────────────┐
	*       │             │
	*       │      +      │  ← 视口中心 (0,0,-1)
	*       │             │
	*       └─────────────┘
	*    (-2,-1)        (2,-1)
	*
	*             ↑
	*          camera
	*         (0,0,0)
	*/


	// 渲染
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	for (auto j{ 0 }; j < image_height; ++j) {
		std::clog << "\rScanlines ramaining: " << (image_height - j) << ' ' << std::flush;
		for (auto i{ 0 }; i < image_width; ++i) {
			auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			auto ray_direction = pixel_center - camera_center;

			ray r(camera_center, ray_direction);

			color pixel_color = ray_color(r);

			write_color(std::cout, pixel_color);
		}
	}







	return 0;
}