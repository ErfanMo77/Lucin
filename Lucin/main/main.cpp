#include <iostream>

#include <chrono>
#include <thread>

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "Renderer/sphere.h"
#include "Renderer/camera.h"
#include "Renderer/hittable_list.h"
#include "Renderer/material.h"


//Global variables
unsigned char* data;
const auto aspect_ratio = 3.0 / 2.0;
const int image_width = 1200;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int maxDepth = 50;

point3 lookfrom(13, 2, 3);
point3 lookat(0, 0, 0);
vec3 vup(0, 1, 0);
auto dist_to_focus = 10.0;
auto aperture = 0.1;
camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

//threads
int threadsize = 40;

color ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec; 
	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void write_color(color& col, int samples_per_pixel) {
	auto r = col.x();
	auto g = col.y();
	auto b = col.z();

	// Divide the color by the number of samples and gamma-correct for gamma=2.0.
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	col[0] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    col[1] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    col[2] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
}

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

void render(int lineIndex, int samples_per_pixel, hittable_list world) {
	int stride = image_height / threadsize;
	int index = (lineIndex)*image_width*3;
	auto startTime = std::chrono::steady_clock::now();
	for (int j = image_height-lineIndex; j > (image_height-lineIndex) - stride && j!=0; j--) {
		//std::cerr << "Scanlines remaining: " << j << ' ';
		if (j == image_height - lineIndex - (stride / 2)) {
			std::cout << "I'm half way there! \"thread number " << lineIndex / stride << "\"" << std::endl;
		}
		for (int i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / (int)(image_width - 1);
				auto v = (j + random_double()) / (int)(image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, maxDepth);
			}

			write_color(pixel_color, samples_per_pixel);
			data[index++] = static_cast<char>(pixel_color.x());
			data[index++] = static_cast<char>(pixel_color.y());
			data[index++] = static_cast<char>(pixel_color.z());
			//count++;


			//	mean += delta;
			//	auto avg = (mean + delta).count() / count * 100;
			//	std::cerr << "\rEstimated remaining time: " << avg * (image_height * image_width - count) / 1000000000 << ' ';
			//}
		}
	}
	auto deltaTime = std::chrono::steady_clock::now();
	//if (count % 100 == 0) {
	auto delta = std::chrono::duration_cast<std::chrono::seconds>(deltaTime - startTime);
	std::cout << "\nLine number: " << (image_height - lineIndex) << " to number: " << (image_height - lineIndex) - stride << " finished! elapsed time = " << delta.count() <<std::endl;
	std::cout << " Thread number " << lineIndex/stride << " joined!\n";
}


int main() {

	//timer
	auto start = std::chrono::steady_clock::now();
	


	data = new unsigned char[image_width * image_height * 3];
	const int samples_per_pixel = 25;


	// World
	auto world = random_scene();

	std::vector<std::thread> threads;
	for (int i = 0; i < threadsize; i++) {
		threads.push_back(std::thread(render,i*(image_height/threadsize),samples_per_pixel, world));
	}

	for(auto& thread:threads)
	{
		thread.join();
	}

	int index = 0;
	int count = 0;
	std::chrono::nanoseconds delta(0);
	std::chrono::nanoseconds mean(0);
	

	//stbi_write_png("../image.png", image_width, image_height, 3, data, image_width * sizeof(unsigned char)*3);
	stbi_write_tga("../image.tga", image_width, image_height, 3, data);
	//stbi_write_jpg("../image.jpg", image_width, image_height, 3, data, 100);
	std::cerr << "\nDone.\n";

	auto end = std::chrono::steady_clock::now();

	std::cerr << "Elapsed time in seconds : "
		<< std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
		<< " s" ;

}