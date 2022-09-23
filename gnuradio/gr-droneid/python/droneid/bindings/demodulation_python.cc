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
/* BINDTOOL_HEADER_FILE(demodulation.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(848192abf7e376e88031ffe617420115)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/droneid/demodulation.h>
// pydoc.h is automatically generated in the build directory
#include <demodulation_pydoc.h>

void bind_demodulation(py::module& m)
{

    using demodulation = ::gr::droneid::demodulation;


    py::class_<demodulation,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<demodulation>>(m, "demodulation", D(demodulation))

        .def(py::init(&demodulation::make),
             py::arg("sample_rate"),
             py::arg("debug_path"),
             D(demodulation, make))


        ;
}
