//
// Created by david on 17.02.2019.
//
#include <sstream>
#include <cmath>
#include <algorithm>

#include "generator.h"

double gainFormula(double level, double basegain, double mult, double boni) {
    return level*basegain*mult*boni;
}

Generator::Generator(double costfactor, double basecost, double basegain) : costfactor(costfactor), basecost(basecost), basegain(basegain)  {

}

double Generator::gain() const {
    return gainFormula(level, basegain, mult, boni);
}

double Generator::cost() const {
    return basecost*pow(costfactor,level);
}

double Generator::eff() const {
    auto tmpgain = basegain;
    if(updates.count(level+1))
        tmpgain *= updates.at(level+1);
    return gainFormula(level+1, tmpgain, mult, boni)/this->cost();
}

std::string Generator::toString() const {
    std::ostringstream strs;
    strs << basecost << " -> " << basegain << " (" << costfactor << ")";
    return strs.str();
}

void Generator::buy() {
    level++;
    if(updates.count(level)>0){
        basegain *= updates[level];
    }
}

void Generator::addupdate(int level, double mult) {
    this->updates[level] = mult;
}

Infrastrucutre::Infrastrucutre(const double costfactor, const double basecost, const double basemult, std::initializer_list<int> affecting) : affecting(affecting), costfactor(costfactor), basecost(basecost), basemult(basemult) {

}

double Infrastrucutre::mult() const {
    return pow(basemult,level);
}

double Infrastrucutre::cost() const {
    return this->basecost*pow(costfactor,level);
}

double Infrastrucutre::eff() const {
    return 0;
}

void Infrastrucutre::buy() {
    this->level++;
}

std::string Infrastrucutre::toString() const {
    std::ostringstream strs;
    strs << basecost << " -> " << basemult << " (" << costfactor << ")";
    return strs.str();
}

double Infrastrucutre::affmult(int i) const {
    if(std::any_of(affecting.begin(), affecting.end(), [i](const auto element){ return i==element; }))
        return mult();
    // Generator is not affecting this
    return 1;
}
