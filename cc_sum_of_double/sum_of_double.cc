#include <vector>
#include <queue>
#include <algorithm> // std::min()
#include <iostream>  // std::cin, std::cout
#include <stdlib.h>  // rand()

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/timer/timer.hpp>

namespace po = boost::program_options;

#define TRACE(x) std::cout << boost::format x
#ifdef DEBUG
#define TDEBUG(x) TRACE(x)
#else
#define TDEBUG(x)
#endif

inline uint64_t d2u(const double& d)
{
    return *((uint64_t*)(&d));
}

inline uint16_t get_exp(const double& d)
{
    return d2u(d) >> 52;
}

inline uint16_t get_mnt(const double& d)
{
    return d2u(d) & 0x1FFFFFFF;
}

// Generates a random positive double value
// 0<= value < 1e+200
// The uppper limit prevents double overflow when summing up an array of values
double generate_double()
{
    union {
        double d;
        uint8_t bytes[sizeof(double)];
    } u;
    u.d = -1.0;

    while (!(u.d >= 0 && u.d < 1e+200)) {
        for (auto& b : u.bytes) {
            b = rand() & 0xFF;
        }
    }
    TDEBUG(("u.d  = %.40e\n") % u.d);
    return u.d;
}

// Fills the vector with random double values
void generate_double_vector(std::vector<double>& v, size_t n)
{
    TRACE(("\nGenerating random vector...\n"));
    boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
    v.resize(n);
    std::generate(v.begin(), v.end(), generate_double);
}

// Sums up all double values of the given array
// O(N) - naive approach
double sum0(const std::vector<double>& v)
{
    double s = 0.0;
    for (auto x : v) {
        s += x;
    }
    return s;
}

// Sums up all double values of the given array
// O(N^2 lonN)
// Quicksort it, sum two first elements, re-sort with the sum
double sum1(std::vector<double>& v)
{
    if (v.empty()) {
        return 0.0;
    }
    for(size_t i = 0; i < v.size() - 1; ++i) {
        std::sort(v.begin()+i, v.end());
        v[i+1] += v[i];
    }
    return v.back();
}

// Sums up all double values of the given array
// O(N^2)
// Partial sort to have two min elements in the beginning of the vector
// Sum two first (min) elements, partial sort with the sum
double sum2(std::vector<double>& v)
{
    if (v.empty()) {
        return 0.0;
    }
    for(size_t i = 0; i < v.size() - 1; ++i) {
        std::partial_sort(v.begin() + i, v.begin() + i + 2, v.end());
        v[i+1] += v[i];
    }
    return v.back();
}

// Sums up all double values of the given array
// O(N lonN)
// Build BST (multiset), sum two first elements (min values),
// insert the sum into BST, repeat
double sum3(const std::vector<double>& v)
{
    std::multiset<double> set(v.begin(), v.end());
    while (set.size() > 1) {
        std::multiset<double>::const_iterator itA = set.begin();
        std::multiset<double>::const_iterator itB = ++set.begin();
        double c = *itA + *itB;
        set.erase(itA, ++itB);
        set.insert(c);
    }
    return !set.empty() ? *set.begin() : 0.0;
}

// Sums up all double values of the given array
// O(N lonN) but faster than BST
// Create a heap, sum two min values, insert the sum into the heap, repeat
double sumH(std::vector<double>& v)
{
    double d1, d2;
    std::make_heap(v.begin(), v.end(), std::greater<double>());

    while (v.size() > 1) {

        d1 = v.front();
        std::pop_heap(v.begin(), v.end(), std::greater<double>());
        v.pop_back();

        d2 = v.front();
        std::pop_heap(v.begin(), v.end(), std::greater<double>());
        v.pop_back();

        TDEBUG(("heap d1 = 0x%016x = % .40e\n"
                "heap d2 = 0x%016x = % .40e\n")
               % d2u(d1) % d1 % d2u(d2) % d2);

        v.push_back(d1 + d2);
        std::push_heap(v.begin(), v.end(), std::greater<double>());
    }
    return !v.empty() ? *v.begin() : 0.0;
}

// Sums up all double values of the given array
// O(N)
// 2 queues algorithm
double sumQ(std::vector<double>& v)
{
    std::queue<double> q_orig, q_sums;
    if (v.size() == 0) {
        return 0;
    }

    std::sort(v.begin(), v.end());
    for (auto x : v) {
        q_orig.push(x);
    }

    auto get_min = [&]() {
        double d = 0;
        if (!q_orig.empty() || !q_sums.empty()) {
            if (q_orig.empty()) {
                d = q_sums.front();
                q_sums.pop();
            } else if (q_sums.empty()) {
                d = q_orig.front();
                q_orig.pop();
            } else if (q_orig.front() < q_sums.front()) {
                d = q_orig.front();
                q_orig.pop();
            } else {
                d = q_sums.front();
                q_sums.pop();
            }
        }
        return d;
    };

    while (q_orig.size() > 0 || q_sums.size() > 1) {
        q_sums.push(get_min() + get_min());
    }

    return q_sums.front();
}

// Sums up all double values of the given array
// O(N) but not as accurate as O(N logN) solutions
// The idea is to devide the range of double values into segments.
// Values falling into the same segment can be summed without precision loss.
// In the end, all preliminary sums are summed up together.
double sumT(std::vector<double>& v)
{
    double s = 0.0;
    uint16_t exp = 0;

    // table of doubles, one element per double exponent
    double t[0x7FF];
    memset(t, 0x00, sizeof(t));

    for (auto d : v) {
        exp = get_exp(d);
        s = t[exp] + d;

        // take care of the carry
        while (get_exp(s) != exp) {
            t[exp] = 0.0;
            exp = get_exp(s);
            s = t[exp] + s;
        }

        t[exp] = s;
    }

    // sum up all values from the table
    s = 0.0;
    for (auto d : t) {
        if (d > 0) {
            s += d;
        }
    }
    return s;
}

const uint8_t SUM_1 = (1<<0);
const uint8_t SUM_2 = (1<<1);
const uint8_t SUM_3 = (1<<2);
const uint8_t SUM_H = (1<<3);
const uint8_t SUM_T = (1<<4);
const uint8_t SUM_Q = (1<<5);
const uint8_t SUM_ALL = 0xFF;

int main(int argc, char *argv[])
{
    size_t input_size = 10;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help")
        ("algo", po::value<std::string>()->default_value("sum3HQT"),
         "Sum algorithm:\n<sum1 | sum2 | sum3 | sum3HQT | all>")
        ("size", po::value<size_t>(&input_size)->default_value(input_size),
         "Size of the array");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    TRACE(("Algo: %s\n")
          % vm["algo"].as<std::string>());

    uint8_t sum = 0;
    if (vm["algo"].as<std::string>() == "sum1") {
        sum = SUM_1;
    } else if (vm["algo"].as<std::string>() == "sum2") {
        sum = SUM_2;
    } else if (vm["algo"].as<std::string>() == "sum3") {
        sum = SUM_3;
    } else if (vm["algo"].as<std::string>() == "sumH") {
        sum = SUM_H;
    } else if (vm["algo"].as<std::string>() == "sumQ") {
        sum = SUM_Q;
    } else if (vm["algo"].as<std::string>() == "sumT") {
        sum = SUM_T;
    } else if (vm["algo"].as<std::string>() == "sum3HQT") {
        sum = SUM_3 | SUM_H | SUM_Q | SUM_T;
    } else if (vm["algo"].as<std::string>() == "all") {
        sum = SUM_ALL;
    }

    srand(time(NULL));
    std::vector<double> input_vector, v;
    generate_double_vector(input_vector, input_size);
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, sT = 0.0, sH = 0.0, sQ = 0.0;

    double vmax = 0.0, vmin = 1.0;
    for (auto d : input_vector) {
        vmax = std::max(vmax, d);
        vmin = std::min(vmin, d);
    }
    TRACE(("vmin = 0x%016x = % .40e\n"
           "vmax = 0x%016x = % .40e\n")
          % d2u(vmin) % vmin
          % d2u(vmax) % vmax);

    // Naive sum
    {
        v = input_vector;
        TRACE(("\nCalculating sum0...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        s0 = sum0(v);
        TRACE(("sum0 = 0x%016x = % .40e\n")
              % d2u(s0) % s0);
    }

    if (sum & SUM_1) {
        v = input_vector;
        TRACE(("\nCalculating sum1...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        s1 = sum1(v);
        TRACE(("sum1 = 0x%016x = % .40e\n"
               "dif0 = 0x%016x = % .40e\n")
              % d2u(s1) % s1
              % d2u(s1 - s0) % (s1 - s0));
    }

    if (sum & SUM_2) {
        v = input_vector;
        TRACE(("\nCalculating sum2...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        s2 = sum2(v);
        TRACE(("sum2 = 0x%016x = % .40e\n"
               "dif0 = 0x%016x = % .40e\n")
              % d2u(s2) % s2
              % d2u(s2 - s0) % (s2 - s0));
    }

    if (sum & SUM_3) {
        v = input_vector;
        TRACE(("\nCalculating sum3...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        s3 = sum3(v);
        TRACE(("sum3 = 0x%016x = % .40e\n"
               "dif0 = 0x%016x = % .40e\n")
              % d2u(s3) % s3
              % d2u(s3 - s0) % (s3 - s0));
    }

    if (sum & SUM_H) {
        v = input_vector;
        TRACE(("\nCalculating sumH...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        sH = sumH(v);
        TRACE(("sumH = 0x%016x = % .40e\n"
               "dif0 = 0x%016x = % .40e\n"
               "dif3 = 0x%016x = % .40e\n")
              % d2u(sH) % sH
              % d2u(sH - s0) % (sH - s0)
              % d2u(sH - s3) % (sH - s3));
    }

    if (sum & SUM_Q) {
        v = input_vector;
        TRACE(("\nCalculating sumQ...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        sQ = sumQ(v);
        TRACE(("sumQ = 0x%016x = % .40e\n"
               "dif0 = 0x%016x = % .40e\n"
               "dif3 = 0x%016x = % .40e\n")
              % d2u(sQ) % sQ
              % d2u(sQ - s0) % (sQ - s0)
              % d2u(sQ - s3) % (sQ - s3));
    }

    if (sum & SUM_T) {
        v = input_vector;
        TRACE(("\nCalculating sum table...\n"));
        boost::timer::auto_cpu_timer t("Done in %t sec CPU, %w sec real\n");
        sT = sumT(v);
        TRACE(("sumT = 0x%016x = % .40e\n"
               "dif0 = 0x%016x = % .40e\n"
               "dif3 = 0x%016x = % .40e\n")
              % d2u(sT) % sT
              % d2u(sT - s0) % (sT - s0)
              % d2u(sT - s3) % (sT - s3));
    }

    return 0;
}
