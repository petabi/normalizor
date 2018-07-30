#include <ctime>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "normalizor.h"

static void build_log_file(const std::string& fname, size_t total_lines)
{
  std::filebuf fb;
  fb.open(fname, std::ios_base::out);
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
}

TEST(test_basic_normalization, test_normalize_lines)
{
  std::string my_log_file = "my_test.log";
  size_t total_lines = 10000;
  build_log_file(my_log_file, total_lines);
  Line_normalizer norm;
  norm.set_input_stream(my_log_file);
  auto lines = norm.normalize();
  EXPECT_EQ(lines.size(), total_lines);
  remove(my_log_file.c_str());
}

TEST(test_basic_normaliztion, test_sections)
{
  std::string my_line = "12/31/1999 12:59:59 an ip 4.56.789.0 a"
                        ";base64,0A1B a hex \\x0b and a vn v1.2_3"
                        " a num 123 lala\n";
  Line_normalizer norm;
  std::istringstream in(my_line);
  norm.set_input_stream(in);
  auto lines = norm.normalize();
  EXPECT_EQ(lines.front().sections.size(), 21);
  std::vector<int> sec_ids = {1, 7, 7, 7, 2, 7, 3, 7, 7, 7, 6,
                              7, 7, 7, 7, 5, 7, 7, 7, 6, 7};
  auto sec_it = lines.front().sections.begin();
  auto sec_id = sec_ids.begin();
  EXPECT_EQ(lines.front().line, my_line);
  for (; sec_it != lines.front().sections.end(); ++sec_id, ++sec_it) {
    EXPECT_EQ(*sec_id, sec_it->second.first);
  }
}

TEST(test_basic_normaliztion, test_sections2)
{
  std::string my_log_file = "my_test.log";
  size_t total_lines = 10;
  build_log_file(my_log_file, total_lines);
  Line_normalizer norm;
  norm.set_input_stream(my_log_file);
  auto lines = norm.normalize();
  EXPECT_EQ(lines.size(), 10);
  for (const auto& nl : lines) {
    EXPECT_EQ(nl.sections.size(), 8);
  }
}

TEST(test_basic_normalization, test_non_ascii)
{
  std::string my_line("lala ησε lala ×ÀÃæ lala πεμας lala\n");
  Line_normalizer norm;
  std::istringstream in(my_line);
  norm.set_input_stream(in);
  auto lines = norm.normalize();
  EXPECT_FALSE(lines.empty());
  EXPECT_EQ(lines.front().line, my_line);
  EXPECT_EQ(lines.front().sections.size(), 3);
  for (const auto& s : lines.front().sections) {
    EXPECT_EQ(s.second.first, 7);
    auto substr = my_line.substr(s.first, s.second.second - s.first);
    for (const auto& c : substr) {
      EXPECT_GT(static_cast<unsigned int>(c), 30);
    }
  }
}

TEST(test_basic_normalization, test_py_normalizor)
{
  std::string my_log_file = "my_test.log";
  size_t total_lines = 10000;
  std::string my_py_norm = DATA_DIR "/test_py_normalizor.py";
  std::string my_cmd =
      "python3 " + my_py_norm + " " + LIB_DIR + " " + my_log_file;
  build_log_file(my_log_file, total_lines);
  int my_status_code = std::system(my_cmd.c_str());
  EXPECT_EQ(my_status_code, 0);
  remove(my_log_file.c_str());
}
