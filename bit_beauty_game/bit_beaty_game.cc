#include <stdint.h>
#include <iostream>
#include <fstream>

// Every move rearranges one 1-bit and one 0-bit in the number so that
// the number gets decreased, but still the number of bits set is the same,
// for example:
// 0011011011101110 ->
// 0011011011011110
//           ^^
// The game ends when all 1-bits are grouped to the right like this:
// 0000001111111111
// When more than one move is possible, it does not matter which one is played,
// since it does not affect the total number of moves.
//
// In order to answer who wins, we shall simply count the number of moves before
// the game ends. The simpliest (as it appears) way to do that is to go from
// left to right and for every 0-bit count how far to the left it shall be
// shifted.

// Calculate number of moves in the game.
// Running time O(log2(n)), i.e. linear in the number of bits in n.
uint32_t play_game(uint32_t n)
{
    uint32_t x = (1 << 31); // our bit "pointer" sliding from left to right
    uint32_t c1 = 0;        // counter for 1-bits seen so far
    uint32_t rc = 0;        // result counter

    // skip heading zeros
    while (!(n & x)) {
        x >>= 1;
    }

    while (x) {
        // find next 0, skip and count 1-bits
        while (n & x) {
            c1++;
            x >>= 1;
        }
        // check if x points to 0-bit in n
        if (x && !(n & x)) {
            // this 0-bit shall be shifted to the left
            // by the number of 1-bits seen so far
            rc += c1;
        }
        x >>= 1;
    }
    return rc;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::ifstream ifs(argv[1]);

    int tcs = 0;
    ifs >> tcs;
    std::cout << "Number of test cases: " << tcs << std::endl;

    uint32_t n = 0, gcnt = 0;
    for (int i = 0; i < tcs; i++) {
        ifs >> n;
        gcnt = play_game(n);
        std::cout << "TC" << (i+1) << ": "
                  << "winner = " << (gcnt & 1)
                  << ", n = " << n
                  << ", count = " << gcnt << std::endl;
    }

    ifs.close();
    return 0;
}
