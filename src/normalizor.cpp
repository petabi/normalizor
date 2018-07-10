//===-------- normalizor.cpp , Highspeed file parsing -------------------===//
/*!
 * Copyright (c) 2017-2018 Petabi, Inc.
 * All rights reserved.
 */

#include <cstring>
#include <fstream>
#include <istream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <hs/hs_common.h>
#include <hs/hs_compile.h>
#include <hs/hs_runtime.h>

#include "normalizor.h"

bool Line_normalizer::build_hs_database()
{
  hs_database_t* db = nullptr;
  hs_compile_error_t* err = nullptr;
  std::vector<const char*> regexes;
  std::vector<unsigned int> ids;
  std::vector<unsigned int> flags;
  regexes.reserve(normal_types.size());
  for (const auto& nt : this->normal_types) {
    regexes.push_back(nt.second.regex.c_str());
    flags.push_back(nt.second.flags | HS_FLAG_SOM_LEFTMOST);
    ids.push_back(static_cast<unsigned int>(nt.first));
  }

  if (hs_compile_multi(regexes.data(), flags.data(), ids.data(),
                       static_cast<unsigned int>(regexes.size()), HS_MODE_BLOCK,
                       nullptr, &db, &err) != HS_SUCCESS) {
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

const Normal_list& Line_normalizer::normalize()
{
  context.block = block.data();
  context.parsed_lines.clear();
  if (!stream_to_normalize || !build_hs_database())
    return context.parsed_lines;
  size_t char_read = read_block();
  context.parsed_lines.reserve(blocksize);
  while (char_read > 0) {
    context.cur_sections.clear();
    context.last_boundary = 0;
    hs_scan(hs_db.get(), block.data(), static_cast<unsigned int>(char_read), 0,
            hs_scratch.get(), on_match, static_cast<void*>(&context));
    char_read = read_block();
  }
  return context.parsed_lines;
}

int Line_normalizer::on_match(unsigned int id, unsigned long long start,
                              unsigned long long to, unsigned int,
                              void* scractch_ctx)
{
  auto ctx = static_cast<struct Line_context*>(scractch_ctx);
  if (id == line_end_id) {
    // Finished parsing a line, so need to build a Normal_line.
    if (!ctx->cur_sections.empty()) {
      size_t longest = ctx->cur_sections.begin()->second.second;
      auto sec_it = ctx->cur_sections.begin();
      ++sec_it;
      auto end_it = ctx->cur_sections.end();
      // This section is to remove sections that are contained in larger ones.
      while (sec_it != end_it) {
        if (sec_it->first < longest) {
          sec_it = ctx->cur_sections.erase(sec_it);
        } else {
          longest = sec_it->second.second;
          ++sec_it;
        }
      }
      ctx->parsed_lines.emplace_back(
          std::string(&ctx->block[ctx->last_boundary], to - ctx->last_boundary),
          ctx->cur_sections);
    }
    ctx->last_boundary = to;
  } else {
    auto relative_start = static_cast<size_t>(start - ctx->last_boundary);
    auto relative_end = static_cast<size_t>(to - ctx->last_boundary);
    auto start_it = ctx->cur_sections.find(relative_start);
    if (start_it == ctx->cur_sections.end() ||
        ctx->cur_sections[relative_start].second < relative_end ||
        (ctx->cur_sections[relative_start].second == relative_end &&
         static_cast<unsigned int>(ctx->cur_sections[relative_start].first) >
             id)) {
      ctx->cur_sections[relative_start] = std::make_tuple(id, relative_end);
    }
  }
  return 0;
}

size_t Line_normalizer::read_block()
{
  if (!stream_to_normalize)
    return 0;
  stream_to_normalize->read(block.data(), blocksize);
  if (stream_to_normalize->eof()) {
    return static_cast<size_t>(stream_to_normalize->gcount());
  }
  if (!stream_to_normalize)
    return 0;

  // walk stream back to last newline.
  long chars_read = stream_to_normalize->gcount();
  long last_newline = chars_read;
  for (; last_newline > 0 &&
       block[static_cast<size_t>(last_newline - 1)] != '\n'; --last_newline) {}
  stream_to_normalize->seekg(static_cast<std::streamoff>(
                            last_newline - chars_read),
                            std::ios_base::cur);
  return static_cast<size_t>(last_newline);
}

void Line_normalizer::set_input_stream(const std::string& stream) {
  file_to_normalize.reset(new std::ifstream(stream, std::ios_base::in));
  stream_to_normalize = static_cast<std::istream*>(file_to_normalize.get());
}

void Line_normalizer::set_input_stream(std::istream& stream) {
  stream_to_normalize = &stream;
}
