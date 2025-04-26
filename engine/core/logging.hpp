#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>
#include <array>

namespace softcube {
    using LogGroupID = std::string_view;

    namespace LogGroups {
        inline LogGroupID create_group(const LogGroupID parent, const LogGroupID child) {
            static std::unordered_map<std::string, std::string> cache;
            const std::string key = std::string(parent) + "::" + std::string(child);
            const auto it = cache.find(key);
            if (it != cache.end()) {
                return it->second;
            }
            cache[key] = key;
            return cache[key];
        }

        template<size_t N>
        struct string_literal {
            explicit constexpr string_literal(const char (&str)[N]) {
                std::copy_n(str, N, value);
            }

            char value[N]{};
        };

        template<string_literal Parent, string_literal Child>
        struct compile_time_group {
        private:
            static constexpr auto combine() {
                constexpr size_t parent_size = std::char_traits<char>::length(Parent.value);
                constexpr size_t child_size = std::char_traits<char>::length(Child.value);
                constexpr size_t separator_size = 2;

                std::array<char, parent_size + separator_size + child_size + 1> result = {};

                for (size_t i = 0; i < parent_size; ++i) {
                    result[i] = Parent.value[i];
                }

                result[parent_size] = ':';
                result[parent_size + 1] = ':';

                for (size_t i = 0; i < child_size; ++i) {
                    result[parent_size + separator_size + i] = Child.value[i];
                }

                return result;
            }

            static constexpr auto combined = combine();

        public:
            static constexpr std::string_view value{combined.data()};
        };
    }

    class logger {
    private:
        logger() = default;

        struct logger_state {
            std::shared_ptr<spdlog::logger> core_logger = nullptr;
            std::unordered_map<std::string, std::shared_ptr<spdlog::logger> > group_loggers;
            bool initialized = false;
            std::recursive_mutex mutex;
        };

        static logger_state &get_state() {
            static logger_state state;
            return state;
        }

    public:
        logger(const logger &) = delete;

        logger &operator=(const logger &) = delete;

        static void init(const std::string_view log_file_path = "softcube.log") {
            auto &state = get_state();
            std::unique_lock lock(state.mutex);

            if (state.initialized) {
                return;
            }

            try {
                spdlog::drop_all();

                const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");

                const auto file_sink = std::make_shared<
                    spdlog::sinks::basic_file_sink_mt>(std::string(log_file_path), true);
                file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");

                spdlog::sinks_init_list sinks = {console_sink, file_sink};

                state.core_logger = std::make_shared<spdlog::logger>("CORE", sinks);
                state.core_logger->set_level(spdlog::level::trace);
                state.core_logger->flush_on(spdlog::level::trace);

                register_logger(state.core_logger);
                spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");
                spdlog::set_level(spdlog::level::trace);

                state.initialized = true;
            } catch (const spdlog::spdlog_ex &ex) {
                std::cerr << "logger initialization failed: " << ex.what() << std::endl;
            }
        }

        static void shutdown() {
            auto &[core_logger, group_loggers, initialized, mutex] = get_state();
            std::unique_lock lock(mutex);

            if (!initialized) {
                return;
            }

            try {
                core_logger->info("shutting down logging system");

                for (auto &[name, logger_instance]: group_loggers) {
                    spdlog::drop(name);
                }
                group_loggers.clear();
                spdlog::drop("core");
                core_logger.reset();

                spdlog::shutdown();
                initialized = false;
            } catch (const std::exception &ex) {
                std::cerr << "logger shutdown failed: " << ex.what() << std::endl;
            }
        }

        static std::shared_ptr<spdlog::logger> get_group_logger(LogGroupID group) {
            auto &[core_logger, group_loggers, initialized, mutex] = get_state();
            std::unique_lock lock(mutex);

            if (!initialized) {
                lock.unlock();
                init();
                lock.lock();

                if (!initialized) {
                    throw std::runtime_error("Failed to initialize logger");
                }
            }

            const std::string group_name(group);

            if (const auto it = group_loggers.find(group_name); it != group_loggers.end()) {
                return it->second;
            }

            try {
                auto group_logger = core_logger->clone(group_name);
                group_loggers[group_name] = group_logger;
                register_logger(group_logger);
                return group_logger;
            } catch (const spdlog::spdlog_ex &ex) {
                std::cerr << "Failed to create logger for group " << group_name << ": " << ex.what() << std::endl;
                return core_logger;
            }
        }

        static void set_level(const spdlog::level::level_enum level) {
            auto &[core_logger, group_loggers, initialized, mutex] = get_state();
            std::unique_lock lock(mutex);

            if (!initialized) {
                return;
            }

            try {
                spdlog::set_level(level);

                if (core_logger) {
                    core_logger->set_level(level);
                }

                for (auto &[_, logger_instance]: group_loggers) {
                    logger_instance->set_level(level);
                }
            } catch (const std::exception &ex) {
                std::cerr << "Failed to set log level: " << ex.what() << std::endl;
            }
        }

        static void set_group_level(const LogGroupID group, const spdlog::level::level_enum level) {
            try {
                auto &state = get_state();
                std::unique_lock lock(state.mutex);

                if (!state.initialized) {
                    return;
                }

                const std::string group_name(group);

                if (const auto it = state.group_loggers.find(group_name); it != state.group_loggers.end()) {
                    it->second->set_level(level);
                } else {
                    const auto logger_instance = get_group_logger(group);
                    logger_instance->set_level(level);
                }
            } catch (const std::exception &ex) {
                std::cerr << "Failed to set log level for group: " << ex.what() << std::endl;
            }
        }
    };
}


#ifdef _DEBUG
#define SC_LOG_GROUP_TRACE(group, ...) ::logger::get_group_logger(group)->trace(__VA_ARGS__)
#define SC_LOG_GROUP_DEBUG(group, ...) ::logger::get_group_logger(group)->debug(__VA_ARGS__)
#else
    #define SC_LOG_GROUP_TRACE(group, ...) (void)0
    #define SC_LOG_GROUP_DEBUG(group, ...) (void)0
#endif

#define SC_LOG_GROUP_INFO(group, ...)     ::logger::get_group_logger(group)->info(__VA_ARGS__)
#define SC_LOG_GROUP_WARN(group, ...)     ::logger::get_group_logger(group)->warn(__VA_ARGS__)
#define SC_LOG_GROUP_ERROR(group, ...)    ::logger::get_group_logger(group)->error(__VA_ARGS__)
#define SC_LOG_GROUP_CRITICAL(group, ...) ::logger::get_group_logger(group)->critical(__VA_ARGS__)

#define SC_LOG_GROUP(group) \
    private: \
        static constexpr ::LogGroupID get_log_group() { return #group; }
#define SC_TRACE(...)    SC_LOG_GROUP_TRACE(get_log_group(), __VA_ARGS__)
#define SC_DEBUG(...)    SC_LOG_GROUP_DEBUG(get_log_group(), __VA_ARGS__)
#define SC_INFO(...)     SC_LOG_GROUP_INFO(get_log_group(), __VA_ARGS__)
#define SC_WARN(...)     SC_LOG_GROUP_WARN(get_log_group(), __VA_ARGS__)
#define SC_ERROR(...)    SC_LOG_GROUP_ERROR(get_log_group(), __VA_ARGS__)
#define SC_CRITICAL(...) SC_LOG_GROUP_CRITICAL(get_log_group(), __VA_ARGS__)
