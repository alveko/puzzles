#include <iostream>  // std::cin, std::cout
#include <bitset>

int main(int argc, char *argv[])
{
    int k = 3;
    int N = 8;

    int s = (1 << k) - 1;
    while (!(s & 1 << N)) {

        // do stuff with s
        std::cout << std::bitset<8>(s);

        int lo = s & ~(s - 1);       // lowest one bit
        int lz = (s + lo) & ~s;      // lowest zero bit above lo

        std::cout << ": lo = " << std::bitset<8>(lo);
        std::cout << "  lz = " << std::bitset<8>(lz);

        s |= lz;                     // add lz to the set
        std::cout << "  (s |= lz) = "
                  << std::bitset<8>(s);

        s &= ~(lz - 1);              // reset bits below lz
        std::cout << "  (s &= ~(lz - 1)) = "
                  << std::bitset<8>(s);

        // s |= (lz / lo / 2) - 1;   // put back right number of bits at end
        s |= (lz >> __builtin_ffs(lo)) - 1;
        std::cout << "  (s |= (lz >> ffs(lo):"
                  <<  __builtin_ffs(lo) << ") - 1) = "
                  << std::bitset<8>(s);

        std::cout << std::endl;
    }

    return 0;
}
