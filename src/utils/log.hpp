#pragma once

#define DEBUG

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif