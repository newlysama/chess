/**
 * @file      test_game.cpp
 * @author    Thibault THOMAS
 * @copyright Copyright 2025 Better Chess Engine
 * @par       This project is released under the MIT License
 *
 * @brief Tests the legal move .
 */

#include "test_utils.h"

namespace test
{
    using namespace engine::core;
    using namespace engine::game;
    using namespace engine::board;

    // ---------------------------------------------------------------------
    // DEPTH 1
    // ---------------------------------------------------------------------

    TEST(PeftTest, FEN_1_DEPTH_1)
    {
        Game game(FEN_1);
        Counters counters;

        uint64_t nodes = perft<true>(game, 1, counters);
        Counters expected = {0, 0, 0, 0, 0, 0, 0};

        EXPECT_EQ(nodes, 20);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_2_DEPTH_1)
    {
        Game game(FEN_2);
        Counters counters;

        uint64_t nodes = perft<true>(game, 1, counters);
        Counters expected = {8, 0, 2, 0, 0, 0, 0};

        EXPECT_EQ(nodes, 48);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_3_DEPTH_1)
    {
        Game game(FEN_3);
        Counters counters;

        uint64_t nodes = perft<true>(game, 1, counters);
        Counters expected = {1, 0, 0, 0, 2, 0, 0};

        EXPECT_EQ(nodes, 14);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_4_DEPTH_1)
    {
        Game game(FEN_4);
        Counters counters;

        uint64_t nodes = perft<true>(game, 1, counters);
        Counters expected = {0, 0, 0, 0, 0, 0, 0};

        EXPECT_EQ(nodes, 6);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_5_DEPTH_1)
    {
        Game game(FEN_5);
        Counters counters;

        uint64_t nodes = perft<true>(game, 1, counters);

        EXPECT_EQ(nodes, 44);
    }

    TEST(PeftTest, FEN_6_DEPTH_1)
    {
        Game game(FEN_6);
        Counters counters;

        uint64_t nodes = perft<true>(game, 1, counters);

        EXPECT_EQ(nodes, 46);
    }

    // ---------------------------------------------------------------------
    // DEPTH 2
    // ---------------------------------------------------------------------

    TEST(PeftTest, FEN_1_DEPTH_2)
    {
        Game game(FEN_1);
        Counters counters;

        uint64_t nodes = perft<true>(game, 2, counters);
        Counters expected = {0, 0, 0, 0, 0, 0, 0};

        EXPECT_EQ(nodes, 400);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_2_DEPTH_2)
    {
        Game game(FEN_2);
        Counters counters;

        uint64_t nodes = perft<true>(game, 2, counters);
        Counters expected = {351, 1, 91, 0, 3, 0, 0};

        EXPECT_EQ(nodes, 2039);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_3_DEPTH_2)
    {
        Game game(FEN_3);
        Counters counters;

        uint64_t nodes = perft<true>(game, 2, counters);
        Counters expected = {14, 0, 0, 0, 10, 0, 0};

        EXPECT_EQ(nodes, 191);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_4_DEPTH_2)
    {
        Game game(FEN_4);
        Counters counters;

        uint64_t nodes = perft<true>(game, 2, counters);
        Counters expected = {87, 0, 6, 48, 10, 0, 0};

        EXPECT_EQ(nodes, 264);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_5_DEPTH_2)
    {
        Game game(FEN_5);
        Counters counters;

        uint64_t nodes = perft<true>(game, 2, counters);

        EXPECT_EQ(nodes, 1486);
    }

    TEST(PeftTest, FEN_6_DEPTH_2)
    {
        Game game(FEN_6);
        Counters counters;

        uint64_t nodes = perft<true>(game, 2, counters);

        EXPECT_EQ(nodes, 2079);
    }

    // ---------------------------------------------------------------------
    // DEPTH 3
    // ---------------------------------------------------------------------

    TEST(PeftTest, FEN_1_DEPTH_3)
    {
        Game game(FEN_1);
        Counters counters;

        uint64_t nodes = perft<true>(game, 3, counters);
        Counters expected = {34, 0, 0, 0, 12, 0, 0};

        EXPECT_EQ(nodes, 8902);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_2_DEPTH_3)
    {
        Game game(FEN_2);
        Counters counters;

        uint64_t nodes = perft<true>(game, 3, counters);
        Counters expected = {17102, 45, 3162, 0, 993, 0, 1};

        EXPECT_EQ(nodes, 97862);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_3_DEPTH_3)
    {
        Game game(FEN_3);
        Counters counters;

        uint64_t nodes = perft<true>(game, 3, counters);
        Counters expected = {209, 2, 0, 0, 267, 0, 0};

        EXPECT_EQ(nodes, 2812);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_4_DEPTH_3)
    {
        Game game(FEN_4);
        Counters counters;

        uint64_t nodes = perft<true>(game, 3, counters);
        Counters expected = {1021, 4, 0, 120, 38, 0, 22};

        EXPECT_EQ(nodes, 9467);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_5_DEPTH_3)
    {
        Game game(FEN_5);
        Counters counters;

        uint64_t nodes = perft<true>(game, 3, counters);

        EXPECT_EQ(nodes, 62379);
    }

    TEST(PeftTest, FEN_6_DEPTH_3)
    {
        Game game(FEN_6);
        Counters counters;

        uint64_t nodes = perft<true>(game, 3, counters);

        EXPECT_EQ(nodes, 89890);
    }

    // ---------------------------------------------------------------------
    // DEPTH 4
    // ---------------------------------------------------------------------

    TEST(PeftTest, FEN_1_DEPTH_4)
    {
        Game game(FEN_1);
        Counters counters;

        uint64_t nodes = perft<true>(game, 4, counters);
        Counters expected = {1576, 0, 0, 0, 469, 0, 8};

        EXPECT_EQ(nodes, 197281);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_2_DEPTH_4)
    {
        Game game(FEN_2);
        Counters counters;

        uint64_t nodes = perft<true>(game, 4, counters);
        Counters expected = {757163, 1929, 128013, 15172, 25523, 6, 43};

        EXPECT_EQ(nodes, 4085603);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_3_DEPTH_4)
    {
        Game game(FEN_3);
        Counters counters;

        uint64_t nodes = perft<true>(game, 4, counters);
        Counters expected = {3348, 123, 0, 0, 1680, 0, 17};

        EXPECT_EQ(nodes, 43238);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_4_DEPTH_4)
    {
        Game game(FEN_4);
        Counters counters;

        uint64_t nodes = perft<true>(game, 4, counters);
        Counters expected = {131393, 0, 7795, 60032, 15492, 0, 5};

        EXPECT_EQ(nodes, 422333);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_5_DEPTH_4)
    {
        Game game(FEN_5);
        Counters counters;

        uint64_t nodes = perft<true>(game, 4, counters);

        EXPECT_EQ(nodes, 2103487);
    }

    TEST(PeftTest, FEN_6_DEPTH_4)
    {
        Game game(FEN_6);
        Counters counters;

        uint64_t nodes = perft<true>(game, 4, counters);

        EXPECT_EQ(nodes, 3894594);
    }

    // ---------------------------------------------------------------------
    // DEPTH 5
    // ---------------------------------------------------------------------

    TEST(PeftTest, FEN_1_DEPTH_5)
    {
        Game game(FEN_1);
        Counters counters;

        uint64_t nodes = perft<true>(game, 5, counters);
        Counters expected = {82719, 258, 0, 0, 27351, 0, 347};

        EXPECT_EQ(nodes, 4865609);
        counters.check(expected);
    }

    /**
     * @todo FIX ME
     */
    // TEST(PeftTest, FEN_2_DEPTH_5)
    // {
    //     Game game(FEN_2);
    //     Counters counters;

    //     uint64_t nodes = perft<true>(game, 5, counters);
    //     Counters expected = {35043416, 73365, 4993637, 8392, 3309887, 2637, 30171};

    //     EXPECT_EQ(nodes, 193690690);
    //     counters.check(expected);
    // }

    TEST(PeftTest, FEN_3_DEPTH_5)
    {
        Game game(FEN_3);
        Counters counters;

        uint64_t nodes = perft<true>(game, 5, counters);
        Counters expected = {52051, 1165, 0, 0, 52950, 3, 0};

        EXPECT_EQ(nodes, 674624);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_4_DEPTH_5)
    {
        Game game(FEN_4);
        Counters counters;

        uint64_t nodes = perft<true>(game, 5, counters);
        Counters expected = {2046173, 6512, 0, 329464, 200568, 50, 50562};

        EXPECT_EQ(nodes, 15833292);
        counters.check(expected);
    }

    TEST(PeftTest, FEN_5_DEPTH_5)
    {
        Game game(FEN_5);
        Counters counters;

        uint64_t nodes = perft<true>(game, 5, counters);

        EXPECT_EQ(nodes, 89941194);
    }

    TEST(PeftTest, FEN_6_DEPTH_5)
    {
        Game game(FEN_6);
        Counters counters;

        uint64_t nodes = perft<true>(game, 5, counters);

        EXPECT_EQ(nodes, 164075551);
    }

} // namespace test
