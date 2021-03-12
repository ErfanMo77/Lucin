#include <iostream>
#include "vec3.h"
#include "color.h"

int main() {

	const int imgWidth = 256;
	const int imgHeight = 256;


	std::cout << "p3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int j = imgHeight - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < imgWidth; ++i) {
			color pixel_color(double(i) / (imgWidth - 1), double(j) / (imgHeight - 1), 0.25);
			write_color(std::cout, pixel_color);
		}
	}
	std::cerr << "\nDone.\n";

	return 0;
}