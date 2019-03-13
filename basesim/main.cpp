#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <random>
#include "generator.h"

/**
 * Computes the gains per tick function.
 * @tparam T
 * @tparam S
 * @param gens Generators to calculate income
 * @param infra Infrastrucutre to buff generators
 * @return
 */
template<typename T, typename S>
double tickgain(const T& gens, const S& infra, const double expmul) {
    double d = 0.1;
    auto index = 0;
    for(auto& g: gens) {
        double mult = 1.0;
        for(const auto& inf: infra)
            mult *= inf.affmult(index);
        d += g.gain()*mult;
        index+=1;
    }
    return d*(1+expmul);
}

/**
 * Decision function of which generator to buy with logging facility.
 * An entry in a CSV (; separated) is created whenever a building is bought.
 *  *
 * @tparam T
 * @tparam S
 * @param gens Generators that are used
 * @param infra Infrastructure in use
 * @param resource Resource that is used to buy a generator
 * @param ticks Current time of the game, used for logging only
 * @param inc Current income of the game, used for logging only
 * @param output logging facility, csv data is written to this ofstream
 */
template<typename T, typename S>
bool buy(T& gens, S& infra, double& resource, const double ticks, const double inc){
    // implement buy strategy
    auto minG = std::min_element(gens.begin(), gens.end(), [](const auto& gen1, const auto& gen2) {
        // cheapest
        //return gen1.cost() < gen2.cost();

        // most costeffective
        return gen1.eff() > gen2.eff();
    });
    if(infra.size()>0) {
        auto minI = std::min_element(infra.begin(), infra.end(), [](const auto &infra1, const auto &infra2) {
            // cheapest
            return infra1.cost() < infra2.cost();
        });

        // Try to buy infrastructure
        if (minI->cost() <= resource) {
            resource -= minI->cost();
            minI->buy();

            return true;
        }
    }

    // try to buy generator
    if(minG->cost() <= resource){
        resource -= minG->cost();
        minG->buy();

        return true;
    }
    return false;
}

/**
 * Calculate part of the expgain and mult functions
 *
 * @param z
 * @return exp(log/loglog)
 */
double calcstuff(const double z) {
    return exp(log(z)/log(log(z)));
}

/**
 * computes multiplier from experience value
 *
 * @param experience
 * @return mulitplier
 */
double expmul(const double experience, const double lockedexp) {
    const double effexp = experience-lockedexp;
    if(effexp<=0) return 0;
    double sig = effexp / 1000.0;
    if(sig>1) sig = 1;
    return ((1-sig)* 0.04 * effexp + sig* calcstuff(effexp));
}

/**
 * computes gain of experience for give resource value
 *
 * @param gained
 * @return experience gained
 */
double expgain(const double gained) {
    double sig1 = gained / 200000000000000000000.0;
    double sig2 = (gained-2000000000000) / 200000000000000000000.0;
    if(sig1>1) sig1 = 1;
    if(sig2>1) sig2 = 1;
    if(sig2<0) sig2 = 0;
    double precalc = gained / (1LL<<33);
    return (1-sig1)*sqrt(precalc)+sig2*calcstuff(gained);
}

/**
 * Creates a fresh instance of generators.
 *
 * @param bcost base cost used to calculate all generator parameters
 * @param gcost increase in cost per generator, multiplicative
 * @param bgain base gain of generators
 * @param ggain increase in gain per generator, multiplicative
 * @param gcostmul cost increase exponential of generators
 * @param upgrades matrix of upgrades indicating a level and a multiplicator for the upgrade, are mapped to generators in order
 */
auto generateGenerators(const double ggain, const double gcost, const double bgain, const double bcost, const std::vector<double>& gcostmul, std::vector<std::vector<std::pair<int,double>>>& upgrades) {
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

    return generators;
}

/**
 * Freshly generates infrastructure for a run, for now hardcoded
 *
 * @return vector of infratstructure
 */
auto generateInfrastrucutre(const int citylevel, const std::vector<int>& research) {
    // Generate all generators used in the game
    std::vector<Infrastrucutre> infrastrucutre;

    if(citylevel == 1) {
        infrastrucutre.push_back(Infrastrucutre(1.1,100000.0,1.015,{0, 2, 5}));
        infrastrucutre.push_back(Infrastrucutre(1.1,100000000.0,1.025,{3, 4, 6}));
        infrastrucutre.push_back(Infrastrucutre(1.1,100000000000.0,1.04,{1, 7}));
    }else if(citylevel == 2){
        // citylevel 2
        // research alternatives
        // 1
        switch(research[0]) {
            case 0: infrastrucutre.push_back(Infrastrucutre(1.15,100000.0,1.045,{0, 2, 5})); break;
            case 1: infrastrucutre.push_back(Infrastrucutre(1.15,100000.0,1.015,{0, 1, 2, 3, 5, 7})); break;
            case 2: infrastrucutre.push_back(Infrastrucutre(1.09,100000.0,1.015,{0, 2, 5})); break;
            default: infrastrucutre.push_back(Infrastrucutre(1.15,100000.0,1.015,{0, 2, 5}));
        }

        // 2
        switch(research[1]) {
            case 0: infrastrucutre.push_back(Infrastrucutre(1.1,100000000.0,1.035,{3, 4, 6})); break;
            case 2: infrastrucutre.push_back(Infrastrucutre(1.089,100000000.0,1.025,{3, 4, 6})); break;
            default: infrastrucutre.push_back(Infrastrucutre(1.1,100000000.0,1.025,{3, 4, 6}));
        }

        // 3
        switch(research[2]) {
            case 0: infrastrucutre.push_back(Infrastrucutre(1.1,100000000000.0,1.045,{1, 7})); break;
            case 2: infrastrucutre.push_back(Infrastrucutre(1.088,100000000000.0,1.04,{1, 7})); break;
            default: infrastrucutre.push_back(Infrastrucutre(1.1,100000000000.0,1.04,{1, 7}));
        }

        // 4
        switch(research[3]) {
            case 0: infrastrucutre.push_back(Infrastrucutre(1.1,1000000000000000000.0,1.045,{0, 1, 2})); break;
            case 2: infrastrucutre.push_back(Infrastrucutre(1.089,1000000000000000000.0,1.04,{0, 1, 2})); break;
            default: infrastrucutre.push_back(Infrastrucutre(1.1,1000000000000000000.0,1.04,{0, 1, 2}));
        }

        // 5
        switch(research[4]) {
            case 0: infrastrucutre.push_back(Infrastrucutre(1.1,100000000000000000000000.0,1.06,{4, 5, 6})); break;
            case 2: infrastrucutre.push_back(Infrastrucutre(1.09,100000000000000000000000.0,1.04,{4, 5, 6})); break;
            default: infrastrucutre.push_back(Infrastrucutre(1.1,100000000000000000000000.0,1.04,{4, 5, 6}));
        }
    }
    return infrastrucutre;
}

/**
 * Simulator function, runs a full game until 100 of the final generator are reached.
 *
 * @param toexp start exp for run
 * @param toexp target exp for reset
 * @param bcost base cost used to calculate all generator parameters
 * @param gcost increase in cost per generator, multiplicative
 * @param bgain base gain of generators
 * @param ggain increase in gain per generator, multiplicative
 * @param gcostmul cost increase exponential of generators
 * @param upgrades matrix of upgrades indicating a level and a multiplicator for the upgrade, are mapped to generators in order
 * @param filename filename of the csv log
 * @return Time used to reach level 100 of highest generator
 */
auto simulate(const double toexp, const double bcost, const double gcost, const double bgain, const double ggain, std::vector<double> gcostmul, std::vector<std::vector<std::pair<int,double>>>& upgrades, const double startexp=0, std::string filename = "") {
    // file outputstream
    std::ofstream output;

    // drawing for research
    std::default_random_engine rnd;
    std::exponential_distribution<double> dist(0.02);

    // Used to indicate a set filename
    bool verbose = false;
    if(filename != ""){
        verbose = true;
        output.open(filename);
        output << "ticks;farm;inn;store;bank;data;factory;energy;casino;income" << std::endl;
    }

    auto generators = generateGenerators(ggain, gcost, bgain, bcost, gcostmul, upgrades);
    const auto target_research = std::vector<int>({1,0,0,1,1});
    auto res_cost = std::vector<int>({1000, 50000, 200000, 500000, 1000000});
    auto citylevel = 0;
    auto infrastructure = generateInfrastrucutre(citylevel,target_research);

    // Simulation loop
    // Income
    double inc;
    // Resource
    double resource = 0;
    double allgain = 0;
    // experience
    double exp = startexp;
    double locked = 0.0;
    double research_mult = 1.0;
    // Time
    uint64_t ticks = 0;
    double allticks = 0;
    std::vector<uint64_t> resetlist;
    do{
        if(resource<0){
            std::cout << "PANIC" << std::endl;
            return resetlist;
        }

        // compute income for this round
        auto expmult =  expmul(exp,locked);
        inc = tickgain(generators, infrastructure,expmult);
        resource += inc;
        allgain += inc;

        // buy new generators
        bool bought = buy(generators, infrastructure, resource, ticks, inc);

        // reset if possible and we gain at least previous exp amount
        if(generators.back().level>=85 && expgain(allgain) >= exp && bought){
            // log on reset
            resetlist.push_back(ticks);

            // gain experience
            exp += expgain(allgain);

            if(citylevel==0 && exp>=1000){
                citylevel = 1;
                exp = 0;
                std::cout << "!";
            }else if(citylevel==1 && exp>=100000){
                citylevel = 2;
                exp = 0;
                std::cout << "!";
            }else{
                // print dot for progress
                std::cout << ".";
            }

            // decide which researches are applicable
            locked = 0.0;
            auto research = target_research;
            for(auto i=0; i<research.size(); i+=1){
                // block research if it is too expensive
                if(exp >= 2*res_cost[i]){
                    research[i] = -1;
                    locked -= res_cost[i];
                }
            }

            // reset non exp stats
            generators = generateGenerators(ggain, gcost, bgain, bcost, gcostmul, upgrades);
            infrastructure = generateInfrastrucutre(citylevel,research);
            resource = 0;
            allgain = 0;

            // apply special researches
            if(research[1]==1){
                for(auto& g: generators){
                    g.mult = 1.001;
                }
            }
            if(research[3]==1){
                for(auto& g: generators){
                    g.boni = std::min(dist(rnd),1000.0)+4;
                }
            }
            if(research[4]==1){
                research_mult = 2.0;
            }else{
                research_mult = 1.0;
            }

            // we measure play length per run
            allticks += ticks;
            ticks = 0;
        }

        // stop if condition for soft reset is met
        ticks++;
    }while(exp < toexp);

    if(verbose)
        output.close();

    // return the time used to reach the goal
    std::cout << std::endl;
    return resetlist;
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
    auto resets = simulate(1000000000, bcost, gcost, bgain, ggain, costs, upgrades, 20);

    auto ticks = std::accumulate(resets.begin(), resets.end(), 0, std::plus<uint64_t>());

    // changed return to diffs
    //decltype(resets) diffs;
    //for(int i=0; i<resets.size(); i++)
    //    diffs.push_back(resets[i]-resets[i-1]);

    auto maxit = std::max_element(resets.begin(), resets.end());
    auto max = *maxit;
    auto maxpos = std::distance(resets.begin(), maxit);

    std::cout << std::endl;
    std::cout << "Resets after: " << std::endl;
    for(auto& ts : resets) {
        std::cout << ts << " or " << ts/10/60/60 << " hours." << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Used " << ticks << " ticks." << std::endl;
    std::cout << "Thats " << ticks/10/60/60 << " hours." << std::endl;

    std::cout << std::endl;
    std::cout << "longest stretch " << max << " ticks." << std::endl;
    std::cout << "Thats " << max/10/60/60 << " hours." << std::endl;
    std::cout << "That's reset nr. " << maxpos+1 << " of " << resets.size() << " resets." << std::endl;

    return 0;
}