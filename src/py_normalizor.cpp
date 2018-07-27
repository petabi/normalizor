//===-------- py_normalizor.cpp , python wrapper for normalizer ----------===//
/*!
 * Copyright (c) 2017-2018 Petabi, Inc.
 * All rights reserved.
 */

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/def.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/module.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include "normalizor.h"

using namespace boost::python;

/*! \brief This declares the python module.  The name must match the library
 *  name exactly!
 */
BOOST_PYTHON_MODULE(py_normalizor) {

  std_pair_to_python_converter<int, size_t>();

  /*! \brief Provides a means to simplify function overloading.  In this case
   *         This allows python to know which function to use given that
   *         set_input_stream has two possible choices.
   */
  void (Line_normalizer::*s1)(const std::string&) =
        &Line_normalizer::set_input_stream;

  /*! \brief The following 2 classes are for facilitating conversion of data
   *         types to and from python.  Both of these data types are typedefed
   *         in normalizor.h.
   */
  class_<Sections>("Sections")
      .def(map_indexing_suite<Sections>());

  class_<Normal_list>("Normal_list")
      .def(vector_indexing_suite<Normal_list>());

  /*! \brief Exposes Normal_type to python.
   */
  class_<Normal_type>("Normal_type", init<std::string, unsigned int,
                      std::string>())
      .def_readonly("regex", &Normal_type::regex)
      .def_readonly("flags", &Normal_type::flags)
      .def_readonly("replacement", &Normal_type::replacement)
  ;

  /*! \brief Exposes Normal_line to python.
   */
  class_<Normal_line, boost::noncopyable>("Normal_line",
      init<std::string, Sections&>())
      .def_readonly("line", &Normal_line::line)
      .def_readonly("sections", &Normal_line::sections)
  ;

  /*! \brief Exposes Line_normalizer to python.
   */
  class_<Line_normalizer, boost::noncopyable>("Line_normalizer", init<>())
      .def("get_current_normal_types",
           &Line_normalizer::get_current_normal_types,
           return_value_policy<reference_existing_object>())
      .def("modify_current_normal_types",
           &Line_normalizer::modify_current_normal_types)
      .def("normalize", &Line_normalizer::normalize,
           return_value_policy<copy_const_reference>())
      .def("set_input_stream", s1)
      .def_readonly("blocksize", &Line_normalizer::blocksize)
      .def_readonly("line_end_id", &Line_normalizer::line_end_id)
  ;
}
