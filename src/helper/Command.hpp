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
    Xml,
    Custom,
    Benchmark,
    Help,
    InvalidNumOfArg,
    Unknown
};
} // namespace helper