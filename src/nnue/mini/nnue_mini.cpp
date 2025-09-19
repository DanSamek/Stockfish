#include "nnue_mini.h"

namespace Stockfish::Eval::NNUE{
    template class NetworkM<L1Mini>;

    template<int N>
    NetworkM<N>::NetworkM() {
        load();
    }

    template<int N>
    Value NetworkM<N>::evaluate(const MiniAccumulator<N>& accumulator) const {
        Value result = 0;

        for (int i = 0; i < N; i++)
            result += screlu(accumulator[i]) * (int)outputLayerWeights[i];

        result /= QA;

        result += outputLayerBias;

        result *= SCALE;
        result /= QA * QB;
        return result;
    }

    template<int N>
    void NetworkM<N>::load() {
        EmbeddedNNUE embedded = get_embedded(EmbeddedNNUEType::MINI);
        MemoryBuffer buffer(const_cast<char*>(reinterpret_cast<const char*>(embedded.data)),
                            size_t(embedded.size));

        std::istream stream(&buffer);
        for(int i = 0; i < INPUT_LAYER_SIZE; i++)
            for(int x = 0; x < N; x++)
                inputLayerWeights[i][x] = read_little_endian<int16_t>(stream);

        assert(!stream.eof());
        for(int i = 0; i < N; i++)
            inputLayerBias[i] = read_little_endian<int16_t>(stream);

        assert(!stream.eof());
        for(int i = 0; i < N; i++)
            outputLayerWeights[i] = read_little_endian<int16_t>(stream);

        assert(!stream.eof());
        outputLayerBias = read_little_endian<int16_t>(stream);
    }
}