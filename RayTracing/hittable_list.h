#pragma once

#include "hittable.h"

#include <memory>
#include <vector>
#include <utility>

class hittable_list : public hittable {
public:
	std::vector<std::shared_ptr<hittable>> objects{};

	hittable_list() = default;
	explicit hittable_list(std::shared_ptr<hittable> object) {
		add(std::move(object));
	}

	void clear() {
		objects.clear();
	}

	void add(std::shared_ptr<hittable> object) {
		objects.push_back(std::move(object));
	}

	bool hit(const ray& r, interval r_t, hit_record& rec) const override {
		hit_record temp_rec{};
		bool hit_anything{ false };

		auto closest_so_far{ r_t.max };

		for (const auto& object : objects) {
			if (object->hit(r, interval(r_t.min, closest_so_far), temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}
};