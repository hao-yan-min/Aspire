#include <chrono>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <random>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <string>

#include <gmp.h>
#include <omp.h>

#include <NTL/BasicThreadPool.h>
#include <NTL/FFT.h>
#include <NTL/RR.h>
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
#include <NTL/ZZ_limbs.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/mat_RR.h>

#include "seal/util/smallntt.h"
#include "seal/util/polyarith.h"
#include "seal/util/uintarith.h"
#include "seal/smallmodulus.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/defines.h"

using namespace std;
using namespace NTL;
using namespace seal;
using namespace seal::util;

thread_local size_t poly_len_as_pow_2 = 0;
thread_local size_t poly_len = 0;

inline unsigned char add_uint32(uint32_t operand1, uint32_t operand2,
                                uint32_t *result) {
    *result = operand1 + operand2;
    return static_cast<unsigned char>(*result < operand1);
}

inline constexpr uint32_t reverse_bits(uint32_t operand) noexcept
{
    operand = (((operand & uint32_t(0xaaaaaaaa)) >> 1) | ((operand & uint32_t(0x55555555)) << 1));
    operand = (((operand & uint32_t(0xcccccccc)) >> 2) | ((operand & uint32_t(0x33333333)) << 2));
    operand = (((operand & uint32_t(0xf0f0f0f0)) >> 4) | ((operand & uint32_t(0x0f0f0f0f)) << 4));
    operand = (((operand & uint32_t(0xff00ff00)) >> 8) | ((operand & uint32_t(0x00ff00ff)) << 8));
    return static_cast<uint32_t>(operand >> 16) | static_cast<uint32_t>(operand << 16);
}

inline uint32_t reverse_bits(uint32_t operand, int bit_count)
{
    // Just return zero if bit_count is zero
    return (bit_count == 0) ? 0 : reverse_bits(operand) >> (
        sizeof(uint32_t) * static_cast<std::size_t>(8)
            - static_cast<std::size_t>(bit_count));
}

void ntt_powers_of_primitive_root(uint32_t modulus, uint32_t root, uint32_t *destination)
{
    uint32_t *destination_start = destination;
    *destination_start = 1;
    for (size_t i = 1; i < poly_len; i++)
    {
        uint32_t *next_destination =
            destination_start + reverse_bits(i, poly_len_as_pow_2);
        *next_destination = (((uint64_t) (*destination)) * ((uint64_t) root)) % modulus;
        destination = next_destination;
    }
}

void ntt_div_by_two_inplace(uint32_t modulus, uint32_t *input)
{
    for (size_t i = 0; i < poly_len; i++)
    {
        input[i] = div2_uint_mod(input[i], modulus);
    }
}

void ntt_scale_powers_of_primitive_root(uint32_t modulus, const uint32_t *input, uint32_t *destination)
{
    for (size_t i = 0; i < poly_len; i++, input++, destination++)
    {
        uint64_t wide_coeff = ((uint64_t)(*input)) << 32;
        uint64_t wide_quotient = wide_coeff / modulus;
        *destination = (uint32_t)wide_quotient;
    }
}

uint32_t div2_uint_mod(uint32_t operand, uint32_t modulus) {
    if (operand & 1)
    {
        uint32_t temp;
        int32_t carry = add_uint32(operand, modulus, &temp);
        operand = temp >> 1;
        if (carry)
        {
            return operand | (uint32_t(1) << (sizeof(uint32_t)*8 - 1));
        }
        return operand;
    }
    return operand >> 1;
}

void my_ntt_scale_powers_of_primitive_root_small(const uint64_t *input, uint64_t *destination, size_t poly_len, uint32_t modulus)
{
    for (size_t i = 0; i < poly_len; i++, input++, destination++)
    {
        uint64_t wide_quotient = 0;//[2]{ 0, 0 };
        uint64_t wide_coeff = *input << 32; //[2]{ 0, *input };
        wide_quotient = wide_coeff / modulus;
        *destination = (uint32_t)wide_quotient;
    }
}


/*
poly_len = 2048, q = 2^13 - 2^12 + 1, qq = 2^31 - 2^17 + 1

    inv/for   q/qq     normal/scaled

    q  inv_root_powers_div_two              [for inv ntt]
    q  scaled_inv_root_powers_div_two
    qq inv_root_powers_div_two
    qq scaled_inv_root_powers_div_two
    q  root_powers                          [for forward ntt]
    q  scaled_root_powers
    qq root_powers
    qq scaled_root_powers
*/

