#include "nnue_mini_accumulator.h"

using namespace Stockfish::Eval::NNUE;
using namespace Stockfish::Eval::NNUE::SIMD;

namespace Stockfish::Eval::NNUE {
    template class MiniAccumulatorStackBase<L1Mini>;

    template<int N>
    const MiniAccumulator<N>& MiniAccumulatorStackBase<N>::current() const {
        return stack[stackIndex];
    }

    template<int N>
    void MiniAccumulatorStackBase<N>::pop() {
        stackIndex--;
    }

    template<int N>
    void MiniAccumulatorStackBase<N>::push(const DirtyPiece& dp, const NetworkMini &networkMini) {
        stack[stackIndex + 1] = stack[stackIndex];
        stackIndex++;

        sub(networkMini.inputLayerWeights[index(dp.pc, dp.from)]);

        if (dp.to != SQ_NONE) {
            add(networkMini.inputLayerWeights[index(dp.pc, dp.to)]);
        }

        if (dp.add_sq != SQ_NONE) {
            add(networkMini.inputLayerWeights[index(dp.add_pc, dp.add_sq)]);
        }

        if (dp.remove_sq != SQ_NONE) {
            sub(networkMini.inputLayerWeights[index(dp.remove_pc, dp.remove_sq)]);
        }
    }

    template<int N>
    void MiniAccumulatorStackBase<N>::add(const MiniAccumulator<N>& weights) {
        #ifdef VECTOR
            constexpr int STEP = sizeof(vec_t) / sizeof(std::int16_t);

            auto* dst = stack[stackIndex].data();
            auto* src = weights.data();

            for (int i = 0; i < N; i += STEP)
            {
                vec_t a = vec_load(reinterpret_cast<const vec_t*>(&dst[i]));
                vec_t b = vec_load(reinterpret_cast<const vec_t*>(&src[i]));
                vec_store(reinterpret_cast<vec_t*>(&dst[i]), vec_add_16(a, b));
            }
        #else
            for (int i = 0; i < N; i++)
                stack[stackIndex][i] += weights[i];
        #endif
    }

    template<int N>
    void MiniAccumulatorStackBase<N>::sub(const MiniAccumulator<N>& weights) {
        #ifdef VECTOR
            constexpr int STEP = sizeof(vec_t) / sizeof(std::int16_t);

            auto* dst = stack[stackIndex].data();
            auto* src = weights.data();

            for (int i = 0; i < N; i += STEP)
            {
                vec_t a = vec_load(reinterpret_cast<const vec_t*>(&dst[i]));
                vec_t b = vec_load(reinterpret_cast<const vec_t*>(&src[i]));
                vec_store(reinterpret_cast<vec_t*>(&dst[i]), vec_sub_16(a, b));
            }
        #else
            for (int i = 0; i < N; i++)
                stack[stackIndex][i] -= weights[i];
        #endif
    }

    template<int N>
    void MiniAccumulatorStackBase<N>::set_position(const Stockfish::Position& position, const NetworkMini& networkMini) {
        stackIndex = 0;
        stack[stackIndex] = networkMini.inputLayerBias;

        for (int sq = 0; sq < 64; sq++) {

            Piece pc = position.piece_on(Square(sq));
            if (pc == NO_PIECE)
                continue;

            int pieceIndex = index(pc, Square(sq));
            add(networkMini.inputLayerWeights[pieceIndex]);
        }
    }

    template<int N>
    int MiniAccumulatorStackBase<N>::index(Piece pc, Square sq) {
        return (pc - 1 - (pc >= 8) * 2) * 64 + sq;
    }

}