
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <sys/resource.h>
#include <sys/time.h>
#include <tuple>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <gperftools/profiler.h>

#include "normalizor.hpp"

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
  optargs.add_options()("profile,p", "Dump profile results to normalizor_profile.txt");
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
  std::cout << "total time to process:" << "\n";
  timeradd(&total, &diff, &total);
  std::cout << total.tv_sec << "." << total.tv_usec << "\n";
  std::cout << "Total lines processed:";
  std::cout << std::to_string(lines.size()) << "\n";
  std::cout << "lines per sec: " << std::to_string(lines.size() / (static_cast<double>(total.tv_sec) + static_cast<double>(total.tv_usec / 1000000))) << "\n";
  
}
