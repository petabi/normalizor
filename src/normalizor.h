//===-------- normalizor.h, Highspeed file parsing ----------------------===//

/*!
 * Copyright (c) 2017-2018 Petabi, Inc.
 * All rights reserved.
 *
 * \brief normalizor is a tool to parse a large set of text and identify each
 *        line, as well as the boundaries of individuals sections in each
 *        line.
 *
 * The primary purpose of normalizor is to quickly parse a large number of
 * log files or similar textual inputs, identify each line, and further
 * identify fields within each line.  The ultimate output is a vector of lines
 * and the "sections" for each line where each element in the sections
 * represents a region of the line that is defined as a region meeting
 * defined normalization requirements.
 * The caller may then normalize these values out of the line
 * at their leisure.  In essence, normalizor provides a low-level, high-speed
 * means to realize standard substring replacement for large files or
 * input streams.
 */
#ifndef NORMALIZOR_H
#define NORMALIZOR_H

#include <array>
#include <cstring>
#include <fstream>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <hs/hs_common.h>
#include <hs/hs_compile.h>
#include <hs/hs_runtime.h>

/*!
 * \brief The size of the number of characters (or bytes) processed at once.
 */
constexpr size_t blocksize = 65536;

/*!
 * \brief The Normal_type is a structure for storing the data used to identify
 *        sections in an line.
 *
 * A normal type consists of three attributes: a regular expression, a set of
 * flags modifying the regular expression, and a replacement value.  The
 * regular expression (and flags) are used to identify the section during
 * normalization.  The replacement string can be used to replace the normalize
 * section.
 */
struct Normal_type {
  Normal_type() : regex(), replacement() {}
  explicit Normal_type(std::string re, unsigned int f, std::string rep)
      : regex(std::move(re)), flags(f), replacement(std::move(rep))
  {
  }
  Normal_type(const struct Normal_type&) = default;
  Normal_type(struct Normal_type&&) noexcept(true) = default;
  Normal_type& operator=(struct Normal_type&&) noexcept(true) = default;
  Normal_type& operator=(const struct Normal_type&) = default;
  ~Normal_type() noexcept(true) = default;

  std::string regex;
  unsigned int flags{0};
  char _padding[4]{0};
  std::string replacement;
};

using Sections = std::map<size_t, std::pair<int, size_t>>;

/*!
 * \brief The Normal_line struct contains a single line of data as well as
 *        a data structure outlining all of the Normal_types in the line.
 *
 * The line, as parsed from the data, is stored in the line member,
 * unabridged.  The sections structure contains the set of Normal_types
 * identified in the line.  The sections structure is defined as follows:
 *   map {
 *     key = start offset of the Normal_type in the line
 *     value = pair {
 *       first = id for this Normal_type as determined by the normal_types
 *               member of Line_normalizer.
 *       second = end offset for the Normal_type in the line.
 *     }
 *  }
 */
struct Normal_line {
  Normal_line(std::string l, Sections& secs) : line(std::move(l)), sections()
  {
    std::swap(secs, sections);
  }
  Normal_line(const struct Normal_line&) = default;
  Normal_line(struct Normal_line&&) noexcept(true) = default;
  Normal_line& operator=(struct Normal_line&&) noexcept(true) = default;
  Normal_line& operator=(const struct Normal_line&) = default;
  ~Normal_line() noexcept(true) = default;

  std::string line;
  Sections sections;
};

using Normal_list = std::vector<struct Normal_line>;

/*!
 * \brief The Line_context is a structure used internally to facilitate the
 *        identification of lines and sections.
 */
struct Line_context {
  Line_context() = default;
  Line_context(const char* b) : block(b) {}
  const char* block{nullptr};
  size_t last_boundary{0};
  Sections cur_sections;
  Normal_list parsed_lines;
};

/*!
 * \brief The Line_normalizer is the core class for peforming normalization.
 *
 * \code{.cpp}
 * Line_normalizor norm;
 * std::vector<struct Normal_line> my_norm_lines =
 *                                 norm.normalize(my_input_stream);
 * \endcode
 */
class Line_normalizer {
public:
  /*!
   * \brief Provides a copy of the current set of Normal_types used for this
   *        normalizer object.
   *
   * \code{.cpp}
   * auto my_norm_types = norm.get_current_normal_types();
   * \endcode
   *
   * \returns map of normal types where map key is the 'ID" for the normal_type
   *          and the value is the Normal_type object.
   */
  const std::map<size_t, struct Normal_type>& get_current_normal_types() const
  {
    return normal_types;
  }

  /*!
   * \brief Allows the user to set new Normal_types.
   *
   * \code{.cpp}
   *  norm.modify_current_normal_types(1, new_normal_type);
   * \endcode
   *
   * NOTE: The Normal_types contained in the normalizer are held in a map
   *       where the key is considered the 'ID' for a particular Normal_type.
   *       The public variable line_end_id is the key for the Normal_type
   *       defining a line end in the input.  Changing the Normal_type
   *       associated with line_end_id to something that does not identify a
   *       line ending in the input will cause undefined behavior.  Also note,
   *       normalization occurs in a hierarchy.  Normal_types with lower IDs
   *       (ID's closer to zero) are given preference in any ties.  Thus,
   *       Normal_types with higher IDs will only match if there are no
   *       conflicts.  Further, longer matches are preferred over shorter.
   *       Finally, it is expected that all regular expressions
   *       used in the Normal_types will compile with hyperscan.
   *
   * \param nt_id The ID of the Normal_type to modify.
   * \param nt A new Normal_type object.
   */
  void modify_current_normal_types(size_t nt_id, struct Normal_type nt)
  {
    normal_types[nt_id] = std::move(nt);
  }

  /*!
   * \brief Parses the input stream until it can read no more characters and
   *        then returns a vector of Normal_line objects where each object
   *        represents one line parsed from the input stream.
   *
   * \code{.cpp}
   *  my_norm_lines = norm.normalize(in);
   * \endcode
   *
   * \returns a vector of Normal_line objects.
   */
  const Normal_list& normalize();

  /*!
   * \brief Designate the file, or stream, to normalize.  If stream assumes
   *        the caller is responsible for the stream.
   *
   * \param stream filename or stream for normalizing.
   */
  void set_input_stream(const std::string& stream);
  void set_input_stream(std::istream& stream);

  /*!
   * \brief The ID for the line_end Normal_type.
   */
  static const size_t line_end_id;

private:
  /*!
   * \brief build hyperscan database returns true on success / false otherwise.
   */
  bool build_hs_database();

  /*!
   * \brief Per match function used by hyperscan.
   */
  static int on_match(unsigned int id, unsigned long long start,
                      unsigned long long to, unsigned int, void* ctx);

  /*!
   * \brief Reads a block of data from the inputstream and places it in block.
   *        Returns number of characters read.
   */
  size_t read_block();

  // member variables.
  std::array<char, blocksize> block;
  struct Line_context context;
  std::unique_ptr<hs_database_t, decltype(hs_free_database)*> hs_db{
      nullptr, &hs_free_database};
  std::map<size_t, struct Normal_type> normal_types = {
      {line_end_id, Normal_type(R"(\n|\r\n)", 0u, "<NL>")},
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
                      R"([0-9A-Za-z+/]{2}==)+)",
                      0u, "<B64>")},
      {4, Normal_type(R"((\x5c{1,2}x[0-9A-Fa-f]{2}|%[0-9A-Fa-f]{2}|)"
                      R"([\x7f-\xff])+)",
                      0u, "<HEX>")},
      {5,
       Normal_type(R"([v/]?\d{1,3}[._]\d{1,3}([._]\d{1,3})?\b)", 0u, "<VN>")},
      {6, Normal_type(R"(\d+(\.\d+)?)", 0u, "<DEC>")},
      {7, Normal_type(R"(\W+)", 0u, "<NW>")}};
  std::unique_ptr<hs_scratch_t, decltype(hs_free_scratch)*> hs_scratch{
      nullptr, &hs_free_scratch};
  std::unique_ptr<std::ifstream> file_to_normalize;
  std::istream* stream_to_normalize = nullptr;
};

/*!
 * \brief Need to provide comparison functions for Normal_line to facilitate
 *        conversion to python.
 */
inline bool operator==(const struct Normal_line& lhs,
                       const struct Normal_line& rhs)
{
  return (lhs.line == rhs.line && lhs.sections == rhs.sections);
}
inline bool operator!=(const struct Normal_line& lhs,
                       const struct Normal_line& rhs)
{
  return !(lhs == rhs);
}

#endif /*NORMALIZOR_H*/
