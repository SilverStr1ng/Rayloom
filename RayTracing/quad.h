#pragma once

#include "hittable.h"

class quad : public hittable {
public:
	quad(const point3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat)
		:Q{ Q }, u{ u }, v{ v }, mat{ mat } {
		auto n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);	
		/**
		* 对于任意点P，我们寻求两个标量 α 和 β，使得：
		* P = Q + α * u + β * v
		* 平面坐标 α 和 β 的范围是 [0, 1]，以确保点 P 位于四边形内，它们由以下方程给出：
		* α = w · （p × v）
		* β = w · （u × p）
		* 
		* 其中
		* p = P - Q
		* w = n / n·(u × v) = n / n · n
		*/

		w = n / dot(n, n);


		set_bounding_box();
	}

	virtual void set_bounding_box() {
		auto bbox_diagonal1{ aabb(Q, Q + u + v) };
		auto bbox_diagonal2{ aabb(Q + u, Q + v) };

		bbox = aabb(bbox_diagonal1, bbox_diagonal2);
	}

	aabb bounding_box() const override { return bbox; }

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		/**
		1. 求出包含四边形的平面
		2. 求解射线和包含四边形平面的交点
		3. 判断交点是否在四边形内

		平面的隐式公式： Ax + By + Cz + D = 0
		Ax + By + Cz = D

		平面法向量： N = (A, B, C)
		位置向量 v = (x, y, z)

		n · v = D

		对于射线 R(t) = P + td，代入平面方程：
		n · (P + td) = D

		t = (D - n · P) / (n · d)

		如果射线和平面平行，则 n · d = 0，表示没有交点。

		（A, B, C）是平面的法向量，可以通过四边形的两个边向量的叉积来计算：
		n = unit_vector(u × v)

		Q位于平面上，因此可以使用点Q来计算D：

		D = n · Q
		*/
		auto denom{ dot(normal, r.direction()) };

		if (std::fabs(denom) < 1e-8) {
			return false; // 射线和平面平行，没有交点
		}

		auto t { (D - dot(normal, r.origin())) / denom };
		if (!ray_t.contains(t)) {
			return false; // 交点不在射线的有效范围内
		}

		auto intersection{ r.at(t) };

		vec3 planar_hitpt_vector{ intersection - Q };
		auto alpha{ dot(w, cross(planar_hitpt_vector, v)) };
		auto beta{ dot(w, cross(u, planar_hitpt_vector)) };

		if (!is_interior(alpha, beta, rec)) {
			return false;
		}

		rec.t = t;
		rec.p = intersection;
		rec.mat = mat;
		rec.set_face_normal(r, normal);

		return true;
	}

	virtual bool is_interior(double a, double b, hit_record& rec) const {
		interval unit_interval{ interval(0,1) };

		if (!unit_interval.contains(a) || !unit_interval.contains(b)) {
			return false;
		}

		rec.u = a;
		rec.v = b;
		return true;
	}


private:
	point3 Q;
	vec3 u, v;
	vec3 w;
	std::shared_ptr<material> mat;
	aabb bbox;
	vec3 normal;
	double D;

};