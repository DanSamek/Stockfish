#ifndef NNUE_MINI_ACCUMULATOR_H_INCLUDED
#define NNUE_MINI_ACCUMULATOR_H_INCLUDED

#include <array>
#include <cstdint>

#include "../../types.h"
#include "../../position.h"
#include "nnue_mini_common.h"
#include "../network.h"

namespace Stockfish::Eval::NNUE {

    template<int N>
    using MiniAccumulator = std::array<std::int16_t, N>;

    template<int N>
    class MiniAccumulatorStackBase {
        alignas(CacheLineSize) std::array<MiniAccumulator<N>, MAX_PLY + 1> stack;
        int stackIndex = 0;

    public:
        void pop();
        void push(const DirtyPiece& dp, const NetworkMini& networkMini);
        const MiniAccumulator<N>& current() const;
        void set_position(const Position& position, const NetworkMini& networkMini);
    private:
        int index(Piece pc, Square sq);
        void add(const MiniAccumulator<N>& weights);
        void sub(const MiniAccumulator<N>& weights);
    };

    using MiniAccumulatorStack = MiniAccumulatorStackBase<L1Mini>;
}
#endif