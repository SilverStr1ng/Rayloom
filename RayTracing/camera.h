#pragma once

#include "hittable.h"
#include "material.h"

class camera {
public:
	//  设置宽高比
	double aspect_ratio{ 1.0 };
	int image_width{ 100 };
	int samples_per_pixel{ 10 };
	int max_depth{ 10 };

	double vfov{ 90 };

	point3 lookfrom{ point3(0,0,0) };
	point3 lookat{ point3(0,0,-1) };
	vec3 vup{ vec3(0,1,0) };

	double defocus_angle{ 0 };  // 每个像素光线的变化角度
	double focus_dist{ 10 };    // 从相机lookfrom点到完美对焦平面的距离


	void render(const hittable& world) {
		initialize();
		// 渲染
		std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

		for (auto j{ 0 }; j < image_height; ++j) {
			std::clog << "\rScanlines ramaining: " << (image_height - j) << ' ' << std::flush;
			for (auto i{ 0 }; i < image_width; ++i) {
				color pixel_color(0, 0, 0);
				for (int sample{ 0 }; sample < samples_per_pixel; ++sample) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}
				write_color(std::cout, pixel_samples_scale * pixel_color);
			}
		}

		std::clog << "\rDone.			\n";
	}

private:
	int image_height;
	double pixel_samples_scale;
	point3 center;
	point3 pixel00_loc;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;
	vec3 u, v, w;
	vec3 defocus_disk_u;
	vec3 defocus_disk_v;


	void initialize() {
		// 计算图像高度，并且确保它至少大于1
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = image_height > 1 ? image_height : 1;

		pixel_samples_scale = 1.0 / samples_per_pixel;

		center = lookfrom;


		// 相机配置

		//auto focal_length = (lookfrom - lookat).length();

		auto theta{ degrees_to_radians(vfov) };
		// h = tan(theta/2)是不言自明的
		auto h{ std::tan(theta / 2) };

		// 视口宽度可以小于1，因为他们是实数
		auto viewport_height = 2 * h * focus_dist;
		auto viewport_width = viewport_height * (static_cast<double>(image_width) / static_cast<double>(image_height));

		// 计算相机坐标系的单位正交基
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// 相机原点
		auto camera_center = point3(0, 0, 0);

		// 计算横跨视口的水平和垂直向量
		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_width * -v;

		// 计算每个像素对应的变化的delta_x和delta_y
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// 计算左上角坐标的位置
		auto viewport_upper_left = center
			- (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

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

		// 计算相机散焦圆盘基向量
		auto defocus_radius{ focus_dist * std::tan(degrees_to_radians(defocus_angle / 2)) };
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

	}

	color ray_color(const ray& r, int depth, const hittable& world) const {
		if (depth <= 0)
			return color(0, 0, 0);
		hit_record rec;

		if (world.hit(r, interval(0.001, infinity), rec)) {
			ray scattered{};
			color attenuation{};
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, depth - 1, world);

			return color(0, 0, 0);
		}

		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);

		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}

	ray get_ray(int i, int j) const {
		// 构造一条源自散焦圆盘并指向像素位置 i, j 周围随机采样点的相机光线。
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

		auto ray_origin = (defocus_angle < 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	point3 defocus_disk_sample() const {
		auto p{ random_in_unit_disk() };
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	vec3 sample_square() const {
		return vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}
};