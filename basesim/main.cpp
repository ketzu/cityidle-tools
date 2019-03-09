#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>
#include <cmath>
#include "generator.h"

/**
 * Computes the gains per tick function.
 * @tparam T
 * @param gens
 * @return
 */
template<typename T>
double tickgain(const T& gens, const double exp, const double expmul) {
    double d = 0.1;
    for(auto& g: gens) {
        d += g.gain();
    }
    return d*(1+exp*expmul);
}

/**
 * Decision function of which generator to buy with logging facility.
 * An entry in a CSV (; separated) is created whenever a building is bought.
 *  *
 * @tparam T
 * @param gens Generators that are used
 * @param resource Resource that is used to buy a generator
 * @param ticks Current time of the game, used for logging only
 * @param inc Current income of the game, used for logging only
 * @param output logging facility, csv data is written to this ofstream
 */
template<typename T>
bool buy(T& gens, double& resource, const double ticks, const double inc, std::ofstream& output){
    // implement buy strategy
    auto min = std::min_element(gens.begin(), gens.end(), [](const auto& gen1, const auto& gen2) {
        // cheapest
        //return gen1.cost() < gen2.cost();

        // most costeffective
        return gen1.eff() > gen2.eff();
    });
    if(min->cost() <= resource){
        resource -= min->cost();
        min->buy();

        std::array<int,8> levels;

        // calculate levels
        std::transform(gens.begin(), gens.end(), levels.begin(), [](const auto& gen) -> int {
            return gen.level;
        });

        // print current time, log10 of gains per generator and full income
        output << ticks << ";";
        for(auto& g : gens) {
            auto tmp = g.gain();
            if(tmp>0)
                output << log10(tmp) << ";";
            else
                output << ";";
        }
        output << inc << std::endl;
        return true;
    }
    return false;
}

/**
 * Simulator function, runs a full game until 100 of the final generator are reached.
 *
 * @param bcost base cost used to calculate all generator parameters
 * @param gcost increase in cost per generator, multiplicative
 * @param bgain base gain of generators
 * @param ggain increase in gain per generator, multiplicative
 * @param exp Experience level of this run
 * @param expmul multiplicator of experience
 * @param gcostmul cost increase exponential of generators
 * @param upgrades matrix of upgrades indicating a level and a multiplicator for the upgrade, are mapped to generators in order
 * @param filename filename of the csv log
 * @return Time used to reach level 100 of highest generator
 */
auto simulate(const double bcost, const double gcost, const double bgain, const double ggain, const double exp, const double expmul, std::vector<double> gcostmul, std::vector<std::vector<std::pair<int,double>>>& upgrades, std::string filename = "") {
    // file outputstream
    std::ofstream output;
    // Used to indicate a set filename
    bool verbose = false;
    if(filename != ""){
        verbose = true;
        output.open(filename);
        output << "ticks;farm;inn;store;bank;data;factory;energy;casino;income" << std::endl;
    }

    // temporary gain variables for easier computation
    double tgain = 1;
    double tcost = 1;

    // Generate all generators used in the game
    std::vector<Generator> generators;
    for(int i=0; i<gcostmul.size(); i++) {
        auto g = Generator(gcostmul[i], bcost*tcost, bgain*tgain);
        for(auto& u: upgrades[i]){
            g.addupdate(u.first,u.second);
        }
        generators.push_back(g);
        tgain *= ggain;
        tcost *= gcost;
    }

    // Simulation loop
    // Income
    double inc;
    // Resource
    double resource = 0;
    // Time
    long ticks = 0;
    std::vector<long> buys;
    do{
        if(resource<0){
            std::cout << "PANIC" << std::endl;
            return buys;
        }

        // compute income for this round
        inc = tickgain(generators, exp, expmul);
        resource += inc;

        // buy new generators
        bool bought = buy(generators, resource, ticks, inc, output);

        // log current time if something is bought, for interactivity analysis
        if(bought)
            buys.push_back(ticks);

        // stop if condition for soft reset is met
        ticks++;
    }while(generators.rbegin()->level < 85);

    if(verbose)
        output.close();

    // return the time used to reach the goal
    return buys;
}

int main() {
    // Generator parameters used in the game
    const auto bcost = 10;
    const auto gcost = 13;

    const auto bgain = 0.1;
    const auto ggain = 8;

    // List of current upgrades
    std::vector<std::vector<std::pair<int,double>>> upgrades(8);

    upgrades[0].push_back({25,3});
    upgrades[0].push_back({50,5});
    upgrades[0].push_back({100,15});
    upgrades[0].push_back({130,10});
    upgrades[0].push_back({170,12});
    upgrades[0].push_back({200,18});

    upgrades[1].push_back({30,3});
    upgrades[1].push_back({60,3});
    upgrades[1].push_back({90,12});
    upgrades[1].push_back({120,15});
    upgrades[1].push_back({150,12});

    upgrades[2].push_back({15,4});
    upgrades[2].push_back({40,4});
    upgrades[2].push_back({80,5});
    upgrades[2].push_back({140,11});

    upgrades[3].push_back({35,5});
    upgrades[3].push_back({70,8});
    upgrades[3].push_back({105,4});
    upgrades[3].push_back({140,9});
    upgrades[3].push_back({180,13});

    upgrades[4].push_back({40,4});
    upgrades[4].push_back({80,7});
    upgrades[4].push_back({120,5});
    upgrades[4].push_back({160,9});

    upgrades[5].push_back({10,3});
    upgrades[5].push_back({40,4});
    upgrades[5].push_back({80,8});

    upgrades[6].push_back({50,3});
    upgrades[6].push_back({90,8});

    upgrades[7].push_back({75,2});
    upgrades[7].push_back({150,4});

    std::vector<double> costs = {1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.095};

    // Simulate
    auto buys = simulate(bcost, gcost, bgain, ggain, 0, 0.04, costs, upgrades, "");

    long ticks = buys.back()+1;

    std::cout << std::endl;
    std::cout << "Used " << ticks << " ticks." << std::endl;
    std::cout << "Thats " << ticks/10/60/60 << " hours." << std::endl;


    decltype(buys) diffs;
    for(int i=0; i<buys.size(); i++)
        diffs.push_back(buys[i]-buys[i-1]);

    auto maxit = std::max_element(diffs.begin(), diffs.end());
    auto max = *maxit;
    auto maxpos = std::distance(diffs.begin(), maxit);

    std::cout << std::endl;
    std::cout << "Longest stretch " << max << " ticks." << std::endl;
    std::cout << "Thats " << max/10/60/60 << " hours." << std::endl;
    std::cout << "At pos " << maxpos << " of " << buys.size() << "." << std::endl;

    return 0;
}