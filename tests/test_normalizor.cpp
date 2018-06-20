#include <cstdio>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
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

TEST (test_basic_normaliztion, test_sections) {
  std::string my_line = "12/31/1999 12:59:59 an ip 4.56.789.0 a"
                        ";base64,0A1B a hex \\x0b and a vn v1.2_3"
                        " a num 123 lala\n";
  Line_normalizer norm;
  std::istringstream in(my_line);
  auto lines = norm.normalize(in);
  EXPECT_EQ(lines.front().sections.size(), 6);
  unsigned int sec_id = 1;
  auto sec_it = lines.front().sections.begin();
  for (; sec_it != lines.front().sections.end(); ++sec_id, ++sec_it) {
    EXPECT_EQ(sec_id, sec_it->second.first);
  }
}
