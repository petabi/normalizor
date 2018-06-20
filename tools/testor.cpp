//===-------- testor.cpp, basic testing for normalizor -------------------===//
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <tuple>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <gperftools/profiler.h>

#include "normalizor.h"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  struct rusage start, end;
  Line_normalizer norm;
  std::filebuf fb;
  std::vector<Normal_line> lines;
  std::string log_file;
  po::options_description posargs;
  posargs.add_options()("log_file", po::value<std::string>(&log_file),
                        "Log file to normalize.");
  po::positional_options_description positions;
  positions.add("log_file", 1);
  po::options_description optargs("Options");
  optargs.add_options()("help,h", "Print usage information.");
  optargs.add_options()("profile,p",
                        "Dump profile results to normalizor_profile.txt");
  optargs.add_options()("debug,d", "Print all lines parsed.");
  po::options_description cliargs;
  cliargs.add(posargs).add(optargs);
  po::options_description cliopts;
  cliopts.add(optargs);
  po::variables_map args;
  try {
    po::store(po::command_line_parser(argc, argv)
                  .options(cliargs)
                  .positional(positions)
                  .run(),
              args);
  } catch (const po::error& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  po::notify(args);
  std::cout << "Normalizor: Starting Normalization!\n";
  if (args.count("profile")) {
    ProfilerStart("normalizer_profile.txt");
  }
  getrusage(RUSAGE_SELF, &start);
  if (fb.open(log_file.c_str(), std::ios_base::in)) {
    std::istream in(&fb);
    lines = norm.normalize(in);
  }
  getrusage(RUSAGE_SELF, &end);
  std::cout << "Normalization Complete!\n";
  if (args.count("profile")) {
    ProfilerFlush();
    ProfilerStop();
  }
  struct timeval diff, total;
  timersub(&(end.ru_utime), &(start.ru_utime), &total);
  timersub(&(end.ru_stime), &(start.ru_stime), &diff);
  timeradd(&total, &diff, &total);
  double total_proc_time = static_cast<double>(total.tv_sec) +
                           (static_cast<double>(total.tv_usec) / 1000000.0);
  if (args.count("debug")) {
    std::cout << "Printing Debug information\n";
    for (const auto& l : lines) {
      std::cout << l.line.c_str();
    }
  }
  struct stat file_stats;
  stat(log_file.c_str(), &file_stats);
  double bytes_per_sec = static_cast<double>(file_stats.st_size) /
                         total_proc_time;
  size_t line_sum = 0.0;
  for (const auto& l : lines) {
    line_sum += l.line.size();
  }
  std::cout << "Processing Statistics\n";
  std::cout << "--Total time to process: " << std::to_string(total_proc_time);
  std::cout << "\n";
  std::cout << "--Total lines processed: ";
  std::cout << std::to_string(lines.size()) << "\n";
  std::cout << "--Lines per sec: " << std::to_string(
               static_cast<double>(lines.size()) / total_proc_time) << "\n";
  std::cout << "--Average Characters per Line: " <<
            static_cast<double>(line_sum) /
            static_cast<double>(lines.size()) << "\n";
  std::cout << "--Bytes per sec: " <<
                std::to_string(bytes_per_sec) << " (" << std::to_string(
                bytes_per_sec / 1024.0 / 1024.0) << " MB per sec)\n";
}
