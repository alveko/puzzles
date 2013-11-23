#include <algorithm>
#include <vector>
#include <iostream>  // std::cin, std::cout

int binomialCoeff1(int n, int k)
{
    int C[n + 1][k + 1];
    int i, j;

    for (i = 0; i <= n; i++)
        for (j = 0; j <= k; j++)
            C[i][j] = 3;

    // Caculate value of Binomial Coefficient in bottom up manner
    for (i = 0; i <= n; i++) {
        for (j = 0; j <= std::min(i, k); j++) {
            if (j == 0 || j == i) {
                // Base case
                C[i][j] = 1;
            } else {
                // Calculate value using previosly stored values
                C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
            }
        }
    }

    return C[n][k];
}

int binomialCoeff2(int n, int k)
{
    int i, j, res;
    int* C = (int*)calloc(k + 1, sizeof(int));

    C[0] = 1;

    for (i = 1; i <= n; i++) {
        for (j = std::min(i, k); j > 0; j--) {
            C[j] = C[j] + C[j - 1];
        }
    }

    res = C[k];
    free(C);
    return res;
}

int binomialCoeff3(int n, int k)
{
    int c = n - (k - 1);
    for (int i = 2; i <= k; ++i) {
        c = (c * (n - (k - i))) / i;
    }
    return c;
}

int main(int argc, char* argv[])
{
    int n = 1, k = 1;
    std::cout << "Enter n and k:" << std::endl;
    std::cin >> n >> k;
    std::cout << "n = " << n << "; k = " << k << std::endl;

    std::cout << "binomialCoeff1(n, k) = " << binomialCoeff1(n, k) << std::endl;
    std::cout << "binomialCoeff2(n, k) = " << binomialCoeff2(n, k) << std::endl;
    std::cout << "binomialCoeff3(n, k) = " << binomialCoeff3(n, k) << std::endl;

    return 0;
}
