
#include <cstring>
#include <istream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <hs/hs_compile.h>
#include <hs/hs_runtime.h>

#include "normalizor.hpp"


int Line_normalizer::on_match(unsigned int id, unsigned long long start,
                              unsigned long long to, unsigned int,
                              void* scractch_ctx)
{
  struct Line_context* ctx = static_cast<struct Line_context*>(scractch_ctx);
  if (id == 0) {
    ctx->parsed_lines.emplace_back(Normal_line(std::string(&ctx->block[ctx->last_boundary], to - ctx->last_boundary), ctx->cur_sections));
    ctx->cur_sections.clear();
    ctx->last_boundary = to;
  } else {
    size_t relative_start = static_cast<size_t>(start - ctx->last_boundary);
    size_t relative_end = static_cast<size_t>(to - ctx->last_boundary);
    auto start_it = ctx->cur_sections.find(relative_start);
    if (start_it == ctx->cur_sections.end() ||
        ctx->cur_sections[relative_start].second < relative_end ||
        (ctx->cur_sections[relative_start].second == relative_end  &&
         static_cast<unsigned int>(ctx->cur_sections[relative_start].first) > id))
    {
      ctx->cur_sections[relative_start] = std::make_tuple(relative_end, id);
    }
  }
  return 0;
}

// Return False on failure or end of file, true on end is not reached yet.
bool Line_normalizer::read_block(std::istream& in) {
  in.read(block, blocksize);
  if (in.eof() || !in)
      return false;

  // walk stream back to last newline.
  std::streampos last_newline = blocksize - 1;
  for (; last_newline >= 0 && block[last_newline] != '\n';
       last_newline -= 1) {}
  if (last_newline >= 0) {
    in.seekg((static_cast<std::streampos>(blocksize) - last_newline),
             std::ios_base::cur);
  }
  return true;
}

// Return vector of sections defining all sections
std::vector<Normal_line> Line_normalizer::normalize(std::istream& in) {
  Line_context lines(block);
  if (!build_hs_database())
    return lines.parsed_lines;
  bool readmore = true;
  while (readmore) {
    readmore = read_block(in);
    lines.cur_sections.clear();
    lines.last_boundary = 0;
    if (in.gcount() > 0) {
      hs_scan(hs_db.get(), block, static_cast<unsigned int>(in.gcount()), 0,
              hs_scratch.get(), on_match, static_cast<void*>(&lines));
    }
  }
  return lines.parsed_lines;
}

bool Line_normalizer::build_hs_database() {
  hs_database_t* db = nullptr;
  hs_compile_error_t* err = nullptr;
  std::vector<const char*> regexes;
  std::vector<unsigned> ids;
  std::vector<unsigned> flags;
  regexes.reserve(normal_types.size());
  for (const auto& nt : this->normal_types) {
    regexes.push_back(nt.second.regex.c_str());
    flags.push_back(static_cast<unsigned int>(nt.second.flags) | HS_FLAG_SOM_LEFTMOST);
    ids.push_back(static_cast<unsigned int>(nt.first));
  }

  if (hs_compile_multi(regexes.data(), flags.data(), ids.data(),
                       static_cast<unsigned int>(regexes.size()),
                       HS_MODE_BLOCK, nullptr, &db, &err) != HS_SUCCESS) {
    hs_free_database(db);
    hs_free_compile_error(err);
    return false;
  }
  this->hs_db.reset(db);
  hs_scratch_t* hs_sc = nullptr;
  if (hs_alloc_scratch(hs_db.get(), &hs_sc) != HS_SUCCESS) {
    hs_free_database(db);
    hs_free_scratch(hs_sc);
    return false;
  }
  this->hs_scratch.reset(hs_sc);
  return true;
}
