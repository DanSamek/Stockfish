#ifndef NNUE_MINI_H_INCLUDED
#define NNUE_MINI_H_INCLUDED

#include "../../types.h"
#include "../nnue_misc.h"
#include "./nnue_mini_common.h"

using namespace Stockfish::Eval::NNUE;

namespace Stockfish::Eval::NNUE {

    template<int N>
    class NetworkM {
        static inline constexpr int INPUT_LAYER_SIZE = 768;
        static inline constexpr int SCALE = 400;
        static inline constexpr int QA    = 255;
        static inline constexpr int QB    = 64;

    public:
        alignas(CacheLineSize) std::array<MiniAccumulator<N>, INPUT_LAYER_SIZE> inputLayerWeights;
        alignas(CacheLineSize) MiniAccumulator<N> inputLayerBias;

        alignas(CacheLineSize) MiniAccumulator<N> outputLayerWeights;
        alignas(CacheLineSize) std::int16_t outputLayerBias;

        NetworkM();
        Value evaluate(const MiniAccumulator<N> &accumulator) const;

    private:
        void load();

        inline int screlu(int value) const {
            int result = std::clamp(value, 0, QA);
            return result * result;
        }
    };
}
#endif