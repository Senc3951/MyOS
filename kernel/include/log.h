#pragma once

#include <common.h>

enum
{
    LOG_LEVEL_NORMAL = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

#define LOG(...)                    LOG_NORMAL_INFO(__VA_ARGS__)
#define LOG_NORMAL_INFO(...)        LOG_MSG(true, LOG_LEVEL_NORMAL, __VA_ARGS__)
#define LOG_WARNING_INFO(...)       LOG_MSG(true, LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERROR_INFO(...)         LOG_MSG(true, LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_NORMAL_NO_INFO(...)     LOG_MSG(false, LOG_LEVEL_NORMAL, __VA_ARGS__)
#define LOG_WARNING_NO_INFO(...)    LOG_MSG(false, LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERROR_NO_INFO(...)      LOG_MSG(false, LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_MSG(pinfo, lvl, ...)    __log_message(pinfo, lvl, __FILE__, __LINE__, __VA_ARGS__)

void __log_message(const bool pinfo, uint32_t lvl, const char *file, const uint32_t line, const char *fmt, ...);