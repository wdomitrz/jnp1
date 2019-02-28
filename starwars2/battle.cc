/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#include <cassert>
#include <iostream>
#include "battle.h"


// Metody SpaceTime
SpaceBattle::SpaceTime::SpaceTime(SpaceBattle::SpaceTime::Time t0, SpaceBattle::SpaceTime::Time t1) : currentTime(t0),
                                                                                                      t1(t1) {
    assert(t0 <= t1);
}

bool SpaceBattle::SpaceTime::isNowAttackTime() const {
    return (currentTime % 2 == 0 || currentTime % 3 == 0) && currentTime % 5 != 0;
}

void SpaceBattle::SpaceTime::moveTime(SpaceBattle::SpaceTime::Time timeStep) {
    currentTime += timeStep;
    currentTime %= (t1 + 1);
}


// Metody SpaceBattle
SpaceBattle::SpaceBattle(SpaceTime::Time t0, SpaceTime::Time t1,
                         std::vector<std::shared_ptr<ImperialStarship>> &imperials,
                         std::vector<std::shared_ptr<RebelStarship>> &rebels) :
        spaceTime(t0, t1), imperials(imperials), rebels(rebels) {}

bool SpaceBattle::hasFinished() {

    // Sprawdzenie czy już kiedyś bitwa się skończyła. Jeśli tak to nie ma
    // sensu nic robić oprócz wypisywania wiadomości.
    if (hasFinishedFlag)
        return true;

    // Zapisanie aktualnych rozmiarów obydwu armi
    size_t imperialFleetCounter = countImperialFleet();
    size_t rebelFleetCounter = countRebelFleet();

    // Sprawdzenie, czy bitwa powinna być dalej kontynuowana
    if (imperialFleetCounter == 0 || rebelFleetCounter == 0) {

        // Zapisanie informacji o tym, że bitwa się już zakończyła
        hasFinishedFlag = true;

        // Zapisanie informacji kto wygrał
        if (imperialFleetCounter != 0)
            resultOfTheBattle = "IMPERIUM WON\n";
        else if (rebelFleetCounter != 0)
            resultOfTheBattle = "REBELLION WON\n";
        else
            resultOfTheBattle = "DRAW\n";

        // Przejście do wypisania rezultatu bitwy
        return true;
    }

    // Jeśli jest jeszcze wystarczająco statków, to kontynuujemy bitwę
    return false;
}

void SpaceBattle::attack() {
    // dla każdego statku Imperium
    for (auto &imperial : imperials)
        // dla każdego statku Rebelii
        for (auto &rebel : rebels)
            // jeśli oba statki nie nie zostały jeszcze zniszczone,
            if (imperial->isAlive() && rebel->isAlive())
                // statek Imperium atakuje statek Rebelii.
                rebel->reactToAttack(*imperial);
}

size_t SpaceBattle::countImperialFleet() const {
    return countShips(imperials);
}

size_t SpaceBattle::countRebelFleet() const {
    return countShips(rebels);
}

void SpaceBattle::tick(SpaceTime::Time timeStep) {
    // Sprawdzenie, czy bitwa już się skończyła
    if (hasFinished()) {
        std::cout << resultOfTheBattle;
        return;
    }

    if (spaceTime.isNowAttackTime())
        attack();

    spaceTime.moveTime(timeStep);
}


// Metody Builder
SpaceBattle::Builder &SpaceBattle::Builder::startTime(SpaceTime::Time t) {
    ist0Set = true;
    t0 = t;
    return *this;
}

SpaceBattle::Builder &SpaceBattle::Builder::maxTime(SpaceTime::Time t) {
    ist1Set = true;
    t1 = t;
    return *this;
}

SpaceBattle::Builder &SpaceBattle::Builder::ship(const std::shared_ptr<RebelStarship> &starship) {
    rebels.emplace_back(starship);
    return *this;
}

SpaceBattle::Builder &SpaceBattle::Builder::ship(const std::shared_ptr<ImperialStarship> &starship) {
    imperials.emplace_back(starship);
    return *this;
}

SpaceBattle SpaceBattle::Builder::build() {
    if (!ist0Set || !ist1Set)
        throw std::logic_error("Both t0 and t1 have to be set!");
    return SpaceBattle(t0, t1, imperials, rebels);
}

