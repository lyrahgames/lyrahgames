#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb_image.h"
}
//
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
//
#include <fmt/core.h>
//
#include <lyrahgames/delaunay/delaunay.hpp>

struct accum {
  float color[4]{};
  unsigned int count{};
};

int main(int argc, char** argv) {
  using namespace std;
  using namespace lyrahgames;
  using fmt::format;

  if (3 != argc) {
    cout  //
        << "This program generates a tessellation of a given raster graphic "
           "by\n"
           "using the Delaunay triangulation and outputs the resulting SVG "
           "image.\n"
           "image.\n\n"
        << "usage:\n"
        << argv[0] << " <input image> <output svg file>\n";
    return -1;
  }

  // Load the given image.
  int image_width, image_height, image_channels;
  // stbi_set_flip_vertically_on_load(true);
  unsigned char* image_data =
      stbi_load(argv[1], &image_width, &image_height, &image_channels, 0);
  if (!image_data) throw runtime_error("Could not load the given image!");
  cout << "resolution = " << image_width << " x " << image_height << " x "
       << image_channels << "\n";

  // Open the output file.
  fstream svg_file{argv[2], ios::out};
  if (!svg_file) throw runtime_error("Could not open given output file!");

  // Generate random points.
  mt19937 rng{random_device{}()};
  uniform_real_distribution<float> dist{0, 1};
  size_t samples = 1000;
  vector<delaunay::point> points(samples);
  points[0] = delaunay::point(0, 0);
  points[1] = delaunay::point(image_width, 0);
  points[2] = delaunay::point(image_width, image_height);
  points[3] = delaunay::point(0, image_height);
  for (size_t i = 4; i < points.size(); ++i)
    points[i] =
        delaunay::point(image_width * dist(rng), image_height * dist(rng));

  // Construct Delaunay triangulation.
  delaunay::triangulation triangulation{};
  const auto elements = triangulation.triangle_data(points);

  // Compute the mean color of every triangle.
  // Very slow implementation!
  vector<accum> accum_buffer(elements.size() / 3);
  for (int i = 0; i < image_height; ++i) {
    for (int j = 0; j < image_width; ++j) {
      const auto x = j + 0.5f;
      const auto y = i + 0.5f;

      size_t index = 0;
      for (size_t k = 0; k < elements.size(); k += 3, ++index) {
        if (delaunay::intersection(
                {{points[elements[k + 0]], points[elements[k + 1]],
                  points[elements[k + 2]]}},
                {x, y})) {
          break;
        }
      }
      if (index == elements.size() / 3) continue;

      accum_buffer[index].count += 1;
      for (int k = 0; k < image_channels; ++k) {
        accum_buffer[index].color[k] +=
            float(image_data[image_channels * (i * image_width + j) + k]);
      }
    }
  }

  // Generate SVG output.
  svg_file << format(                      //
      "<svg height=\"{}\" width=\"{}\">",  //
      image_height, image_width);
  size_t index = 0;
  for (size_t i = 0; i < elements.size(); i += 3, ++index) {
    svg_file << format(
        "<polygon "
        "points=\"{0},{1} {2},{3} {4},{5}\" "
        "style=\""
        "fill:rgb({6},{7},{8});"
        "stroke:rgb({6},{7},{8});"
        "stroke-width:0.5"
        "\" "
        "/>",
        points[elements[i + 0]].x, points[elements[i + 0]].y,  //
        points[elements[i + 1]].x, points[elements[i + 1]].y,  //
        points[elements[i + 2]].x, points[elements[i + 2]].y,  //
        accum_buffer[index].color[0] / accum_buffer[index].count,
        accum_buffer[index].color[1] / accum_buffer[index].count,
        accum_buffer[index].color[2] / accum_buffer[index].count);
  }
  svg_file << "</svg>\n" << flush;

  stbi_image_free(image_data);
}