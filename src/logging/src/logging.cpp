/**
 * @file logging.cpp
 * @author Thibault THOMAS
 *
 * @brief Implementation of logging system management
 */

/*----- System -----*/
#include <vector>

/*----- External Libs -----*/
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

/*----- Project Headers -----*/
#include "logging/include/logging.h"
#include "conf/paths.h"

namespace logging
{

    // Define loggers
    static std::shared_ptr<spdlog::logger> eng_logger;
    static std::shared_ptr<spdlog::logger> app_logger;

/*-------------------------------------------------------------------*
 *                        INIT ENGINE LOGGER                         *
 *-------------------------------------------------------------------*/

    /**
     * @details
     * Defines the engine's logger (sinks, level, output) and initialize it.
     * If we build in release mode, log in log files defined in const.h.
     * If we are in any other mode, log in stdout.
     */
    void init_engine_logger()
    {
        if (eng_logger) return;

        std::vector<spdlog::sink_ptr> sinks;

        #if defined(BUILD_RELEASE)
            auto path = conf::paths::engine_log_file;

            sinks.push_back(
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true)
            );

        #else
            sinks.push_back(
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
            );

        #endif

        eng_logger = std::make_shared<spdlog::logger>("engine", sinks.begin(), sinks.end());

        #if defined(BUILD_DEBUG) || defined(BUILD_PROFILE) || defined(BUILD_TEST) || defined(BUILD_BENCHMARK)
            eng_logger->set_level(spdlog::level::debug);

        #else
            eng_logger->set_level(spdlog::level::warn);
        #endif

        spdlog::register_logger(eng_logger);
    }

    std::shared_ptr<spdlog::logger> engine_logger()
    {
        init_engine_logger();
        return eng_logger;
    }

/*-------------------------------------------------------------------*
 *                          INIT APP LOGGER                          *
 *-------------------------------------------------------------------*/

    /**
     * @details
     * Defines the app's logger (sinks, level, output) and initialize it.
     * If we build in release mode, log in log files defined in const.h.
     * If we are in any other mode, log in stdout.
     */
    void init_application_logger()
    {
        if (app_logger) return;

        std::vector<spdlog::sink_ptr> sinks;

        #if defined(BUILD_RELEASE)
            auto path = conf::paths::app_log_file;

            sinks.push_back(
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true)
            );

        #else
            sinks.push_back(
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
            );
        #endif

        app_logger = std::make_shared<spdlog::logger>("app", sinks.begin(), sinks.end());

        #if defined(BUILD_DEBUG) || defined(BUILD_PROFILE) || defined(BUILD_TEST) || defined(BUILD_BENCHMARK)
            app_logger->set_level(spdlog::level::debug);

        #else
            app_logger->set_level(spdlog::level::warn);
        #endif

        spdlog::register_logger(app_logger);
    }

    std::shared_ptr<spdlog::logger> application_logger()
    {
        init_application_logger();
        return app_logger;
    }

} // namespace logging
