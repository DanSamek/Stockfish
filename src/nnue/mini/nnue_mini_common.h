#ifndef NNUE_MINI_COMMON_H_INCLUDED
#define NNUE_MINI_COMMON_H_INCLUDED

#include <array>
#include <cstdint>

namespace Stockfish::Eval::NNUE{
    template<int N>
    using MiniAccumulator = std::array<std::int16_t, N>;
}
#endif