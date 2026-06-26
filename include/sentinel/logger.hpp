#pragma once

namespace sentinel::logger
{
    void log_info(const char* message);

    void log_warn(const char* message);

    void log_error(const char* message);
}