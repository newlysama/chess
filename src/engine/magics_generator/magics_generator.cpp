/**
 * @file      magics_generator.cpp
 * @author    Thibault THOMAS
 * @copyright Copyright 2025 Better Chess Engine
 * @par       This project is released under the MIT License
 *
 * @brief Magic numbers generation implementation.
 */

#include "engine/magics_generator/magics_generator.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <oneapi/tbb.h>
#include <random>
#include <vector>

#include "engine/board/mask.h"
#include "logging/logging.h"

namespace engine::magics_generator
{
    using namespace engine::core;
    using namespace engine::board;

    Bitboard slidingAttackRook(const int square, const Bitboard occupancy) noexcept
    {
        Bitboard attacks(0ULL);
        Bitboard occupancyMasked = occupancy & ROOK_RELEVANT_MASKS[square];

        // North
        Bitboard ray = shiftDir<Direction::NORTH>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::NORTH>(ray);
        }

        // South
        ray = shiftDir<Direction::SOUTH>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::SOUTH>(ray);
        }

        // East
        ray = shiftDir<Direction::EAST>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::EAST>(ray);
        }

        // West
        ray = shiftDir<Direction::WEST>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::WEST>(ray);
        }

        return attacks;
    }

    Bitboard slidingAttackBishop(const int square, const Bitboard occupancy) noexcept
    {
        Bitboard attacks(0ULL);
        Bitboard occupancyMasked = occupancy & BISHOP_RELEVANT_MASKS[square];

        // NE
        Bitboard ray = shiftDir<Direction::NORTH_EAST>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::NORTH_EAST>(ray);
        }

        // NW
        ray = shiftDir<Direction::NORTH_WEST>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::NORTH_WEST>(ray);
        }

        // SE
        ray = shiftDir<Direction::SOUTH_EAST>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::SOUTH_EAST>(ray);
        }

        // SW
        ray = shiftDir<Direction::SOUTH_WEST>(Bitboard{1ULL << square});
        while (!ray.isEmpty())
        {
            attacks |= ray;
            if ((ray & occupancyMasked).isEmpty() == false)
                break;

            ray = shiftDir<Direction::SOUTH_WEST>(ray);
        }

        return attacks;
    }

    uint8_t findShiftRook(const int square) noexcept
    {
        return 64 - ROOK_RELEVANT_MASKS[square].popCount();
    }

    uint8_t findShiftBishop(int square) noexcept
    {
        return 64 - BISHOP_RELEVANT_MASKS[square].popCount();
    }

    Bitboard findMagicRook(int square) noexcept
    {
        const Bitboard mask = ROOK_RELEVANT_MASKS[square];
        const uint8_t shift = findShiftRook(square);

        const uint8_t bits = mask.popCount();
        const uint64_t subsetCount = 1ULL << bits;

        std::vector<uint64_t> used(subsetCount, 0ULL);
        std::vector<uint64_t> occupancies(subsetCount);

        // Precompute all occupancyupancy subsets
        std::vector<int> bitIndices;
        for (int i = 0; i < 64; i++)
        {
            if (mask.isSet(i))
            {
                bitIndices.push_back(i);
            }
        }

        for (uint64_t idx = 0; idx < subsetCount; idx++)
        {
            uint64_t occupancy = 0ULL;
            for (uint8_t j = 0; j < bits; j++)
            {
                if (idx & (1ULL << j))
                {
                    occupancy |= (1ULL << bitIndices[j]);
                }
            }
            occupancies[idx] = occupancy;
        }

        std::mt19937_64 rng(std::random_device{}());
        while (true)
        {
            uint64_t magic = rng() & rng() & rng();
            std::fill(used.begin(), used.end(), 0ULL);
            bool fail = false;

            for (uint64_t idx = 0; idx < subsetCount; idx++)
            {
                uint64_t occupancy = occupancies[idx];
                uint64_t key = (occupancy * magic) >> shift;
                uint64_t attack = slidingAttackRook(square, Bitboard{occupancy}).m_data;

                if (used[key] == 0ULL)
                {
                    used[key] = attack;
                }
                else if (used[key] != attack)
                {
                    fail = true;
                    break;
                }
            }

            if (!fail)
            {
                return Bitboard{magic};
            }
        }
    }

    Bitboard findMagicBishop(const int square) noexcept
    {
        const Bitboard mask = BISHOP_RELEVANT_MASKS[square];
        const uint8_t shift = findShiftBishop(square);

        const uint8_t bits = mask.popCount();
        const uint64_t subsetCount = 1ULL << bits;

        std::vector<uint64_t> used(subsetCount, 0ULL);
        std::vector<uint64_t> occupancies(subsetCount);
        std::vector<int> bitIndices;

        for (int i = 0; i < 64; i++)
        {
            if (mask.isSet(i))
            {
                bitIndices.push_back(i);
            }
        }

        for (uint64_t idx = 0; idx < subsetCount; idx++)
        {
            uint64_t occupancy = 0ULL;
            for (uint8_t j = 0; j < bits; j++)
            {
                if (idx & (1ULL << j))
                {
                    occupancy |= (1ULL << bitIndices[j]);
                }
            }

            occupancies[idx] = occupancy;
        }

        std::mt19937_64 rng(std::random_device{}());
        while (true)
        {
            uint64_t magic = rng() & rng() & rng();
            std::fill(used.begin(), used.end(), 0ULL);
            bool fail = false;

            for (uint64_t idx = 0; idx < subsetCount; idx++)
            {
                uint64_t occupancy = occupancies[idx];
                uint64_t key = (occupancy * magic) >> shift;
                uint64_t attack = slidingAttackBishop(square, Bitboard{occupancy}).m_data;

                if (used[key] == 0ULL)
                {
                    used[key] = attack;
                }
                else if (used[key] != attack)
                {
                    fail = true;
                    break;
                }
            }

            if (!fail)
                return Bitboard{magic};
        }
    }

    void initMagics() noexcept
    {
        std::array<uint64_t, 64> rMagicVals;
        std::array<uint8_t, 64> rShiftVals;
        std::array<uint64_t, 64> bMagicVals;
        std::array<uint8_t, 64> bShiftVals;

        tbb::parallel_for(0, 64, [&](int square) {
            LOG_INFO("Computing magics for square {}", square);
            auto rmagic = findMagicRook(square);
            auto bmagic = findMagicBishop(square);

            rMagicVals[square] = rmagic.m_data;
            bMagicVals[square] = bmagic.m_data;

            LOG_INFO("Computing shifts for square {}", square);
            rShiftVals[square] = findShiftRook(square);
            bShiftVals[square] = findShiftBishop(square);
        });

        // Print C++ constexpr initializers
        // clang-format off
        std::cout << "inline constexpr core::BitboardTable rookMagics = {";
        for (int i = 0; i < 64; i++)
        {
            std::cout << "Bitboard{0x"
                      << std::hex
                      << rMagicVals[i]
                      << "ULL}"
                      << (i < 63 ? ", " : i % 3 == 0 ? "\n" : "");
        }
        std::cout << "};\n\n";

        std::cout << "inline constexpr core::BitboardTable bishopMagics = {";
        for (int i = 0; i < 64; i++)
        {
            std::cout << "Bitboard{0x"
                      << std::hex
                      << bMagicVals[i]
                      << "ULL}"
                      << (i < 63 ? ", " : i % 3 == 0 ? "\n" : "");
        }
        std::cout << "};\n\n";

        std::cout << "inline constexpr std::array<uint8_t,64> rookShifts = {";
        for (int i = 0; i < 64; i++)
        {
            std::cout << std::dec
                      << static_cast<int>(rShiftVals[i])
                      << (i < 63 ? ", " : i % 3 == 0 ? "\n" : "");
        }
        std::cout << "};\n\n";

        std::cout << "inline constexpr std::array<uint8_t,64> bishopShifts = {";
        for (int i = 0; i < 64; i++)
        {
            std::cout << std::dec
                      << static_cast<int>(bShiftVals[i])
                      << (i < 63 ? ", " : i % 3 == 0 ? "\n" : "");
        }
        std::cout << "};\n\n";
    }

} // namespace engine::magics_generator
