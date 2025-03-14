#pragma once

#define DEBUG

#ifdef DEBUG
#define LOG(...)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        printf(__VA_ARGS__);                                                                       \
        printf("\n");                                                                              \
        fflush(stdout);                                                                            \
    } while (false)
#else
#define LOG(...)
#endif

#define ERR(...)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        printf("\033[31m");                                                                        \
        LOG(__VA_ARGS__);                                                                          \
        printf("\033[0m");                                                                         \
    } while (false)

#define WRN(...)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        printf("\033[33m");                                                                        \
        LOG(__VA_ARGS__);                                                                          \
        printf("\033[0m");                                                                         \
    } while (false)

#define IMP(...)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        printf("\033[32m");                                                                        \
        LOG(__VA_ARGS__);                                                                          \
        printf("\033[0m");                                                                         \
    } while (false)
