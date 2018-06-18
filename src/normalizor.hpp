#ifndef NORMALIZOR_H
#define NORMALIZOR_H

#include <cstring>
#include <istream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <hs/hs_compile.h>
#include <hs/hs_runtime.h>

struct Normal_type {
  Normal_type(std::string re, unsigned int f, std::string rep) : regex(re), flags(f),
              replacement(rep) {}
  std::string regex;
  size_t flags;
  std::string replacement;
};

struct Normal_line {
  Normal_line(std::string l,
              std::map<size_t, std::pair<int, size_t>> secs) : line(l),
              sections(secs) {}
  std::string line;
  std::map<size_t, std::pair<int, size_t>> sections;
};

struct Line_context {
  Line_context() : block(nullptr), last_boundary(0) {}
  Line_context(const char* b) : block(b) {}
  const char* block;
  size_t last_boundary;
  std::map<size_t, std::pair<int, size_t>> cur_sections;
  std::vector<Normal_line> parsed_lines;
};

class Line_normalizer {
public:
  Line_normalizer() {}
  void clear_context() {
    context.block = nullptr;
    context.last_boundary = 0;
    context.cur_sections.clear();
    context.parsed_lines.clear();
  }
  std::vector<struct Normal_line> normalize(std::istream& in);

private:
  bool build_hs_database();
  size_t read_block(std::istream& in);
  static int on_match(unsigned int id, unsigned long long start,
                      unsigned long long to, unsigned int, void* ctx);
  static constexpr size_t blocksize = 65536;
  char block[blocksize] = {0};
  struct Line_context context;
  std::unique_ptr<hs_database_t, decltype(hs_free_database)*> hs_db{nullptr, &hs_free_database};
  const std::map<size_t, const struct Normal_type> normal_types = {
    {0, Normal_type(R"(\n)", 0u, "<NL>")},
    {1, Normal_type(R"((((\d{1,2}|\d{4})[-\/\s](\d{1,2}|jan|feb|mar|)"
                    R"(apr|may|jun|jul|aug|sep|oct|nov|dec)[-\/\s](\d{4}|)"
                    R"(\d{1,2})|((jan(uary)?|feb(uary)?|mar(ch)?|apr(il)?|)"
                    R"(may|jun(e)?|jul(y)?|aug(ust)?|sep(tember)?|)"
                    R"(oct(ober)?|nov(ember)?|dec(ember)?)\s\s?\d{1,2})"
                    R"((\s\d{4})?))([\s:]\d{2}[:]\d{2}[:]\d{2})"
                    R"((\s(am|pm|[+-](\d{3,4}|\d{2}[:]\d{2})))?)?))",
                    HS_FLAG_CASELESS, "<TS>")},
    {2, Normal_type(R"(\d{1,3}[-.]\d{1,3}[-.]\d{1,3}[-.]\d{1,3}(\/\d{1,2})?)",
                    0u, "<IP>")},
    {3, Normal_type(R"(;base64,([0-9A-Za-z+/]{4}|[0-9A-Za-z+/]{3}=|)"
                    R"([0-9A-Za-z+/]{2}==)+)", 0u, "<B64>")},
    {4, Normal_type(R"((\x5c{1,2}x[0-9A-Fa-f]{2}|%[0-9A-Fa-f]{2}|)"
                    R"([\x7f-\xff])+)", 0u, "<HEX>")},
    {5, Normal_type(R"([v/]?\d{1,3}[._]\d{1,3}([._]\d{1,3})?\b)", 0u, "<VN>")},
    {6, Normal_type(R"(\d+(\.\d+)?)", 0u, "<DEC>")}
  };
  std::unique_ptr<hs_scratch_t, decltype(hs_free_scratch)*> hs_scratch{nullptr, &hs_free_scratch};
};
#endif /*NORMALIZOR_H*/
