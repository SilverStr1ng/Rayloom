#pragma once

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"


#include <algorithm>

class bvh_node : public hittable {
public:
	bvh_node(hittable_list list)
		: bvh_node(list.objects, 0, list.objects.size()) {
		// 这里有一个C++的微妙之处。这个构造函数（不带span索引）会创建hittable列表的隐式副本，我们将会修改这个副本。被复制列表的生命周期只持续到这个构造函数退出为止。这没问题，因为我们只需要持久化生成的包围体层次结构。
	}

	bvh_node(std::vector<std::shared_ptr<hittable>>& objects, size_t start, size_t end) {
		// 构建源对象跨度的边界框
		bbox = aabb::empty;

		for (size_t object_index = start; object_index < end; object_index++) {
			bbox = aabb(bbox, objects[object_index]->bounding_box());
		}

		int axis{ bbox.longest_axis() };
		auto comparator = (axis == 0) ? box_x_compare
			: (axis == 1) ? box_y_compare
			: box_z_compare;


		size_t object_span = end - start;

		if (object_span == 1) {
			left = right = objects[start];
		}
		else if (object_span == 2) {
			if (comparator(objects[start], objects[start + 1])) {
				left = objects[start];
				right = objects[start + 1];
			}
			else {
				left = objects[start + 1];
				right = objects[start];
			}
		}
		else {
			std::sort(objects.begin() + start, objects.begin() + end, comparator);
			auto mid = start + object_span / 2;
			left = std::make_shared<bvh_node>(objects, start, mid);
			right = std::make_shared<bvh_node>(objects, mid, end);
		}

		bbox = aabb(left->bounding_box(), right->bounding_box());
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		if (!bbox.hit(r, ray_t)) return false;

		bool hit_left{ left->hit(r, ray_t, rec) };
		bool hit_right{ right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec) };

		return hit_left || hit_right;
	}

	aabb bounding_box() const override {
		return bbox;
	}

private:
	std::shared_ptr<hittable> left;
	std::shared_ptr<hittable> right;
	aabb bbox;

	static bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index) {
		auto a_axis_interval{ a->bounding_box().axis_interval(axis_index) };
		auto b_axis_interval{ b->bounding_box().axis_interval(axis_index) };
		return a_axis_interval.min < b_axis_interval.min;
	}

	static bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
		return box_compare(a, b, 0);
	}

	static bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
		return box_compare(a, b, 1);
	}

	static bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
		return box_compare(a, b, 2);
	}
};