#include "nnue_mini.h"

namespace Stockfish::Eval::NNUE{

    template class NetworkM<64>;


    template<int N>
    NetworkM<N>::NetworkM() {
        load();
    }

    template<int N>
    Value NetworkM<N>::evaluate(const MiniAccumulator<N>& accumulator) const {
        Value result = outputLayerBias;

        for (int i = 0; i < N; i++)
            result += crelu(accumulator[i]) * (int)outputLayerWeights[i];

        result *= SCALE;
        result /= QA * QB;
        return result;
    }

    inline int16_t read_number(std::istream& stream){
        int16_t value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    template<int N>
    void NetworkM<N>::load() {
        EmbeddedNNUE embedded = get_embedded(EmbeddedNNUEType::MINI);
        MemoryBuffer buffer(const_cast<char*>(reinterpret_cast<const char*>(embedded.data)),
                            size_t(embedded.size));

        std::istream stream(&buffer);
        for(int i = 0; i < INPUT_LAYER_SIZE; i++)
            for(int x = 0; x < N; x++)
                inputLayerWeights[i][x] = read_number(stream);

        assert(!stream.eof());
        for(int i = 0; i < N; i++)
            inputLayerBias[i] = read_number(stream);

        assert(!stream.eof());
        for(int i = 0; i < N; i++)
            outputLayerWeights[i] = read_number(stream);

        assert(!stream.eof());
        outputLayerBias = read_number(stream);
    }
}