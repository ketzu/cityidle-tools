//
// Created by david on 17.02.2019.
//
#include <string>
#include <map>

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


#endif //IDLESIM_GENERATOR_H
