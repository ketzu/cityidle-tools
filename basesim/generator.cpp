//
// Created by david on 17.02.2019.
//
#include <sstream>
#include <cmath>

#include "generator.h"

double gainFormula(double level, double basegain) {
    return level*basegain;
}

Generator::Generator(double costfactor, double basecost, double basegain) : costfactor(costfactor), basecost(basecost), basegain(basegain)  {

}

double Generator::gain() const {
    return gainFormula(this->level, this->basegain);
}

double Generator::cost() const {
    return this->basecost*pow(this->costfactor,this->level);
}

double Generator::eff() const {
    auto tmpgain = this->basegain;
    if(this->updates.count(level+1))
        tmpgain *= this->updates.at(level+1);
    return gainFormula(this->level+1, tmpgain)/this->cost();
}

std::string Generator::toString() const {
    std::ostringstream strs;
    strs << this->basecost << " -> " << this->basegain << " (" << this->costfactor << ")";
    return strs.str();
}

void Generator::buy() {
    this->level++;
    if(this->updates.count(this->level)>0){
        this->basegain *= this->updates[this->level];
    }
}

void Generator::addupdate(int level, double mult) {
    this->updates[level] = mult;
}
