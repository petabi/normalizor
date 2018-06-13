
#include <cstring>
#include <fstream>
#include <istream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "normalizor.hpp"

int main(int, char*[])
{
  Line_normalizer norm;
  std::filebuf fb;
  std::vector<Normal_line> lines;
  if (fb.open("access-1024.log", std::ios_base::in)) {
    std::istream in(&fb);
    lines = norm.normalize(in);
  }
  printf("lines: ");
  for (const auto& l : lines) {
    printf("  l: %s\n", l.line.c_str());
  }
}
