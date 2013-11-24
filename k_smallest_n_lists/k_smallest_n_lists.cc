// g++ --std=c++11 k_smallest_n_lists.cc
// ./a.out input

// Algorithm:
// 1) Sort all (N) input lists (vectors) and for each list store its min and max
//    values in another vector (vmm)
// 2) Sort vmm vector
// 3) Iterate through vmm vector, keeping track of min/max values:
//    3.1) if a min value is seen => add vector to the active hash,
//    3.2) if a max value is seen => remove vector from the active hash
//    3.3) when a vector is removed, done++ (initially done=0)
//
//    If (done < k) and (done + active) >= k, every next value can be k-smallest
//    so keep track of intersactions of active ranges (using binary search)

// Running time:
// initial soring of N lists: O(N * MlogM), where M is the input lists max size
// iterating through vmm vector: O(2N * logM)
// = O(N*MlogM + 2N*logM) = O(N*MlogM)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>        // pair
#include <functional>     // greater
#include <iterator>       // istream_iterator
#include <memory>         // shared_ptr
#include <algorithm>      // copy_n, sort
#include <stdint.h>       // uint32_t

using value = uint32_t;
using vvector = std::vector<value>;
using pair_vec_val = std::pair<const vvector*, value>;
auto comp = std::less<value>(); // std::greater<value>();

struct problem_data
{
    size_t n;
    size_t k;
    std::vector< vvector > lists;
};

size_t count_k_order_n_lists(std::shared_ptr<problem_data> pd)
{
    // check k and n are consistent
    if (!(pd->k > 0 && pd->k <= pd->n)) {
        return 0;
    }

    // min/max values from all input vectors
    std::vector<pair_vec_val> vmm;

    // sort all input vectors
    for (auto& v : pd->lists) {
        std::sort(v.begin(), v.end(), comp);
        // store the min and max values of the vector
        vmm.push_back({&v, *v.begin()});
        vmm.push_back({&v, *v.rbegin()});
    }

    // sort the list of min/max values from all vectors
    std::sort(vmm.begin(), vmm.end(),
              [](const pair_vec_val& p1, const pair_vec_val& p2) {
                  return comp(p1.second, p2.second);
              });

    // map: vector => position from which numbers might be k-smallest
    std::unordered_map<const vvector*, typename vvector::const_iterator> active;
    vvector result;
    size_t done = 0;

    // slide through min/max values from all ranges/vectors
    for (auto& vec_val : vmm) {

        // check if the current vector is in the active hash or not:
        // it is not, if the vec_val pair has the min value of the vector,
        // otherwise the vector has been added to active when the min was seen
        auto act_itr = active.find(vec_val.first);
        if (act_itr == active.end()) {
            // active does not contain the vector => it's the min value, add it
            auto start_it = vec_val.first->begin();
            if ((done + active.size()) < (pd->k - 1)) {
                // too few vectors processed so far => set position undefined
                start_it = vec_val.first->end();

            } else if ((done + active.size()) == (pd->k - 1)) {
                // this is the first time a new value can be k-smallest
                // => reset positions of all currently active vectors
                for (auto& vec_itr : active) {
                    vec_itr.second = std::lower_bound(vec_itr.first->begin(),
                                                      vec_itr.first->end(),
                                                      vec_val.second, comp);
                }
            }
            // add the current vector to active
            active[vec_val.first] = start_it;

        } else {
            // active contains the vector => it's the max value,
            // copy possible k-smallest values and remove the vector from active
            std::copy(act_itr->second, vec_val.first->end(),
                      std::back_inserter(result));
            active.erase(act_itr);
            done++;

            if (done >= pd->k) {
                // if we have processed more than k max values from k vectors,
                // there is no need to search further => copy possible matches
                // from the active hash and break
                for (auto& vec_itr : active) {
                    std::copy(vec_itr.second,
                              std::upper_bound(vec_itr.second,
                                               vec_itr.first->end(),
                                               vec_val.second, comp),
                              std::back_inserter(result));
                }
                break;
            }
        }
    }
    return result.size();
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
    for (int i = 0; i < tcs; i++) {

        // read test case data
        auto pd = std::make_shared<problem_data>();
        ifs >> pd->n;
        ifs >> pd->k;

        // read and create n lists
        pd->lists.resize(pd->n);
        for (uint32_t i = 0; i < pd->n; i++) {
            uint32_t listlen = 0;
            ifs >> listlen;
            pd->lists[i].reserve(listlen);
            std::copy_n(std::istream_iterator<uint32_t>(ifs), listlen,
                        std::back_inserter(pd->lists[i]));
        }

        // solve the test case
        auto cnt = count_k_order_n_lists(pd);

        std::cout << "TC" << (i+1) << ": "
                  << "n = " << pd->n << ", k = " << pd->k
                  << ", cnt = " << cnt << std::endl;
    }

    ifs.close();
    return 0;
}
