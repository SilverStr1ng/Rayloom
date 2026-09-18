#include "rtweekend.h"


#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

static color ray_color(const ray& r, const hittable& world) {
	hit_record rec{};

	if (world.hit(r, 0, infinity, rec)) {
		return 0.5 * (rec.normal + color(1, 1, 1));
	}

	vec3 unit_direction{ unit_vector(r.direction()) };

	auto a{ 0.5 * (unit_direction.y() + 1.0) };

	return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
	//  设置宽高比
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;

	// 计算图像高度，并且确保它至少大于1
	int image_height = static_cast<int>(image_width / aspect_ratio);
	image_height = image_height > 1 ? image_height : 1;

	// world
	hittable_list world{};

	world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100));

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

			color pixel_color = ray_color(r,world);

			write_color(std::cout, pixel_color);
		}
	}







	return 0;
}