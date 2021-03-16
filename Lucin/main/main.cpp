#include <iostream>

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "Renderer/sphere.h"
#include "Renderer/Utility.h"
#include "Renderer/color.h"
#include "Renderer/camera.h"
#include "Renderer/hittable_list.h"

double hit_sphere(const point3& center, double radius, const ray& r) {
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;

	if (discriminant < 0) {
		return -1.0;
	}
	else {
		return (-half_b - sqrt(discriminant)) / a;
	}
}

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void write_color(color& col, int samples_per_pixel) {
	auto r = col.x();
	auto g = col.y();
	auto b = col.z();

	// Divide the color by the number of samples.
	auto scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;

	col[0] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    col[1] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    col[2] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
}


int main() {

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1920;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    unsigned char* data = new unsigned char[image_width * image_height * 3];
    const int samples_per_pixel = 8;

	// World
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -110, -2), 100));

    // Camera
	camera cam;

    int index = 0;
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);

			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world);
			}

            write_color(pixel_color,samples_per_pixel);
            data[index++] = static_cast<unsigned char>(pixel_color.x());
            data[index++] = static_cast<unsigned char>(pixel_color.y());
            data[index++] = static_cast<unsigned char>(pixel_color.z());
        }
    }
    stbi_write_jpg("../image.jpg", image_width, image_height, 3, data, 100);

}