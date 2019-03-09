#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <random>

using namespace std;

// ----------------- C++ version of the grid evaluation code of the game -------------------
template<typename T>
auto getPlusNeighbors(T &grid, uint8_t x, uint8_t y) {
    vector<uint8_t> ngbrs;
    if (x != 0) {
        ngbrs.push_back(grid[x - 1][y]);
    }
    if (x != 4) {
        ngbrs.push_back(grid[x + 1][y]);
    }
    if (y != 0) {
        ngbrs.push_back(grid[x][y - 1]);
    }
    if (y != 4) {
        ngbrs.push_back(grid[x][y + 1]);
    }
    return ngbrs;
};

template<typename T>
auto getXNeighbors(T &grid, uint8_t x, uint8_t y) {
    vector<uint8_t> ngbrs;
    if (x != 0 && y != 0) {
        ngbrs.push_back(grid[x - 1][y - 1]);
    }
    if (x != 4 && y != 4) {
        ngbrs.push_back(grid[x + 1][y + 1]);
    }
    if (x != 4 && y != 0) {
        ngbrs.push_back(grid[x + 1][y - 1]);
    }
    if (x != 0 && y != 4) {
        ngbrs.push_back(grid[x - 1][y + 1]);
    }
    return ngbrs;
};

template<typename T>
auto commercialEffect(T &grid, uint8_t x, uint8_t y) {
    auto effect = 1.1;
    auto rescounter = 0;
    auto indcounter = 0;
    auto comcounter = 0;
    for (auto &pn : getPlusNeighbors(grid, x, y)) {
        if (pn == 3) {
            indcounter += 1;
            if (indcounter < 4)
                effect *= 2.35;
            else
                effect *= 0.9;
        }
        if (pn == 2) {
            rescounter += 1;
            if (rescounter < 4)
                effect *= 2.3;
            else
                effect *= 3.75;
        }
        if (pn == 1) {
            comcounter += 1;
            if (comcounter < 3)
                effect *= 2.52;
            else
                effect *= 0.83;
        }
    }
    for (auto xn : getXNeighbors(grid, x, y)) {
        if (xn == 2) {
            rescounter += 1;
            if (rescounter < 3)
                effect *= 1.8;
            else
                effect *= 1.2;
        }
        if (xn == 1) {
            comcounter += 1;
            if (comcounter < 3)
                effect *= 2.6;
        }
    }
    return effect;
}

template<typename T>
auto residentialEffect(T &grid, uint8_t x, uint8_t y) {
    auto effect = 1.1;
    auto rescounter = 0;
    auto comcounter = 0;
    for (auto pn : getPlusNeighbors(grid, x, y)) {
        if (pn == 3)
            effect /= 2;
        if (pn == 2) {
            rescounter += 1;
            if (rescounter < 4)
                effect *= 2.4;
            else
                effect *= 1.6;
        }
        if (pn == 1) {
            comcounter += 1;
            if (comcounter < 3)
                effect *= 3.2;
            else
                effect *= 0.8;
        }
    }
    for (auto xn : getXNeighbors(grid, x, y)) {
        if (xn == 2) {
            rescounter += 1;
            if (rescounter < 3)
                effect *= 2.2;
            else
                effect *= 1.4;
        }
        if (xn == 1) {
            comcounter += 1;
            if (comcounter < 3)
                effect *= 2.3;
            else
                effect *= 0.9;
        }
    }
    return effect;
};

template<typename T>
auto industrialEffect(T &grid, uint8_t x, uint8_t y) {
    auto effect = 1.1;
    auto rescounter = 0;
    auto comcounter = 0;
    auto pns = getPlusNeighbors(grid, x, y);
    auto xns = getXNeighbors(grid, x, y);
    pns.insert(pns.end(), xns.begin(), xns.end());
    for (auto pn : pns) {
        if (pn == 3) {
            effect *= 1.3;
        }
        if (pn == 2) {
            rescounter += 1;
            if (rescounter < 3)
                effect *= 3.3;
            else
                effect *= 1.5;
        }
        if (pn == 1) {
            comcounter += 1;
            if (comcounter < 3)
                effect *= 2.6;
            else
                effect *= 0.7;
        }
    }
    return effect;
};

template<typename T>
auto evalGrid(T &grid) {
    array<double,3> values = {1, 1, 1};
    for (auto x = 0; x < 5; x += 1) {
        for (auto y = 0; y < 5; y += 1) {
            switch (grid[x][y]) {
                case 1:
                    values[0] += commercialEffect(grid, x, y);
                    break;
                case 2:
                    values[1] += residentialEffect(grid, x, y);
                    break;
                case 3:
                    values[2] += industrialEffect(grid, x, y);
                    break;
            }
        }
    }
    return values;
};

// ------------------------------------ Helper Function -------------------------------------

void printarray(array<array<uint8_t,5>,5>& G) {
    for(auto g : G){
        for(auto e : g){
            cout << "\t" << (int)e;
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    // grid we are working with
    array<array<uint8_t,5>,5> grid;

    // Max values for the three categories and respective grids
    double commax = 0.0; array<array<uint8_t,5>,5> com;
    double retmax = 0.0; array<array<uint8_t,5>,5> ret;
    double indmax = 0.0; array<array<uint8_t,5>,5> ind;

    // We will use random values as the implementation is inefficient
    // and it will take forever to try all 847288609443 options
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis(0, 847288609442);

    // loop parameter for how many random ones we try
    for(uint64_t e = 0; e<10000000 ; e++){

        // fill the grid with values derived from the random string
        uint64_t tmp = dis(gen);
        for(auto x=0; x<5; x++){
            for(auto y=0; y<5; y++){
                grid[x][y] = tmp % 3 +1;
                tmp -= tmp%3;
                tmp /= 3;
            }
        }

        // update maximua
        auto vals = evalGrid(grid);
        if(vals[0] > commax) {
            commax = vals[0];
            com = grid;
        }
        if(vals[1] > retmax){
            retmax  = vals[1];
            ret = grid;
        }
        if(vals[2] > indmax){
            indmax = vals[2];
            ind = grid;
        }
    }

    // finished, output values
    cout << "C: " << commax << " \tR: " << retmax << " \tI: " << indmax << endl;

    cout << endl;
    cout << "Max found com grid:" << endl;
    printarray(com);
    cout << endl;

    cout << endl;
    cout << "Max found ret grid:" << endl;
    printarray(ret);

    cout << endl;
    cout << "Max found ind grid:" << endl;
    printarray(ind);
    return 0;
}