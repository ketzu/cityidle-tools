//
// Created by david on 17.02.2019.
//
#include <string>
#include <map>
#include <vector>

#ifndef IDLESIM_GENERATOR_H
#define IDLESIM_GENERATOR_H

/**
 * Implements a generator of an idlegame, that produces resources per tick.
 */
class Generator {
    std::map<int,double> updates;
    double costfactor;
    double basecost;
    double basegain;

public:
    // Public as this is a private project and I believe I know that I am building a bikeshed
    long level = 0;
    // set by special research
    double boni = 1;
    // set by other research
    double mult = 1;

    // cost is basecost*costfactor^level, gain is basegain*level
    Generator(double costfactor, double basecost, double basegain);

    // gain = basegain*level
    double gain() const;

    // cost = basecost*costfactor^level
    double cost() const;

    // efficiency is gain of next level divided by cost
    double eff() const;

    // Increase level by one and apply possible upgrades
    void buy();

    // Add an upgrade to this generator (Should be in Constructor, but too lazy)
    void addupdate(int level, double mult);

    std::string toString() const;
};

class Infrastrucutre {
    std::vector<int> affecting;
    double costfactor;
    double basecost;
    double basemult;

public:
    // Public as this is a private project and I believe I know that I am building a bikeshed
    long level = 0;

    // cost is basecost*costfactor^level, gain mult basemult^level
    Infrastrucutre(const double costfactor, const double basecost, const double basemult, std::initializer_list<int> affecting);

    // mult = basegain*level
    double mult() const;

    // cost = basecost*costfactor^level
    double cost() const;

    // efficiency is gain of next level divided by cost
    double eff() const;

    // Increase level by one and apply possible upgrades
    void buy();

    // Returns multiplier for a given generator
    double affmult(int i) const;

    std::string toString() const;
};

#endif //IDLESIM_GENERATOR_H
