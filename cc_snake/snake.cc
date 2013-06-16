#include <vector>
#include <iterator> 
#include <algorithm> // std::min()
#include <iostream>  // std::cin, std::cout

// Spiral-snake traversal of 2d array (n*m).
//
// For example, elements of the following array
// are traversed in ascending order:
//
//     11 11 11 12 
//     14 25 27 12 
//     14 13 13 13 

typedef void (*snake_cb)(int, int, int);

void go_snake_rec(int xmin, int ymin, int xmax, int ymax,
                  const std::vector< std::vector<int> > &v2d, snake_cb cb)
{
    if (xmin > xmax || ymin > ymax)
        return;
    
    int x, y;

    // 1. full top row
    // 2. right inner column
    // 3. full bottom row (if needed)
    // 4. left inner column (if needed)
    for (x = xmin,   y = ymin; y <= ymax; y++) cb(x, y, v2d[x][y]);
    for (x = xmin+1, y = ymax; x <  xmax; x++) cb(x, y, v2d[x][y]);
    if (xmin < xmax) for (x = xmax,   y = ymax; y >= ymin; y--) cb(x, y, v2d[x][y]);
    if (ymin < ymax) for (x = xmax-1, y = ymin; x >  xmin; x--) cb(x, y, v2d[x][y]);
    
    go_snake_rec(xmin+1, ymin+1, xmax-1, ymax-1, v2d, cb);
}

void go_snake(int n, int m,
              const std::vector< std::vector<int> > &v2d, snake_cb cb)
{
    struct { int dx; int dy; } dir[] = { {0,1}, {1,0}, {0,-1}, {-1,0} };

    // number of spiral turns
    int sp = (std::min(n,m) + 1) / 2; 
    
    for (int s = 0; s < sp; s++) {
        size_t x = s, y = s;
        int dirs = 4;
        int sx = m - s*2 - 1;
        int sy = n - s*2 - 1;

        // degenerated case (most inner loop)
        if (sx <= 0 || sy <= 0) {
            sx = (sx >= sy) ? sx + 1 : 0;
            sy = (sx >= sy) ? 0 : sy + 1;
            dirs = 2;
        }
        
        for (int d = 0; d < dirs; d++) {
            for (int k = 0; k < ((d % 2) ? sy : sx); k++) {
                cb(x, y, v2d[x][y]);
                x += dir[d].dx;
                y += dir[d].dy;
            }
        }
    }
}

void snake_out(int x, int y, int val)
{
    // std::cout << x << " " << y << ": "<< val << std::endl;
    std::cout << val << " ";
}

int main(int argc, char *argv[])
{
    size_t n = 0, m = 0;
    
    // read input
    std::cin >> n >> m;
    std::vector< std::vector<int> > v2d(n);
    for (size_t i = 0; i < n; i++) {
        std::copy_n(std::istream_iterator<int>(std::cin), m,
                    std::back_inserter(v2d[i]));
    }

    // echo input
    std::cout << "n = " << n << "; m = " << m << std::endl;
    for (size_t i = 0; i < n; i++) {
        std::copy(v2d[i].begin(), v2d[i].end(),
                  std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;
    }
    
    std::cout << "go_snake     ";
    go_snake(n, m, v2d, snake_out);
    std::cout << std::endl;
    
    std::cout << "go_snake_rec ";
    go_snake_rec(0, 0, n-1, m-1, v2d, snake_out);
    std::cout << std::endl;

    return 0;
}
