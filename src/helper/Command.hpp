#pragma once

namespace helper
{
enum class Command
{
    Random,
    Commutative,
    Cancellation,
    Idempotency,
    Cda,
    SeVpa,
    MeVpa,
    eCda,
    Custom,
    Benchmark,
    Help,
    InvalidNumOfArg,
    Unknown
};
} // namespace helper