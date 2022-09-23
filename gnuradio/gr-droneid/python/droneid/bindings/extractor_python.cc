/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(extractor.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(18a466547b48a63d103e999c9547d6b3)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/droneid/extractor.h>
// pydoc.h is automatically generated in the build directory
#include <extractor_pydoc.h>

void bind_extractor(py::module& m)
{

    using extractor = ::gr::droneid::extractor;


    py::class_<extractor,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<extractor>>(m, "extractor", D(extractor))

        .def(py::init(&extractor::make),
             py::arg("arg0"),
             py::arg("arg1"),
             D(extractor, make))


        .def("set_threshold",
             &extractor::set_threshold,
             py::arg("arg0"),
             D(extractor, set_threshold))

        ;
}
