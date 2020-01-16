//
// Created by yangzhou on 1/14/20.
//

#include <iomanip>
#include <typeinfo>
#include <unordered_map>
#include "unit_test.hpp"

#include "platon/bigint.hpp"
using namespace std;

TEST_CASE(bigint, testShifts) {
    //left
    uint512_t a1 = 0;
    for (auto c : uint512_t::_impl::shift_left(a1, 0).value()) {
        ASSERT(c == 0, "");
    }
    uint512_t a2 = 0;
    for (auto c : uint512_t::_impl::shift_left(a2, 15).value()) {
        ASSERT(c == 0, "");
    }

    uint512_t a3 = 0;
    a3.value()[56] = 0xf1;
    a3.value()[57] = 0xf2;
    a3.value()[58] = 0xf3;
    a3.value()[59] = 0xf4;
    a3.value()[60] = 0xf5;
    a3.value()[61] = 0xf6;
    a3.value()[62] = 0xf7;
    a3.value()[63] = 0xf8;
    uint512_t a31 = uint512_t::_impl::shift_left(a3, 8);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a31.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a31.value()[55] == 0xf1, "");
    ASSERT(a31.value()[56] == 0xf2, "");
    ASSERT(a31.value()[57] == 0xf3, "");
    ASSERT(a31.value()[58] == 0xf4, "");
    ASSERT(a31.value()[59] == 0xf5, "");
    ASSERT(a31.value()[60] == 0xf6, "");
    ASSERT(a31.value()[61] == 0xf7, "");
    ASSERT(a31.value()[62] == 0xf8, "");
    ASSERT(a31.value()[63] == 0x00, "");

    uint512_t a32 = uint512_t::_impl::shift_left(a3, 16);
    for (int idx = 0; idx < 54; ++idx) {
        ASSERT(a32.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a32.value()[54] == 0xf1, "");
    ASSERT(a32.value()[55] == 0xf2, "");
    ASSERT(a32.value()[56] == 0xf3, "");
    ASSERT(a32.value()[57] == 0xf4, "");
    ASSERT(a32.value()[58] == 0xf5, "");
    ASSERT(a32.value()[59] == 0xf6, "");
    ASSERT(a32.value()[60] == 0xf7, "");
    ASSERT(a32.value()[61] == 0xf8, "");
    ASSERT(a32.value()[62] == 0x00, "");
    ASSERT(a32.value()[63] == 0x00, "");

    uint512_t a33 = uint512_t::_impl::shift_left(a3, 4);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a33.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a33.value()[55] == 0x0f, "");
    ASSERT(a33.value()[56] == 0x1f, "");
    ASSERT(a33.value()[57] == 0x2f, "");
    ASSERT(a33.value()[58] == 0x3f, "");
    ASSERT(a33.value()[59] == 0x4f, "");
    ASSERT(a33.value()[60] == 0x5f, "");
    ASSERT(a33.value()[61] == 0x6f, "");
    ASSERT(a33.value()[62] == 0x7f, "");
    ASSERT(a33.value()[63] == 0x80, "");

    uint512_t a34 = uint512_t::_impl::shift_left(a3, 64);
    for (int idx = 0; idx < 48; ++idx) {
        ASSERT(a34.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a34.value()[48] == 0xf1, "");
    ASSERT(a34.value()[49] == 0xf2, "");
    ASSERT(a34.value()[50] == 0xf3, "");
    ASSERT(a34.value()[51] == 0xf4, "");
    ASSERT(a34.value()[52] == 0xf5, "");
    ASSERT(a34.value()[53] == 0xf6, "");
    ASSERT(a34.value()[54] == 0xf7, "");
    ASSERT(a34.value()[55] == 0xf8, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a34.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a35 = uint512_t::_impl::shift_left(a3, 256);
    for (int idx = 0; idx < 24; ++idx) {
        ASSERT(a35.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a35.value()[24] == 0xf1, "");
    ASSERT(a35.value()[25] == 0xf2, "");
    ASSERT(a35.value()[26] == 0xf3, "");
    ASSERT(a35.value()[27] == 0xf4, "");
    ASSERT(a35.value()[28] == 0xf5, "");
    ASSERT(a35.value()[29] == 0xf6, "");
    ASSERT(a35.value()[30] == 0xf7, "");
    ASSERT(a35.value()[31] == 0xf8, "");
    for (int idx = 32; idx < 63; ++idx) {
        ASSERT(a35.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a36 = uint512_t::_impl::shift_left(a3, 260);
    for (int idx = 0; idx < 23; ++idx) {
        ASSERT(a36.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a36.value()[23] == 0x0f, "");
    ASSERT(a36.value()[24] == 0x1f, "");
    ASSERT(a36.value()[25] == 0x2f, "");
    ASSERT(a36.value()[26] == 0x3f, "");
    ASSERT(a36.value()[27] == 0x4f, "");
    ASSERT(a36.value()[28] == 0x5f, "");
    ASSERT(a36.value()[29] == 0x6f, "");
    ASSERT(a36.value()[30] == 0x7f, "");
    ASSERT(a36.value()[31] == 0x80, "");
    for (int idx = 32; idx < 63; ++idx) {
        ASSERT(a36.value()[idx] == 0, std::to_string(idx));
    }

    //right uint
    uint512_t a4 = 0;
    for (auto c : uint512_t::_impl::shift_right(a4, 0).value()) {
        ASSERT(c == 0, "");
    }
    uint512_t a5 = 0;
    for (auto c : uint512_t::_impl::shift_right(a5, 15).value()) {
        ASSERT(c == 0, "");
    }

    uint512_t a6 = 0;
    a6.value()[56] = 0xf1;
    a6.value()[57] = 0xf2;
    a6.value()[58] = 0xf3;
    a6.value()[59] = 0xf4;
    a6.value()[60] = 0xf5;
    a6.value()[61] = 0xf6;
    a6.value()[62] = 0xf7;
    a6.value()[63] = 0xf8;
    a6.value()[0] = 0xf1;
    a6.value()[1] = 0xf2;
    a6.value()[2] = 0xf3;
    a6.value()[3] = 0xf4;
    a6.value()[4] = 0xf5;
    a6.value()[5] = 0xf6;
    a6.value()[6] = 0xf7;
    a6.value()[7] = 0xf8;
    uint512_t a61 = uint512_t::_impl::shift_right(a6, 8);
    ASSERT(a61.value()[0] == 0, "");
    ASSERT(a61.value()[1] == 0xf1, "");
    ASSERT(a61.value()[2] == 0xf2, "");
    ASSERT(a61.value()[3] == 0xf3, "");
    ASSERT(a61.value()[4] == 0xf4, "");
    ASSERT(a61.value()[5] == 0xf5, "");
    ASSERT(a61.value()[6] == 0xf6, "");
    ASSERT(a61.value()[7] == 0xf7, "");
    ASSERT(a61.value()[8] == 0xf8, "");
    for (int idx = 9; idx < 57; ++idx) {
        ASSERT(a61.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a61.value()[57] == 0xf1, "");
    ASSERT(a61.value()[58] == 0xf2, "");
    ASSERT(a61.value()[59] == 0xf3, "");
    ASSERT(a61.value()[60] == 0xf4, "");
    ASSERT(a61.value()[61] == 0xf5, "");
    ASSERT(a61.value()[62] == 0xf6, "");
    ASSERT(a61.value()[63] == 0xf7, "");

    uint512_t a62 = uint512_t::_impl::shift_right(a6, 16);
    ASSERT(a62.value()[0] == 0, "");
    ASSERT(a62.value()[1] == 0, "");
    ASSERT(a62.value()[2] == 0xf1, "");
    ASSERT(a62.value()[3] == 0xf2, "");
    ASSERT(a62.value()[4] == 0xf3, "");
    ASSERT(a62.value()[5] == 0xf4, "");
    ASSERT(a62.value()[6] == 0xf5, "");
    ASSERT(a62.value()[7] == 0xf6, "");
    ASSERT(a62.value()[8] == 0xf7, "");
    ASSERT(a62.value()[9] == 0xf8, "");
    for (int idx = 10; idx < 58; ++idx) {
        ASSERT(a62.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a62.value()[58] == 0xf1, "");
    ASSERT(a62.value()[59] == 0xf2, "");
    ASSERT(a62.value()[60] == 0xf3, "");
    ASSERT(a62.value()[61] == 0xf4, "");
    ASSERT(a62.value()[62] == 0xf5, "");
    ASSERT(a62.value()[63] == 0xf6, "");

    uint512_t a63 = uint512_t::_impl::shift_right(a6, 4);
    ASSERT(a63.value()[0] == 0x0f, "");
    ASSERT(a63.value()[1] == 0x1f, "");
    ASSERT(a63.value()[2] == 0x2f, "");
    ASSERT(a63.value()[3] == 0x3f, "");
    ASSERT(a63.value()[4] == 0x4f, "");
    ASSERT(a63.value()[5] == 0x5f, "");
    ASSERT(a63.value()[6] == 0x6f, "");
    ASSERT(a63.value()[7] == 0x7f, "");
    ASSERT(a63.value()[8] == 0x80, "");
    for (int idx = 9; idx < 56; ++idx) {
        ASSERT(a63.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a63.value()[56] == 0x0f, "");
    ASSERT(a63.value()[57] == 0x1f, "");
    ASSERT(a63.value()[58] == 0x2f, "");
    ASSERT(a63.value()[59] == 0x3f, "");
    ASSERT(a63.value()[60] == 0x4f, "");
    ASSERT(a63.value()[61] == 0x5f, "");
    ASSERT(a63.value()[62] == 0x6f, "");
    ASSERT(a63.value()[63] == 0x7f, "");

    uint512_t a64 = uint512_t::_impl::shift_right(a6, 64);
    for (int idx = 0; idx < 8; ++idx) {
        ASSERT(a64.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a64.value()[8] == 0xf1, "");
    ASSERT(a64.value()[9] == 0xf2, "");
    ASSERT(a64.value()[10] == 0xf3, "");
    ASSERT(a64.value()[11] == 0xf4, "");
    ASSERT(a64.value()[12] == 0xf5, "");
    ASSERT(a64.value()[13] == 0xf6, "");
    ASSERT(a64.value()[14] == 0xf7, "");
    ASSERT(a64.value()[15] == 0xf8, "");
    for (int idx = 16; idx < 63; ++idx) {
        ASSERT(a64.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a65 = uint512_t::_impl::shift_right(a6, 256);
    for (int idx = 0; idx < 32; ++idx) {
        ASSERT(a65.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a65.value()[32] == 0xf1, "");
    ASSERT(a65.value()[33] == 0xf2, "");
    ASSERT(a65.value()[34] == 0xf3, "");
    ASSERT(a65.value()[35] == 0xf4, "");
    ASSERT(a65.value()[36] == 0xf5, "");
    ASSERT(a65.value()[37] == 0xf6, "");
    ASSERT(a65.value()[38] == 0xf7, "");
    ASSERT(a65.value()[39] == 0xf8, "");
    for (int idx = 40; idx < 63; ++idx) {
        ASSERT(a65.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a66 = uint512_t::_impl::shift_right(a6, 260);
    for (int idx = 0; idx < 32; ++idx) {
        ASSERT(a66.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a66.value()[32] == 0x0f, "");
    ASSERT(a66.value()[33] == 0x1f, "");
    ASSERT(a66.value()[34] == 0x2f, "");
    ASSERT(a66.value()[35] == 0x3f, "");
    ASSERT(a66.value()[36] == 0x4f, "");
    ASSERT(a66.value()[37] == 0x5f, "");
    ASSERT(a66.value()[38] == 0x6f, "");
    ASSERT(a66.value()[39] == 0x7f, "");
    ASSERT(a66.value()[40] == 0x80, "");
    for (int idx = 41; idx < 63; ++idx) {
        ASSERT(a66.value()[idx] == 0, std::to_string(idx));
    }

    //right int > 0
    int512_t b4 = 0;
    for (auto c : int512_t::_impl::shift_right(b4, 0).value()) {
        ASSERT(c == 0, "");
    }
    int512_t b5 = 0;
    for (auto c : int512_t::_impl::shift_right(b5, 15).value()) {
        ASSERT(c == 0, "");
    }

    int512_t b6 = 0;
    b6.value()[0] = 0x71;
    b6.value()[1] = 0xf2;
    b6.value()[2] = 0xf3;
    b6.value()[3] = 0xf4;
    b6.value()[4] = 0xf5;
    b6.value()[5] = 0xf6;
    b6.value()[6] = 0xf7;
    b6.value()[7] = 0xf8;
    b6.value()[56] = 0xf1;
    b6.value()[57] = 0xf2;
    b6.value()[58] = 0xf3;
    b6.value()[59] = 0xf4;
    b6.value()[60] = 0xf5;
    b6.value()[61] = 0xf6;
    b6.value()[62] = 0xf7;
    b6.value()[63] = 0xf8;

    int512_t b61 = int512_t::_impl::shift_right(b6, 8);
    ASSERT(b61.value()[0] == 0, "");
    ASSERT(b61.value()[1] == 0x71, "");
    ASSERT(b61.value()[2] == 0xf2, "");
    ASSERT(b61.value()[3] == 0xf3, "");
    ASSERT(b61.value()[4] == 0xf4, "");
    ASSERT(b61.value()[5] == 0xf5, "");
    ASSERT(b61.value()[6] == 0xf6, "");
    ASSERT(b61.value()[7] == 0xf7, "");
    ASSERT(b61.value()[8] == 0xf8, "");
    for (int idx = 9; idx < 57; ++idx) {
        ASSERT(b61.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b61.value()[57] == 0xf1, "");
    ASSERT(b61.value()[58] == 0xf2, "");
    ASSERT(b61.value()[59] == 0xf3, "");
    ASSERT(b61.value()[60] == 0xf4, "");
    ASSERT(b61.value()[61] == 0xf5, "");
    ASSERT(b61.value()[62] == 0xf6, "");
    ASSERT(b61.value()[63] == 0xf7, "");

    int512_t b62 = int512_t::_impl::shift_right(b6, 16);
    ASSERT(b62.value()[0] == 0, "");
    ASSERT(b62.value()[1] == 0, "");
    ASSERT(b62.value()[2] == 0x71, "");
    ASSERT(b62.value()[3] == 0xf2, "");
    ASSERT(b62.value()[4] == 0xf3, "");
    ASSERT(b62.value()[5] == 0xf4, "");
    ASSERT(b62.value()[6] == 0xf5, "");
    ASSERT(b62.value()[7] == 0xf6, "");
    ASSERT(b62.value()[8] == 0xf7, "");
    ASSERT(b62.value()[9] == 0xf8, "");
    for (int idx = 10; idx < 58; ++idx) {
        ASSERT(b62.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b62.value()[58] == 0xf1, "");
    ASSERT(b62.value()[59] == 0xf2, "");
    ASSERT(b62.value()[60] == 0xf3, "");
    ASSERT(b62.value()[61] == 0xf4, "");
    ASSERT(b62.value()[62] == 0xf5, "");
    ASSERT(b62.value()[63] == 0xf6, "");

    int512_t b63 = int512_t::_impl::shift_right(b6, 4);
    ASSERT(b63.value()[0] == 0x07, "");
    ASSERT(b63.value()[1] == 0x1f, "");
    ASSERT(b63.value()[2] == 0x2f, "");
    ASSERT(b63.value()[3] == 0x3f, "");
    ASSERT(b63.value()[4] == 0x4f, "");
    ASSERT(b63.value()[5] == 0x5f, "");
    ASSERT(b63.value()[6] == 0x6f, "");
    ASSERT(b63.value()[7] == 0x7f, "");
    ASSERT(b63.value()[8] == 0x80, "");
    for (int idx = 9; idx < 56; ++idx) {
        ASSERT(b63.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b63.value()[56] == 0x0f, "");
    ASSERT(b63.value()[57] == 0x1f, "");
    ASSERT(b63.value()[58] == 0x2f, "");
    ASSERT(b63.value()[59] == 0x3f, "");
    ASSERT(b63.value()[60] == 0x4f, "");
    ASSERT(b63.value()[61] == 0x5f, "");
    ASSERT(b63.value()[62] == 0x6f, "");
    ASSERT(b63.value()[63] == 0x7f, "");

    int512_t b64 = int512_t::_impl::shift_right(b6, 64);
    ASSERT(b64.value()[8] == 0x71, "");
    ASSERT(b64.value()[9] == 0xf2, "");
    ASSERT(b64.value()[10] == 0xf3, "");
    ASSERT(b64.value()[11] == 0xf4, "");
    ASSERT(b64.value()[12] == 0xf5, "");
    ASSERT(b64.value()[13] == 0xf6, "");
    ASSERT(b64.value()[14] == 0xf7, "");
    ASSERT(b64.value()[15] == 0xf8, "");
    for (int idx = 16; idx < 63; ++idx) {
        ASSERT(b64.value()[idx] == 0, std::to_string(idx));
    }

    int512_t b65 = int512_t::_impl::shift_right(b6, 256);
    for (int idx = 0; idx < 32; ++idx) {
        ASSERT(b65.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b65.value()[32] == 0x71, "");
    ASSERT(b65.value()[33] == 0xf2, "");
    ASSERT(b65.value()[34] == 0xf3, "");
    ASSERT(b65.value()[35] == 0xf4, "");
    ASSERT(b65.value()[36] == 0xf5, "");
    ASSERT(b65.value()[37] == 0xf6, "");
    ASSERT(b65.value()[38] == 0xf7, "");
    ASSERT(b65.value()[39] == 0xf8, "");
    for (int idx = 40; idx < 63; ++idx) {
        ASSERT(b65.value()[idx] == 0, std::to_string(idx));
    }

    int512_t b66 = int512_t::_impl::shift_right(b6, 260);
    for (int idx = 0; idx < 32; ++idx) {
        ASSERT(b66.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b66.value()[32] == 0x07, "");
    ASSERT(b66.value()[33] == 0x1f, "");
    ASSERT(b66.value()[34] == 0x2f, "");
    ASSERT(b66.value()[35] == 0x3f, "");
    ASSERT(b66.value()[36] == 0x4f, "");
    ASSERT(b66.value()[37] == 0x5f, "");
    ASSERT(b66.value()[38] == 0x6f, "");
    ASSERT(b66.value()[39] == 0x7f, "");
    ASSERT(b66.value()[40] == 0x80, "");
    for (int idx = 41; idx < 63; ++idx) {
        ASSERT(b66.value()[idx] == 0, std::to_string(idx));
    }

    //right int < 0
    int512_t b7 = 0;
    b7.value()[0] = 0xf1;
    b7.value()[1] = 0xf2;
    b7.value()[2] = 0xf3;
    b7.value()[3] = 0xf4;
    b7.value()[4] = 0xf5;
    b7.value()[5] = 0xf6;
    b7.value()[6] = 0xf7;
    b7.value()[7] = 0xf8;
    b7.value()[56] = 0xf1;
    b7.value()[57] = 0xf2;
    b7.value()[58] = 0xf3;
    b7.value()[59] = 0xf4;
    b7.value()[60] = 0xf5;
    b7.value()[61] = 0xf6;
    b7.value()[62] = 0xf7;
    b7.value()[63] = 0xf8;
    int512_t b71 = int512_t::_impl::shift_right(b7, 8);
    ASSERT(b71.value()[0] == 0xff, "");
    ASSERT(b71.value()[1] == 0xf1, "");
    ASSERT(b71.value()[2] == 0xf2, "");
    ASSERT(b71.value()[3] == 0xf3, "");
    ASSERT(b71.value()[4] == 0xf4, "");
    ASSERT(b71.value()[5] == 0xf5, "");
    ASSERT(b71.value()[6] == 0xf6, "");
    ASSERT(b71.value()[7] == 0xf7, "");
    ASSERT(b71.value()[8] == 0xf8, "");
    for (int idx = 9; idx < 57; ++idx) {
        ASSERT(b71.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b71.value()[57] == 0xf1, "");
    ASSERT(b71.value()[58] == 0xf2, "");
    ASSERT(b71.value()[59] == 0xf3, "");
    ASSERT(b71.value()[60] == 0xf4, "");
    ASSERT(b71.value()[61] == 0xf5, "");
    ASSERT(b71.value()[62] == 0xf6, "");
    ASSERT(b71.value()[63] == 0xf7, "");

    int512_t b72 = int512_t::_impl::shift_right(b7, 16);
    ASSERT(b72.value()[0] == 0xff, "");
    ASSERT(b72.value()[1] == 0xff, "");
    ASSERT(b72.value()[2] == 0xf1, "");
    ASSERT(b72.value()[3] == 0xf2, "");
    ASSERT(b72.value()[4] == 0xf3, "");
    ASSERT(b72.value()[5] == 0xf4, "");
    ASSERT(b72.value()[6] == 0xf5, "");
    ASSERT(b72.value()[7] == 0xf6, "");
    ASSERT(b72.value()[8] == 0xf7, "");
    ASSERT(b72.value()[9] == 0xf8, "");
    for (int idx = 10; idx < 58; ++idx) {
        ASSERT(b72.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b72.value()[58] == 0xf1, "");
    ASSERT(b72.value()[59] == 0xf2, "");
    ASSERT(b72.value()[60] == 0xf3, "");
    ASSERT(b72.value()[61] == 0xf4, "");
    ASSERT(b72.value()[62] == 0xf5, "");
    ASSERT(b72.value()[63] == 0xf6, "");

    int512_t b73 = int512_t::_impl::shift_right(b7, 4);
    ASSERT(b73.value()[0] == 0xff, "");
    ASSERT(b73.value()[1] == 0x1f, "");
    ASSERT(b73.value()[2] == 0x2f, "");
    ASSERT(b73.value()[3] == 0x3f, "");
    ASSERT(b73.value()[4] == 0x4f, "");
    ASSERT(b73.value()[5] == 0x5f, "");
    ASSERT(b73.value()[6] == 0x6f, "");
    ASSERT(b73.value()[7] == 0x7f, "");
    ASSERT(b73.value()[8] == 0x80, "");
    for (int idx = 9; idx < 56; ++idx) {
        ASSERT(b73.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b73.value()[56] == 0x0f, "");
    ASSERT(b73.value()[57] == 0x1f, "");
    ASSERT(b73.value()[58] == 0x2f, "");
    ASSERT(b73.value()[59] == 0x3f, "");
    ASSERT(b73.value()[60] == 0x4f, "");
    ASSERT(b73.value()[61] == 0x5f, "");
    ASSERT(b73.value()[62] == 0x6f, "");
    ASSERT(b73.value()[63] == 0x7f, "");

    int512_t b74 = int512_t::_impl::shift_right(b7, 64);
    for (int idx = 0; idx < 7; ++idx) {
        ASSERT(b74.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b74.value()[8] == 0xf1, "");
    ASSERT(b74.value()[9] == 0xf2, "");
    ASSERT(b74.value()[10] == 0xf3, "");
    ASSERT(b74.value()[11] == 0xf4, "");
    ASSERT(b74.value()[12] == 0xf5, "");
    ASSERT(b74.value()[13] == 0xf6, "");
    ASSERT(b74.value()[14] == 0xf7, "");
    ASSERT(b74.value()[15] == 0xf8, "");
    for (int idx = 16; idx < 64; ++idx) {
        ASSERT(b74.value()[idx] == 0, std::to_string(idx));
    }

    int512_t b75 = int512_t::_impl::shift_right(b7, 256);
    for (int idx = 0; idx < 32; ++idx) {
        ASSERT(b75.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b75.value()[32] == 0xf1, "");
    ASSERT(b75.value()[33] == 0xf2, "");
    ASSERT(b75.value()[34] == 0xf3, "");
    ASSERT(b75.value()[35] == 0xf4, "");
    ASSERT(b75.value()[36] == 0xf5, "");
    ASSERT(b75.value()[37] == 0xf6, "");
    ASSERT(b75.value()[38] == 0xf7, "");
    ASSERT(b75.value()[39] == 0xf8, "");
    for (int idx = 40; idx < 63; ++idx) {
        ASSERT(b75.value()[idx] == 0, std::to_string(idx));
    }

    int512_t b76 = int512_t::_impl::shift_right(b7, 260);
    for (int idx = 0; idx < 33; ++idx) {
        ASSERT(b76.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b76.value()[33] == 0x1f, "");
    ASSERT(b76.value()[34] == 0x2f, "");
    ASSERT(b76.value()[35] == 0x3f, "");
    ASSERT(b76.value()[36] == 0x4f, "");
    ASSERT(b76.value()[37] == 0x5f, "");
    ASSERT(b76.value()[38] == 0x6f, "");
    ASSERT(b76.value()[39] == 0x7f, "");
    ASSERT(b76.value()[40] == 0x80, "");
    for (int idx = 41; idx < 63; ++idx) {
        ASSERT(b76.value()[idx] == 0, std::to_string(idx));
    }

    int512_t b8 = -13;
    int512_t b81 = int512_t::_impl::shift_right(b8, 1);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b81.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b81.value()[63] == 0xf9, "");

}

TEST_CASE(bigint, testAssign) {
    uint512_t a1 = 0;
    int512_t b1 = 0;

    a1 = 4;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 4, "");
    b1 = 4;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b1.value()[63] == 4, "");

    a1 = -4;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 0xfc, "");
    b1 = -4;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b1.value()[63] == 0xfc, "");

    a1 = std::uint128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 4, "");
    b1 = std::int128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b1.value()[63] == 4, "");

    a1 = std::uint128_t(-4);
    for (int idx = 0; idx < 48; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    for (int idx = 48; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 0xfc, "");
    b1 = std::int128_t(-4);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b1.value()[63] == 0xfc, "");

    a1 = std::int128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 4, "");
    b1 = std::uint128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b1.value()[63] == 4, "");

    a1 = std::int128_t(-4);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 0xfc, "");
    b1 = std::uint128_t(-4);
    for (int idx = 0; idx < 48; ++idx) {
        ASSERT(b1.value()[idx] == 0, std::to_string(idx));
    }
    for (int idx = 48; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b1.value()[63] == 0xfc, "");

}

TEST_CASE(bigint, testOperatorPlus) {
    uint512_t a1 = 3;
    a1.value()[55] = 1;

    uint512_t a11 = uint512_t::_impl::operator_plus_T(a1, 5);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a11.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a11.value()[63] == 8, "");

    uint512_t a12 = uint512_t::_impl::operator_plus_T(a1, 0);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a12.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a12.value()[63] == 3, "");

    uint512_t a14 = uint512_t::_impl::operator_plus_T(a1, std::numeric_limits<uint64_t>::max());
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a14.value()[55] == 2, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a14.value()[63] == 2, "");

    int512_t b1 = 3;
    b1.value()[55] = 1;

    int512_t b11 = int512_t::_impl::operator_plus_T(b1, 5);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b11.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b11.value()[63] == 8, "");

    int512_t b12 = int512_t::_impl::operator_plus_T(b1, 0);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b12.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b12.value()[63] == 3, "");

    int512_t b2 = -3;
    b2.value()[55] = -2;

    int512_t b22 = int512_t::_impl::operator_plus_T(b2, 0);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b22.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b22.value()[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b22.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b22.value()[63] == 0xfd, "");

    int512_t b23 = int512_t::_impl::operator_plus_T(b2, uint16_t(5));
    for (int idx = 0; idx < 56; ++idx) {
        ASSERT(b23.value()[idx] == 0xff, std::to_string(idx));
    }
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b23.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b23.value()[63] == 2, "");

    int512_t b3 = -3;

    int512_t b31 = int512_t::_impl::operator_plus_T(b3, 5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b31.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(static_cast<int64_t>(b31.value()[63]) == 2, "");

    int512_t b4 = -1;
    b4.value()[55] = -2;
    b4.value()[63] = 3;

    int512_t b41 = int512_t::_impl::operator_plus_T(b4, 5);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b41.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b41.value()[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b41.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b41.value()[63] == 8, "");
}

TEST_CASE(bigint, testOperatorTilda) {
    uint512_t a1 = 250;
    a1.value()[3] = 17;

    uint512_t a2 = uint512_t::_impl::operator_unary_tilda(a1);
    for (int idx = 0; idx < 3; ++idx) {
        ASSERT(a2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a2.value()[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a2.value()[63] == 0x05, "");

    int512_t b1 = 250;
    b1.value()[3] = 17;

    int512_t b2 = int512_t::_impl::operator_unary_tilda(b1);
    for (int idx = 0; idx < 3; ++idx) {
        ASSERT(b2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b2.value()[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        ASSERT(b2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b2.value()[63] == 0x05, "");

    int512_t b3 = -250;
    b3.value()[3] = 17;

    int512_t b4 = int512_t::_impl::operator_unary_tilda(b3);
    for (int idx = 0; idx < 3; ++idx) {
        ASSERT(b4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b4.value()[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        ASSERT(b4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b4.value()[63] == 0xf9, "");
}

TEST_CASE(bigint, testOperatorUnaryMinus) {
    uint512_t a1 = 250;
    a1.value()[3] = 17;

    uint512_t a2 = uint512_t::_impl::operator_unary_minus(a1);
    for (int idx = 0; idx < 3; ++idx) {
        ASSERT(a2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a2.value()[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a2.value()[63] == 0x06, "");

    int512_t b1 = 250;
    b1.value()[3] = 17;

    int512_t b2 = int512_t::_impl::operator_unary_minus(b1);
    for (int idx = 0; idx < 3; ++idx) {
        ASSERT(b2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b2.value()[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        ASSERT(b2.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b2.value()[63] == 0x06, "");

    int512_t b3 = -250;
    b3.value()[3] = 17;

    int512_t b4 = int512_t::_impl::operator_unary_minus(b3);
    for (int idx = 0; idx < 3; ++idx) {
        ASSERT(b4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b4.value()[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        ASSERT(b4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b4.value()[63] == 0xfa, "");
}

TEST_CASE(bigint, testOperatorPlusWide) {
    uint512_t a1 = 3;
    a1.value()[55] = 1;

    uint512_t a11 = uint512_t::_impl::operator_plus(a1, a1);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a11.value()[55] == 2, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a11.value()[63] == 6, "");

    uint512_t a12 = uint512_t::_impl::operator_plus(a1, uint256_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a12.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a12.value()[63] == 3, "");

    uint512_t a13 = uint512_t::_impl::operator_minus(a1, std::uint128_t(5));
    for (int idx = 0; idx < 56; ++idx) {
        ASSERT(a13.value()[idx] == 0, std::to_string(idx));
    }
    for (int idx = 57; idx < 63; ++idx) {
        ASSERT(a13.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a13.value()[63] == 0xfe, "");

    uint512_t a14 = uint512_t::_impl::operator_plus(a1, uint512_t::_impl::operator_plus_T(a1, std::numeric_limits<uint64_t>::max()));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a14.value()[55] == 3, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a14.value()[63] == 5, "");

    uint512_t a15 = uint512_t::_impl::operator_minus(a1, uint256_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a15.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a15.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a15.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a15.value()[63] == 3, "");

    int512_t b1 = 3;
    b1.value()[55] = 1;

    int512_t b11 = int512_t::_impl::operator_plus(b1, std::uint128_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b11.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b11.value()[63] == 8, "");

    int512_t b12 = int512_t::_impl::operator_plus(b1, uint512_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b12.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b12.value()[63] == 3, "");

    int512_t b13 = int512_t::_impl::operator_minus(b1, std::uint128_t(5));
    for (int idx = 0; idx < 56; ++idx) {
        ASSERT(b13.value()[idx] == 0, std::to_string(idx));
    }
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b13.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b13.value()[63] == 0xfe, "");

    int512_t b14 = int512_t::_impl::operator_minus(b1, std::int128_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b14.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b14.value()[63] == 3, "");

    int512_t b2 = -3;
    b2.value()[55] = -2;

    int512_t b21 = int512_t::_impl::operator_minus(b2, std::uint128_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b21.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b21.value()[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b21.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(static_cast<int8_t>(b21.value()[63]) == -8, "");

    int512_t b22 = int512_t::_impl::operator_plus(b2, int256_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b22.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b22.value()[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b22.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(static_cast<int8_t>(b22.value()[63]) == -3, "");

    int512_t b23 = int512_t::_impl::operator_plus(b2, std::uint128_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b23.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b23.value()[55] == 0xff, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b23.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(b23.value()[63] == 2, "");

    int512_t b3 = -3;

    int512_t b31 = int512_t::_impl::operator_plus(b3, std::int128_t(5));
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(b31.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(static_cast<int8_t>(b31.value()[63]) == 2, "");

    int512_t b4 = -1;
    b4.value()[55] = -2;
    b4.value()[63] = 3;

    int512_t b41 = int512_t::_impl::operator_plus(b4, uint256_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(b41.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(b41.value()[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(b41.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(static_cast<int8_t>(b41.value()[63]) == 8, "");

}
TEST_CASE(bigint, testOperatorStar) {
    uint512_t a1 = 256_uint512;

    uint512_t a11 = uint512_t::_impl::operator_star(a1, uint512_t(1));
    for (int idx = 0; idx < 62; ++idx) {
        ASSERT(a11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a11.value()[62] == 1, "");
    ASSERT(a11.value()[63] == 0, "");

    uint512_t a12 = uint512_t::_impl::operator_star(a1, uint512_t(0));
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a12.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a13 = uint512_t::_impl::operator_star(a1, uint512_t(2));
    for (int idx = 0; idx < 62; ++idx) {
        ASSERT(a13.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a13.value()[62] == 2, "");
    ASSERT(a13.value()[63] == 0, "");

    uint512_t a2 = std::numeric_limits<uint64_t>::max();
    uint512_t a24 = uint512_t::_impl::operator_star(a2, uint512_t(2));
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a24.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a24.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a24.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a24.value()[63] == 0xfe, "");

    uint512_t a3 = 256;

    uint512_t a31 = uint512_t::_impl::operator_star(a3, 1);
    for (int idx = 0; idx < 62; ++idx) {
        ASSERT(a31.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a31.value()[62] == 1, "");
    ASSERT(a31.value()[63] == 0, "");

    uint512_t a32 = uint512_t::_impl::operator_star(a3, 0);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a32.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a33 = uint512_t::_impl::operator_star(a3, 2);
    for (int idx = 0; idx < 62; ++idx) {
        ASSERT(a33.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a33.value()[62] == 2, "");
    ASSERT(a33.value()[63] == 0, "");

    uint512_t a4 = std::numeric_limits<uint64_t>::max();
    uint512_t a44 = uint512_t::_impl::operator_star(a4, 2);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a44.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a44.value()[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        ASSERT(a44.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a44.value()[63] == 0xfe, "");

}
TEST_CASE(bigint, testOperatorMore) {
    ASSERT(int512_t::_impl::operator_more(15, -18), "");
    ASSERT(!uint512_t::_impl::operator_more(15, 18U), "");
    ASSERT(uint512_t::_impl::operator_more(18, 15U), "");
    ASSERT(!uint512_t::_impl::operator_more(15, 18U), "");
    ASSERT(int512_t::_impl::operator_more(-15, -18), "");
    ASSERT(!int512_t::_impl::operator_more(-18, -15), "");
}

TEST_CASE(bigint, testOperatorEq) {
    ASSERT(uint512_t::_impl::operator_eq(uint512_t(12), int64_t(12)), "");
    ASSERT(uint512_t::_impl::operator_eq(12, 12), "");
    ASSERT(uint512_t::_impl::operator_eq(0, 0), "");
}

TEST_CASE(bigint, testOperatorPipe) {
    uint512_t a1 = 0x0102030405060708;
    for (int i = 0; i < 7; ++i) {
        a1.value()[i] = 0x01;
    }
    uint512_t a2 = uint512_t::_impl::operator_pipe(a1, 0xf0f0f0f0f0f0f0f0);

    for (int i = 0; i < 7; ++i) {
        ASSERT(a2.value()[i] == 0x01, " " + std::to_string(a2.value()[i]));
    }
    for (int idx = 8; idx < 56; ++idx) {
        ASSERT(a2.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a2.value()[56] == 0xf1, "");
    ASSERT(a2.value()[57] == 0xf2, "");
    ASSERT(a2.value()[58] == 0xf3, "");
    ASSERT(a2.value()[59] == 0xf4, "");
    ASSERT(a2.value()[60] == 0xf5, "");
    ASSERT(a2.value()[61] == 0xf6, "");
    ASSERT(a2.value()[62] == 0xf7, "");
    ASSERT(a2.value()[63] == 0xf8, "");

    uint512_t a3 = 0xf0f0f0f0f0f0f0f0;
    for (int i = 0; i < 7; ++i) {
        a3.value()[i] = 0xf0;
    }

    uint512_t a4 = uint512_t::_impl::operator_pipe(a1, a3);

    for (int i = 0; i < 7; ++i) {
        ASSERT(a4.value()[i] == 0xf1, "");
    }
    for (int idx = 8; idx < 56; ++idx) {
        ASSERT(a4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a4.value()[56] == 0xf1, "");
    ASSERT(a4.value()[57] == 0xf2, "");
    ASSERT(a4.value()[58] == 0xf3, "");
    ASSERT(a4.value()[59] == 0xf4, "");
    ASSERT(a4.value()[60] == 0xf5, "");
    ASSERT(a4.value()[61] == 0xf6, "");
    ASSERT(a4.value()[62] == 0xf7, "");
    ASSERT(a4.value()[63] == 0xf8, "");
}

TEST_CASE(bigint, testOperatorAmp) {
    uint512_t a1 = 0x0102030405060708;
    for (int i = 0; i < 7; ++i) {
        a1.value()[i] = 0x01;
    }
    uint512_t a2 = uint512_t::_impl::operator_amp(a1, 0xf0f0f0f0f0f0f0f0);

    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a3 = 0xffffffffffffffff;
    for (int i = 0; i < 7; ++i) {
        a2.value()[i] = 0xf0;
    }

    uint512_t a4 = uint512_t::_impl::operator_amp(a1, a3);

    for (int idx = 0; idx < 56; ++idx) {
        ASSERT(a4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a4.value()[56] == 0x01, "");
    ASSERT(a4.value()[57] == 0x02, "");
    ASSERT(a4.value()[58] == 0x03, "");
    ASSERT(a4.value()[59] == 0x04, "");
    ASSERT(a4.value()[60] == 0x05, "");
    ASSERT(a4.value()[61] == 0x06, "");
    ASSERT(a4.value()[62] == 0x07, "");
    ASSERT(a4.value()[63] == 0x08, "");
}

TEST_CASE(bigint, testOperatorSlash) {
    uint512_t a1 = uint512_t::_impl::operator_slash(15, 3);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 5, "");

    uint512_t a2 = uint512_t::_impl::operator_slash(17, 3);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a2.value()[63] == 5, "");

    uint512_t a3 = uint512_t::_impl::operator_slash(0, 3);
    for (int idx = 0; idx < 64; ++idx) {
        ASSERT(a3.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a4 = 2048;
    a4.value()[55] = 2;
    uint512_t a41 = uint512_t::_impl::operator_slash(a4, 2);
    for (int idx = 0; idx < 55; ++idx) {
        ASSERT(a41.value()[idx] == 0, std::to_string(idx));
    }
//    ASSERT(a41.value()[55] ==2, a41.value()[55]);
    for (int idx = 56; idx < 62; ++idx) {
        ASSERT(a41.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a41.value()[62] == 4, "");
    ASSERT(a41.value()[63] == 0, "");

    ASSERT((int256_t(-6) / int256_t(-2)) == int256_t(3), "");

}
TEST_CASE(bigint, testOperatorPercent) {
    uint512_t a1 = uint512_t::_impl::operator_percent(15, 3);
    for (int idx = 0; idx < 64; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a2 = uint512_t::_impl::operator_percent(17, 3);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a2.value()[63] == 2, "");

    uint512_t a3 = uint512_t::_impl::operator_percent(0, 3);
    for (int idx = 0; idx < 64; ++idx) {
        ASSERT(a3.value()[idx] == 0, std::to_string(idx));
    }

    uint512_t a4 = 2049;
    a4.value()[55] = 2;
    uint512_t a41 = uint512_t::_impl::operator_percent(a4, 2);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a41.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a41.value()[63] == 1, "");

    ASSERT(int256_t(-14) % int256_t(10) == int256_t(-4), "");
}
TEST_CASE(bigint, testCircumflex) {
    uint512_t a1 = 0xff;

    uint512_t a2 = uint512_t::_impl::operator_circumflex(a1, 0xf);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a2.value()[63] == 0xf0, "");
}
TEST_CASE(bigint, testToString) {
    ASSERT(to_string(uint512_t(123455)) == std::string("123455"), "");
    ASSERT(to_string(int512_t(-123455)) == std::string("-123455"), "");

    ASSERT(to_wstring(uint512_t(123455)) == std::wstring(L"123455"), "");
    ASSERT(to_wstring(int512_t(-123455)) == std::wstring(L"-123455"), "");
}
TEST_CASE(bigint, testCast) {
    ASSERT(int(uint512_t(18)) == 18, "");
    ASSERT(double(uint512_t(1024)) == 1024, "");

    std::uint128_t a1 = uint512_t(18);
    for (int idx = 0; idx < 15; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a1.value()[15] == 18, "");

    uint512_t a2 = 1024;
    ASSERT(a2 == 1024, "");
    uint32_t a3 = a2;
    ASSERT(a3 == 1024, "");

    ASSERT(int(int512_t(-1024)) == -1024, "");
    ASSERT(double(int512_t(-1024)) == -1024, "");
    ASSERT(double(-int512_t(1024)) == -1024, "");
    ASSERT(-double(int512_t(1024)) == -1024, "");
}
TEST_CASE(bigint, testFromString) {
    auto a1 = uint512_t::_impl::from_str("1234567");
    ASSERT(std::to_string(a1) == "1234567", "");
    for (int idx = 0; idx < 61; ++idx) {
        ASSERT(a1.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a1.value()[61] == 0x12, "");
    ASSERT(a1.value()[62] == 0xd6, "");
    ASSERT(a1.value()[63] == 0x87, "");

    std::string s = "1000000000000000000";
    std::istringstream in(s);
    in >> a1;
    ASSERT(a1 == 1000000000000000000_int256, "");

    std::wstring ws = L"7000000000000000000";
    std::wistringstream win(ws);
    win >> a1;
    ASSERT(a1 == 7000000000000000000_int256, "");
}
TEST_CASE(bigint, testNativeOperators) {
    auto a1 = ~uint512_t(0xff);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a1.value()[idx] == 0xff, std::to_string(idx));
    }
    ASSERT(a1.value()[63] == 0x00, "");

    auto a2 = -uint512_t(0x1);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a2.value()[idx] == 0xff, std::to_string(idx));
    }

    auto a3 = 2 * uint512_t(0x1);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a3.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a3.value()[63] == 2, "");

    auto a4 = uint512_t(0x1) * 2;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a4.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a4.value()[63] == 2, "");

    auto a5 = 4 / uint512_t(0x2);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a5.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a5.value()[63] == 2, "");

    auto a6 = uint512_t(0x4) / 2;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a6.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a6.value()[63] == 2, "");

    auto a7 = 8 % uint512_t(0x5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a7.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a7.value()[63] == 3, "");

    auto a8 = uint512_t(0x8) % 5;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a8.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a8.value()[63] == 3, "");

    auto a9 = 8 + uint512_t(0x5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a9.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a9.value()[63] == 13, "");

    auto a10 = uint512_t(0x8) + 5;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a10.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a10.value()[63] == 13, "");

    auto a11 = 8 - uint512_t(0x5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a11.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a11.value()[63] == 3, "");

    auto a12 = uint512_t(0x8) - 5;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a12.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a12.value()[63] == 3, "");

    ASSERT(uint512_t(15) < 20U, "");
    ASSERT(uint512_t(20) > 15U, "");
    ASSERT(uint512_t(15) <= 20U, "");
    ASSERT(uint512_t(20) >= 15U, "");
    ASSERT(uint512_t(20) <= 20U, "");
    ASSERT(uint512_t(20) >= 20U, "");
    ASSERT(uint512_t(20) == 20U, "");

    ASSERT(15U < uint512_t(20), "");
    ASSERT(20U > uint512_t(15), "");
    ASSERT(15U <= uint512_t(20), "");
    ASSERT(20U >= uint512_t(15), "");
    ASSERT(20U <= uint512_t(20), "");
    ASSERT(20U >= uint512_t(20), "");
    ASSERT(20U == uint512_t(20), "");

    ASSERT(uint512_t(15) < std::uint128_t(20), "");
    ASSERT(uint512_t(20) > std::uint128_t(15), "");
    ASSERT(uint512_t(15) <= std::uint128_t(20), "");
    ASSERT(uint512_t(20) >= std::uint128_t(15), "");
    ASSERT(uint512_t(20) <= std::uint128_t(20), "");
    ASSERT(uint512_t(20) >= std::uint128_t(20), "");
    ASSERT(uint512_t(20) == std::uint128_t(20), "");

    ASSERT(std::uint128_t(15) < uint512_t(20), "");
    ASSERT(std::uint128_t(20) > uint512_t(15), "");
    ASSERT(std::uint128_t(15) <= uint512_t(20), "");
    ASSERT(std::uint128_t(20) >= uint512_t(15), "");
    ASSERT(std::uint128_t(20) <= uint512_t(20), "");
    ASSERT(std::uint128_t(20) >= uint512_t(20), "");
    ASSERT(std::uint128_t(20) == uint512_t(20), "");

    auto a13 = 9 & uint512_t(5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a13.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a13.value()[63] == 1, "");

    auto a14 = uint512_t(9) & 5;

    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a14.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a14.value()[63] == 1, "");

    auto a15 = 9 | uint512_t(5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a15.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a15.value()[63] == 13, "");

    auto a16 = uint512_t(9) | 5;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a16.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a16.value()[63] == 13, "");

    auto a17 = 4 ^ uint512_t(5);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a17.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a17.value()[63] == 1, "");

    auto a18 = uint512_t(4) ^ 5;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a18.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a18.value()[63] == 1, "");

    auto a19 = uint512_t(8) << 1;
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a19.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a19.value()[63] == 16, "");

    auto a20 = uint512_t(8) >> 1;

    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a20.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a20.value()[63] == 4, "");

    ASSERT(!uint256_t(0), "");
    ASSERT(uint256_t(1), "");

    auto a21 = +uint512_t(0x1);
    for (int idx = 0; idx < 63; ++idx) {
        ASSERT(a21.value()[idx] == 0, std::to_string(idx));
    }
    ASSERT(a21.value()[63] == 1, "");

    ASSERT(1024_int128 * 2.0 == 2048, "");
    ASSERT(2.0 * 1024_int128 == 2048, "");

    std::int128_t b = -123;
    ASSERT(b < 0, "");

    uint256_t c1 = 100000000000000000000_uint256;
    uint256_t c2 = 100000000000000000000_uint256;
    uint256_t c3 = c1 * c2;
    ASSERT(c3 == 10000000000000000000000000000000000000000_uint256, "");
}
TEST_CASE(bigint, testNativeOperatorsAssign) {
    uint512_t a1 = 18;

    a1 *= 2U;
    ASSERT(a1 == 36U, "");

    a1 /= 2U;
    ASSERT(a1 == 18U, "");

    a1 %= 5U;
    ASSERT(a1 == 3U, "");

    a1 += 2U;
    ASSERT(a1 == 5U, "");

    a1 -= 1U;
    ASSERT(a1 == 4U, "");

    a1 &= 3U;
    ASSERT(a1 == 0U, "");

    a1 |= 255U;
    ASSERT(a1 == 255U, "");

    a1 ^= 0xffffffffffffffff;
    ASSERT(a1 == 0xffffffffffffff00, "");

    a1 <<= 4;
    ASSERT(a1 == 0xffffffffffffff000_uint128, "");

    a1 >>= 8;
    ASSERT(a1 == 0x0ffffffffffffff0, "");
}
TEST_CASE(bigint, testNativeOperatorsAssignFloat) {
    uint512_t a1 = 18;

    a1 *= 2.5;
    ASSERT(a1 == 45, "");

    a1 /= 2.5;
    ASSERT(a1 == 18U, "");

    a1 += 2.9;
    ASSERT(a1 == 20, "");

    a1 -= 1.9;
    ASSERT(a1 == 18, "");
}
TEST_CASE(bigint, testConstexpr) {
    constexpr uint256_t a0{};
    static_assert(a0 == 0, "");
    static_assert(a0 != 1, "");
    static_assert(1 != a0, "");
    static_assert(a0 >= 0U, "");
    static_assert(a0 < 1U, "");
    static_assert(a0 <= 1U, "");

    constexpr uint512_t a1 = 7;
    static_assert(a1 > 0U, "");
    static_assert(a1 == 7, "");
    static_assert(a1 + 12 == 19, "");
    static_assert(12 + a1 == 19, "");

    constexpr std::int128_t a2 = a1;
    static_assert(a2 == 7, "");
    static_assert(a2 - 3 == 4, "");
    static_assert(11 - a2 == 4, "");

    static_assert(a1 / 2 == 3, "");
    static_assert(a2 * 2 == 14, "");
    static_assert(a1 % 2 == 1, "");

    static_assert(~a2 == -8, "");
    static_assert(-a2 == -7, "");

    static_assert((a1 | 0xff) == 0xff, "");
    static_assert((a1 & 0xff) == 7, "");
    static_assert((a2 ^ 0xff) == 0xf8, "");

    static_assert(0xff_int128 == 255, "");
    static_assert(255_int128 == 255, "");

    static_assert((a1 >> 1) == 3, "");
    static_assert((a1 << 1) == 14, "");

    constexpr uint256_t b = std::numeric_limits<uint256_t>::min();
    static_assert(b == 0, "");

    constexpr std::int128_t c = std::numeric_limits<std::int128_t>::min();
    static_assert(c == 0x80000000000000000000000000000000_uint128, "");

    static_assert(0x80000000000000000000000000000000_uint128, "");
}
TEST_CASE(bigint, testToChars) {
    std::uint128_t a = 65535;
    std::int128_t b = -65535;
    std::uint128_t c = 0;

    char arr1[1];
    std::to_chars_result res = to_chars(arr1, arr1 + sizeof(arr1), a, 1);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, "");
    res = to_chars(arr1, arr1 + sizeof(arr1), a, 50);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, "");
    res = to_chars(arr1 + sizeof(arr1), arr1, a);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, "");

    res = to_chars(arr1, arr1 + sizeof(arr1), a);
    ASSERT(res.ec.value() == (int)std::errc::value_too_large, "");

    char arr2[5];
    res = to_chars(arr2, arr2 + sizeof(arr2), a);
    ASSERT(!res.ec, "");
    res = to_chars(arr2, arr2 + sizeof(arr2), b);
    ASSERT(res.ec.value() == (int)std::errc::value_too_large, "");
}
#define checkFromChars(T,  expect,  base,  str) { \
        T num = 0; \
        std::from_chars_result res = from_chars(str.data(), str.data() + str.size(), num, base); \
        ASSERT(!res.ec, res.ec.message()); \
        ASSERT(expect == num, \
               std::to_string(expect) + " " + std::to_string(base) + " " + std::to_string(num)); \
    }


TEST_CASE(bigint, testFromChars) {
    std::uint128_t a = 0;
    std::int128_t b = 0;

    char arr1[] = "65535";
    std::from_chars_result res = from_chars(arr1, arr1 + sizeof(arr1), a, 1);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());
    res = from_chars(arr1, arr1 + sizeof(arr1), a, 50);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());
    res = from_chars(arr1 + sizeof(arr1), arr1, a);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());

    char arr2[] = "-65535";
    res = from_chars(arr2, arr2 + sizeof(arr2), a);
    ASSERT(res.ec.value() == (int)std::errc::result_out_of_range, res.ec.message());

    char arr3[] = "-";
    res = from_chars(arr3, arr3 + 1, b);
    ASSERT(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());

    checkFromChars(std::uint128_t, 65535, 2, std::string("1111111111111111"));
    checkFromChars(std::uint128_t, 65535, 3, std::string("10022220020"));
    checkFromChars(std::uint128_t, 65535, 4, std::string("33333333"));
    checkFromChars(std::uint128_t, 65535, 5, std::string("4044120"));
    checkFromChars(std::uint128_t, 65535, 6, std::string("1223223"));
    checkFromChars(std::uint128_t,  65535, 7, std::string("362031"));
    checkFromChars(std::uint128_t,  65535, 8, std::string("177777"));
    checkFromChars(std::uint128_t,  65535, 9, std::string("108806"));
    checkFromChars(std::uint128_t,  65535, 10, std::string("65535"));
    checkFromChars(std::uint128_t,  65535, 11, std::string("45268"));
    checkFromChars(std::uint128_t,  65535, 12, std::string("31b13"));
    checkFromChars(std::uint128_t,  65535, 13, std::string("23aa2"));
    checkFromChars(std::uint128_t,  65535, 14, std::string("19c51"));
    checkFromChars(std::uint128_t,  65535, 15, std::string("14640"));
    checkFromChars(std::uint128_t,  65535, 16, std::string("ffff"));
    checkFromChars(std::uint128_t,  65535, 17, std::string("d5d0"));
    checkFromChars(std::uint128_t,  65535, 18, std::string("b44f"));
    checkFromChars(std::uint128_t,  65535, 19, std::string("9aa4"));
    checkFromChars(std::uint128_t,  65535, 20, std::string("83gf"));
    checkFromChars(std::uint128_t,  65535, 21, std::string("71cf"));
    checkFromChars(std::uint128_t,  65535, 22, std::string("638j"));
    checkFromChars(std::uint128_t,  65535, 23, std::string("58k8"));
    checkFromChars(std::uint128_t,  65535, 24, std::string("4hif"));
    checkFromChars(std::uint128_t,  65535, 25, std::string("44la"));
    checkFromChars(std::uint128_t,  65535, 26, std::string("3iof"));
    checkFromChars(std::uint128_t,  65535, 27, std::string("38o6"));
    checkFromChars(std::uint128_t,  65535, 28, std::string("2rgf"));
    checkFromChars(std::uint128_t,  65535, 29, std::string("2jqo"));
    checkFromChars(std::uint128_t,  65535, 30, std::string("2cof"));
    checkFromChars(std::uint128_t,  65535, 31, std::string("2661"));
    checkFromChars(std::uint128_t,  65535, 32, std::string("1vvv"));
    checkFromChars(std::uint128_t,  65535, 33, std::string("1r5u"));
    checkFromChars(std::uint128_t,  65535, 34, std::string("1mnh"));
    checkFromChars(std::uint128_t,  65535, 35, std::string("1ihf"));
    checkFromChars(std::uint128_t,  65535, 36, std::string("1ekf"));

    checkFromChars(std::int128_t,  -65535, 2, std::string("-1111111111111111"));
    checkFromChars(std::int128_t,  -65535, 3, std::string("-10022220020"));
    checkFromChars(std::int128_t,  -65535, 4, std::string("-33333333"));
    checkFromChars(std::int128_t,  -65535, 5, std::string("-4044120"));
    checkFromChars(std::int128_t,  -65535, 6, std::string("-1223223"));
    checkFromChars(std::int128_t,  -65535, 7, std::string("-362031"));
    checkFromChars(std::int128_t,  -65535, 8, std::string("-177777"));
    checkFromChars(std::int128_t,  -65535, 9, std::string("-108806"));
    checkFromChars(std::int128_t,  -65535, 10, std::string("-65535"));
    checkFromChars(std::int128_t,  -65535, 11, std::string("-45268"));
    checkFromChars(std::int128_t,  -65535, 12, std::string("-31b13"));
    checkFromChars(std::int128_t,  -65535, 13, std::string("-23aa2"));
    checkFromChars(std::int128_t,  -65535, 14, std::string("-19c51"));
    checkFromChars(std::int128_t,  -65535, 15, std::string("-14640"));
    checkFromChars(std::int128_t,  -65535, 16, std::string("-ffff"));
    checkFromChars(std::int128_t,  -65535, 17, std::string("-d5d0"));
    checkFromChars(std::int128_t,  -65535, 18, std::string("-b44f"));
    checkFromChars(std::int128_t,  -65535, 19, std::string("-9aa4"));
    checkFromChars(std::int128_t,  -65535, 20, std::string("-83gf"));
    checkFromChars(std::int128_t,  -65535, 21, std::string("-71cf"));
    checkFromChars(std::int128_t,  -65535, 22, std::string("-638j"));
    checkFromChars(std::int128_t,  -65535, 23, std::string("-58k8"));
    checkFromChars(std::int128_t,  -65535, 24, std::string("-4hif"));
    checkFromChars(std::int128_t,  -65535, 25, std::string("-44la"));
    checkFromChars(std::int128_t,  -65535, 26, std::string("-3iof"));
    checkFromChars(std::int128_t,  -65535, 27, std::string("-38o6"));
    checkFromChars(std::int128_t,  -65535, 28, std::string("-2rgf"));
    checkFromChars(std::int128_t,  -65535, 29, std::string("-2jqo"));
    checkFromChars(std::int128_t,  -65535, 30, std::string("-2cof"));
    checkFromChars(std::int128_t,  -65535, 31, std::string("-2661"));
    checkFromChars(std::int128_t,  -65535, 32, std::string("-1vvv"));
    checkFromChars(std::int128_t,  -65535, 33, std::string("-1r5u"));
    checkFromChars(std::int128_t,  -65535, 34, std::string("-1mnh"));
    checkFromChars(std::int128_t,  -65535, 35, std::string("-1ihf"));
    checkFromChars(std::int128_t,  -65535, 36, std::string("-1ekf"));

    for (int i = 2; i <= 36; ++i) {
        checkFromChars(std::uint128_t, 0, i, std::string("0"));
    }
}
TEST_CASE(bigint, testNegative) {
    ASSERT(int256_t(-3) * int256_t(-2) == (-3) * (-2), "");
    ASSERT(int256_t(3) * int256_t(-2) == (3) * (-2), "");
    ASSERT(int256_t(-3) * int256_t(2) == (-3) * (2), "");
    ASSERT(int256_t(3) * int256_t(2) == (3) * (2), "");

    ASSERT(int256_t(-3) / int256_t(-2) == (-3) / (-2), "");
    ASSERT(int256_t(3) / int256_t(-2) == (3) / (-2), "");
    ASSERT(int256_t(-3) / int256_t(2) == (-3) / (2), "");
    ASSERT(int256_t(3) / int256_t(2) == (3) / (2), "");

    ASSERT(int256_t(-3) % int256_t(-2) == (-3) % (-2), "");
    ASSERT(int256_t(3) % int256_t(-2) == (3) % (-2), "");
    ASSERT(int256_t(-3) % int256_t(2) == (-3) % (2), "");
    ASSERT(int256_t(3) % int256_t(2) == (3) % (2), "");

    ASSERT(int256_t(-3) + int256_t(-2) == (-3) + (-2), "");
    ASSERT(int256_t(3) + int256_t(-2) == (3) + (-2), "");
    ASSERT(int256_t(-3) + int256_t(2) == (-3) + (2), "");
    ASSERT(int256_t(3) + int256_t(2) == (3) + (2), "");

    ASSERT(int256_t(-3) - int256_t(-2) == (-3) - (-2), "");
    ASSERT(int256_t(3) - int256_t(-2) == (3) - (-2), "");
    ASSERT(int256_t(-3) - int256_t(2) == (-3) - (2), "");
    ASSERT(int256_t(3) - int256_t(2) == (3) - (2), "");

    ASSERT((int256_t(-3) == int256_t(-2)) == ((-3) == (-2)), "");
    ASSERT((int256_t(3) == int256_t(-2)) == ((3) == (-2)), "");
    ASSERT((int256_t(-3) == int256_t(2)) == ((-3) == (2)), "");
    ASSERT((int256_t(3) == int256_t(2)) == ((3) == (2)), "");

    ASSERT((int256_t(-2) == int256_t(-2)) == ((-2) == (-2)), "");
    ASSERT((int256_t(2) == int256_t(-2)) == ((2) == (-2)), "");
    ASSERT((int256_t(-2) == int256_t(2)) == ((-2) == (2)), "");
    ASSERT((int256_t(2) == int256_t(2)) == ((2) == (2)), "");

    ASSERT((-int256_t(-2) == -int256_t(-2)) == (-(-2) == -(-2)), "");
    ASSERT((-int256_t(2) == -int256_t(-2)) == (-(2) == -(-2)), "");
    ASSERT((-int256_t(-2) == -int256_t(2)) == (-(-2) == -(2)), "");
    ASSERT((-int256_t(2) == -int256_t(2)) == (-(2) == -(2)), "");

    ASSERT((-int256_t(0) == int256_t(0)) == (-(0) == (0)), "");
    ASSERT((int256_t(0) == -int256_t(0)) == ((0) == -(0)), "");

    ASSERT((int256_t(-3) <= int256_t(-2)) == ((-3) <= (-2)), "");
    ASSERT((int256_t(3) <= int256_t(-2)) == ((3) <= (-2)), "");
    ASSERT((int256_t(-3) <= int256_t(2)) == ((-3) <= (2)), "");
    ASSERT((int256_t(3) <= int256_t(2)) == ((3) <= (2)), "");

    ASSERT((int256_t(-2) <= int256_t(-2)) == ((-2) <= (-2)), "");
    ASSERT((int256_t(2) <= int256_t(-2)) == ((2) <= (-2)), "");
    ASSERT((int256_t(-2) <= int256_t(2)) == ((-2) <= (2)), "");
    ASSERT((int256_t(2) <= int256_t(2)) == ((2) <= (2)), "");

    ASSERT((int256_t(-3) < int256_t(-2)) == ((-3) < (-2)), "");
    ASSERT((int256_t(3) < int256_t(-2)) == ((3) < (-2)), "");
    ASSERT((int256_t(-3) < int256_t(2)) == ((-3) < (2)), "");
    ASSERT((int256_t(3) < int256_t(2)) == ((3) < (2)), "");

    ASSERT((int256_t(-2) < int256_t(-2)) == ((-2) < (-2)), "");
    ASSERT((int256_t(2) < int256_t(-2)) == ((2) < (-2)), "");
    ASSERT((int256_t(-2) < int256_t(2)) == ((-2) < (2)), "");
    ASSERT((int256_t(2) < int256_t(2)) == ((2) < (2)), "");

    ASSERT((int256_t(-3) >= int256_t(-2)) == ((-3) >= (-2)), "");
    ASSERT((int256_t(3) >= int256_t(-2)) == ((3) >= (-2)), "");
    ASSERT((int256_t(-3) >= int256_t(2)) == ((-3) >= (2)), "");
    ASSERT((int256_t(3) >= int256_t(2)) == ((3) >= (2)), "");

    ASSERT((int256_t(-2) >= int256_t(-2)) == ((-2) >= (-2)), "");
    ASSERT((int256_t(2) >= int256_t(-2)) == ((2) >= (-2)), "");
    ASSERT((int256_t(-2) >= int256_t(2)) == ((-2) >= (2)), "");
    ASSERT((int256_t(2) >= int256_t(2)) == ((2) >= (2)), "");

    ASSERT((int256_t(-3) > int256_t(-2)) == ((-3) > (-2)), "");
    ASSERT((int256_t(3) > int256_t(-2)) == ((3) > (-2)), "");
    ASSERT((int256_t(-3) > int256_t(2)) == ((-3) > (2)), "");
    ASSERT((int256_t(3) > int256_t(2)) == ((3) > (2)), "");

    ASSERT((int256_t(-2) > int256_t(-2)) == ((-2) > (-2)), "");
    ASSERT((int256_t(2) > int256_t(-2)) == ((2) > (-2)), "");
    ASSERT((int256_t(-2) > int256_t(2)) == ((-2) > (2)), "");
    ASSERT((int256_t(2) > int256_t(2)) == ((2) > (2)), "");

}
TEST_CASE(bigint, testUnaryIncOrDecr) {
    int512_t a1 = 0;
    ASSERT(++a1 == 1, "");
    ASSERT(a1 == 1, "");

    ASSERT(a1++ == 1, "");
    ASSERT(a1 == 2, "");

    ASSERT(--a1 == 1, "");
    ASSERT(a1 == 1, "");

    ASSERT(a1-- == 1, "");
    ASSERT(a1 == 0, "");

    uint512_t b1 = 0;
    ASSERT(++b1 == 1, "");
    ASSERT(b1 == 1, "");

    ASSERT(b1++ == 1, "");
    ASSERT(b1 == 2, "");

    ASSERT(--b1 == 1, "");
    ASSERT(b1 == 1, "");

    ASSERT(b1-- == 1, "");
    ASSERT(b1 == 0, "");
}
TEST_CASE(bigint, testCtors) {
        int512_t a1 = 0;
        for (auto c : a1.value()) {
            ASSERT(c == 0, "");
        }
        uint512_t b1 = 0;
        for (auto c : b1.value()) {
            ASSERT(c == 0, "");
        }

        int512_t a2 = 13;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(a2.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(a2.value()[63] == 13, "");
        uint512_t b2 = 13;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(b2.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(b2.value()[63] == 13, "");

        int512_t a3 = -13;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(a3.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(a3.value()[63] == 0xf3, "");
        uint512_t b3 = -13;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(b3.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(b3.value()[63] == 0xf3, "");

        int512_t a4 = std::int128_t(13);
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(a4.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(a4.value()[63] == 13, "");
        uint512_t b4 = std::uint128_t(13);
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(b4.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(b4.value()[63] == 13, "");

        int512_t a5 = std::uint128_t(13);
        for (int idx = 0; idx < 7; ++idx) {
            ASSERT(a5.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(a5.value()[63] == 13, "");
        uint512_t b5 = std::int128_t(13);
        for (int idx = 0; idx < 7; ++idx) {
            ASSERT(b5.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(b5.value()[63] == 13, "");

        int512_t a6 = std::uint128_t(-13);
        for (int idx = 0; idx < 48; ++idx) {
            ASSERT(a6.value()[idx] == 0, std::to_string(idx));
        }
        for (int idx = 48; idx < 63; ++idx) {
            ASSERT(a6.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(a6.value()[63] == 0xf3, "");
        uint512_t b6 = std::int128_t(-13);
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(b6.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(b6.value()[63] == 0xf3, "");

        int512_t a7 = std::int128_t(-13);
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(a7.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(a7.value()[63] == 0xf3, "");
        uint512_t b7 = std::uint128_t(-13);
        for (int idx = 0; idx < 48; ++idx) {
            ASSERT(b7.value()[idx] == 0, std::to_string(idx));
        }
        for (int idx = 56; idx < 63; ++idx) {
            ASSERT(b7.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(b7.value()[63] == 0xf3, "");

        int512_t a8 = -13.0;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(a8.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(a8.value()[63] == 0xf3, "");
        uint512_t b8 = -13.0;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(b8.value()[idx] == 0xff, std::to_string(idx));
        }
        ASSERT(b8.value()[63] == 0xf3, "");

        int512_t a9 = 13.0;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(a9.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(a9.value()[63] == 13, "");
        uint512_t b9 = 13.0;
        for (int idx = 0; idx < 63; ++idx) {
            ASSERT(b9.value()[idx] == 0, std::to_string(idx));
        }
        ASSERT(b9.value()[63] == 13, "");
}
TEST_CASE(bigint, testSwap) {
    int256_t a = 0;
    int256_t b = 256;

    std::swap(a, b);

    ASSERT(a == 256, "");
    ASSERT(b == 0, "");
}
TEST_CASE(bigint, testNumericLimits) {
    ASSERT(
            std::numeric_limits<std::uint128_t>::max() ==
            340282366920938463463374607431768211455_uint128,
            "");
}


UNITTEST_MAIN() {
    RUN_TEST(bigint, testShifts)
    RUN_TEST(bigint, testAssign)
    RUN_TEST(bigint, testOperatorPlus)
    RUN_TEST(bigint, testOperatorTilda)
    RUN_TEST(bigint, testOperatorUnaryMinus)
    RUN_TEST(bigint, testOperatorPlusWide)
    RUN_TEST(bigint, testOperatorStar)
    RUN_TEST(bigint, testOperatorMore)
    RUN_TEST(bigint, testOperatorEq)
    RUN_TEST(bigint, testOperatorPipe)
    RUN_TEST(bigint, testOperatorAmp)
    RUN_TEST(bigint, testOperatorSlash)
    RUN_TEST(bigint, testOperatorPercent)
    RUN_TEST(bigint, testToString)
    RUN_TEST(bigint, testNativeOperators)
    RUN_TEST(bigint, testNativeOperatorsAssign)
    RUN_TEST(bigint, testNativeOperatorsAssignFloat)
    RUN_TEST(bigint, testConstexpr)
    RUN_TEST(bigint, testToChars)
    RUN_TEST(bigint, testNegative)
    RUN_TEST(bigint, testCtors)
    RUN_TEST(bigint, testUnaryIncOrDecr)
    RUN_TEST(bigint, testSwap)
    RUN_TEST(bigint, testNumericLimits)
    RUN_TEST(bigint, testFromString)
}
