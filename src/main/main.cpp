/**
 * @file      main.cpp
 * @author    Thibault THOMAS
 * @copyright Copyright 2025 Better Chess Engine
 * @par       This project is released under the MIT License
 *
 * @brief Entry point of the program
 */

#include "engine/benchmark/benchmark.h"
#include "engine/magics_generator/magics_generator.h"
#include "logging/logging.h"
#include "main/console_runner/console_runner.h"
#include "main/options.h"

/**
 * @brief Run the program according to the defined options
 *
 * @param [in] options : processed user entered options
 */

static int run(const options::Options& options)
{
#if defined(PLAY_CONSOLE)
    if (options.benchmark)
    {
        engine::benchmark::benchmark_perft(options.benchmark_depth);
        return 0;
    }

    auto fen = options.fen;

    console_runner::ConsoleRunner runner =
        fen.has_value() ? console_runner::ConsoleRunner{fen.value()} : console_runner::ConsoleRunner{};

    runner.runGame();

    return 0;

#elif defined(GENERATE_MAGICS)
    engine::magics_generator::initMagics();
    return 0;

#else
    return 0;
#endif
}

int main(int argc, char* argv[])
{
    // Logger
    logging::init_logger();

    // Argument parsing
    auto parsed = options::parse(argc, argv);

    if (!parsed)
    {
        std::println("Argument error: {}", parsed.error());
        return EXIT_FAILURE;
    }

    // Dispatch according to mode
    const auto& options = parsed.value();
    run(options);
}
