/* -*- c++ -*- */
/*
 * Copyright 2022 gr-droneid author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/droneid/lte_decode.h>
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace droneid {

lte_decode::lte_decode() {
    d1_.resize(TURBO_DECODER_BIT_COUNT);
    d2_.resize(TURBO_DECODER_BIT_COUNT);
    d3_.resize(TURBO_DECODER_BIT_COUNT);

    turbo_decoder_input_.resize(TURBO_DECODER_INPUT_BIT_COUNT);
    decoded_bytes_.resize(EXPECTED_PAYLOAD_BYTES);

    rate_matcher_ = lte_rate_matcher_alloc();
    turbo_decoder_ = alloc_tdec();

    rate_matcher_io_ = {
        .D = TURBO_DECODER_BIT_COUNT,
        .E = TURBO_DECODER_INPUT_BIT_COUNT,
        .d = {&d1_[0], &d2_[0], &d3_[0]},
        .e = &turbo_decoder_input_[0]
    };
}

lte_decode::~lte_decode() {
    if (rate_matcher_ != nullptr) {
        lte_rate_matcher_free(rate_matcher_);
    }

    if (turbo_decoder_ != nullptr) {
        free_tdec(turbo_decoder_);
    }
}

std::vector<uint8_t> lte_decode::decode(const std::vector<int8_t> &bits) {
    if (bits.size() != TURBO_DECODER_INPUT_BIT_COUNT) {
        std::ostringstream err;
        err << "Turbo decoder expected " << TURBO_DECODER_INPUT_BIT_COUNT << " but got " << bits.size();
        throw std::runtime_error(err.str());
    }

    int8_t bit_lut[2] = {-63, 63};
    std::vector<int8_t> bits_copy = bits;
    for (int idx = 0; idx < bits.size(); idx++) {
        turbo_decoder_input_[idx] = bit_lut[bits[idx]];
    }

    lte_rate_match_fw(rate_matcher_, &rate_matcher_io_, 0);
    const int decode_status = lte_turbo_decode(turbo_decoder_, EXPECTED_PAYLOAD_BITS, TURBO_ITERATIONS,
                                               &decoded_bytes_[0], &d1_[0], &d2_[0], &d3_[0]);

    fprintf(stdout, "MOO: ");
    for (const auto & i : decoded_bytes_) {
        fprintf(stdout, "%02x", i);
    }
    fprintf(stdout, "\n");

    if (decode_status != 0) {
        throw std::runtime_error("Failed to remove Turbo code.  Status: " + std::to_string(decode_status));
    }

    const uint32_t calculated_crc = CRC::Calculate(&decoded_bytes_[0], decoded_bytes_.size(), CRC::CRC_24_LTEA());
    if (calculated_crc != 0) {
        return {};
    }

    return decoded_bytes_;
}


} /* namespace droneid */
} /* namespace gr */
