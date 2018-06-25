//===-------- py_normalizor.cpp , python wrapper for normalizer ----------===//
/*!
 * Copyright (c) 2017-2018 Petabi, Inc.
 * All rights reserved.
 */

#include <cstring>
#include <istream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/def.hpp>
#include <boost/python/make_constructor.hpp>

#include "normalizor.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(py_normalizor) {
  class_<Normal_type>("Normal_type", init<std::string, unsigned int,
                      std::string>())
      .def_readonly("regex", &Normal_type::regex)
      .def_readonly("flags", &Normal_type::flags)
      .def_readonly("replacement", &Normal_type::replacement)
  ;

  class_<Normal_line, boost::noncopyable>("Normal_line", init<std::string, std::map<size_t, std::pair<int, size_t>>&>())
      .def_readonly("line", &Normal_line::line)
      .def_readonly("sections", &Normal_line::sections)
  ;

  class_<Line_normalizer, boost::noncopyable>("Line_normalizer")
      .def("get_current_normal_types",
           &Line_normalizer::get_current_normal_types,
           return_value_policy<reference_existing_object>())
      .def("modify_current_normal_types",
           &Line_normalizer::modify_current_normal_types)
      .def("normalize", &Line_normalizer::normalize,
           return_value_policy<copy_const_reference>())
      .def_readonly("blocksize", &Line_normalizer::blocksize)
      .def_readonly("line_end_id", &Line_normalizer::line_end_id)
  ;
}
