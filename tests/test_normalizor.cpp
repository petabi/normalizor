#include <cstdio>
#include <fstream>
#include <istream>
#include <ostream>
#include <time.h>

#include <gtest/gtest.h>

#include "normalizor.hpp"

static void build_log_file(const std::string& fname, size_t total_lines) {
  std::filebuf fb;
  fb.open(fname, std::ios_base::app | std::ios_base::out);
  std::ostream out(&fb);
  std::string le = " This is my log entry ";
  time_t mytime;
  struct tm* mytinfo;
  char formatted_time_buffer[64];
  for (size_t i = 0; i < total_lines; ++i) {
    mytime = time(&mytime);
    mytinfo = localtime(&mytime);
    std::strftime(formatted_time_buffer, 64, "%m/%d/%Y %H:%M:%S", mytinfo);
    std::string myline(formatted_time_buffer);
    myline.append(le + std::to_string(i) + "\n");
    out.write(myline.c_str(), static_cast<std::streamsize>(myline.size()));
  }
  out.flush();
}

TEST (test_basic_normalization, test_normalize_lines) {
  std::string my_log_file = "my_test.log";
  size_t total_lines = 10000;
  build_log_file(my_log_file, total_lines);
  Line_normalizer norm;
  std::filebuf fb;
  fb.open(my_log_file, std::ios_base::in);
  std::istream in(&fb);
  auto lines = norm.normalize(in);
  EXPECT_EQ(lines.size(), total_lines);
  remove(my_log_file.c_str());
}
