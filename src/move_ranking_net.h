#include "array"
#include "types.h"
#include "position.h"
#include "weights.h"

namespace Stockfish{

    /***
     * Current net is from AliceRoselia.
     * No changes made at all, just cleaned.
     * Except MRN_MAX_PLY - update accumulators only if ss->ply <= MRN_MAX_PLY.
     */
    class MoveRankingNet {
        std::array<std::array<int, 32>, 2> accumulators;

        inline int get_flipped_square(const Square& square) const{
            return (int(square) & 7) | (56 - (56 & int(square)));
        }
    public:
        static inline constexpr int MRN_MAX_PLY = 2;

        MoveRankingNet(){
            reset();
        }

        void reset(){
            for (int i = 0; i < 32; i++)
            {
                accumulators[WHITE][i] = piece_square_bias[i];
                accumulators[BLACK][i] = piece_square_bias[i];
            }
        }

        void add(const Piece& piece, const Square& square){
            int flipped_square = get_flipped_square(square);
            for (int i = 0; i < 32; i++)
            {
                accumulators[WHITE][i] += piece_square_vectors[type_of(piece) + 6 * color_of(piece) - 1][square][i];
                accumulators[BLACK][i] += piece_square_vectors[type_of(piece) + 6 * (!color_of(piece)) - 1][flipped_square][i];
            }
        }

        inline void remove(const Piece& piece, const Square& square)
        {
            int flipped_square = get_flipped_square(square);
            for (int i = 0; i < 32; i++)
            {
                accumulators[WHITE][i] -= piece_square_vectors[type_of(piece) + 6 * color_of(piece) - 1][square][i];
                accumulators[BLACK][i] -= piece_square_vectors[type_of(piece) + 6 * (!color_of(piece)) - 1][flipped_square][i];
            }
        }

        template<Color us>
        int evaluate(const Piece& piece, const Move& move) const{
            int p = int(type_of(piece)) - 1;
            Square s = move.to_sq();

            if constexpr (us == BLACK)
                s = Square(get_flipped_square(s));

            std::array<int32_t, 16> tmp;
            for (int i=0; i<16; ++i)
            {
                tmp[i] = bias2[p][s][i];
            }

            for (int i = 0; i < 32; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    tmp[j] += accumulators[us][i] * move_vectors[p][s][i][j];
                }
            }

            int final_accumulator = output_bias[p][s];
            for (int i = 0; i < 16; i++)
            {
                final_accumulator += std::max(tmp[i] >> 16, 0) * output_layer[p][s][i];
            }

            return final_accumulator >> 16;
        }
    };
}

