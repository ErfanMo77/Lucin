#include <iostream>

int main() {

	const int imgWidth = 256;
	const int imgHeight = 256;

	std::cout << "p3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int j = imgHeight-1; j>=0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i=0; i<imgWidth; i++)
		{
			auto r = double(i) / (imgWidth-1);
			auto g = double(j) / (imgHeight-1);
			auto b = 0.3;

			int ir = static_cast<int>(r * 255.999);
			int ig = static_cast<int>(g * 255.999);
			int ib = static_cast<int>(b * 255.999);

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}
	std::cerr << "\nDone.\n";

	return 0;
}