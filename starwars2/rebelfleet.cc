/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#include <cassert>
#include "rebelfleet.h"

// Metody RebelStarship
RebelStarship::RebelStarship(StarThing::ShieldPoints shield, RebelStarship::Speed speed) : Starship(shield),
                                                                                           speed(speed) {}

void RebelStarship::reactToAttack(ImperialStarship &enemy) {
    takeDamage(enemy.getAttackPower());
}

RebelStarship::Speed RebelStarship::getSpeed() const { return speed; }


// Metody Explorer
bool Explorer::isSpeedCorrect(const RebelStarship::Speed &speed) const {
    return (speed >= 299796 && speed <= 2997960);
}

Explorer::Explorer(StarThing::ShieldPoints shield, RebelStarship::Speed speed) : RebelStarship(shield, speed) {
    assert(isSpeedCorrect(speed));
}


// Metody AttackingRebelStarship
AttackingRebelStarship::AttackingRebelStarship(StarThing::ShieldPoints shield, RebelStarship::Speed speed,
                                               Attacking::AttackPower attackPower)
        : RebelStarship(shield, speed), AttackingGivenPower(attackPower) {}

void AttackingRebelStarship::reactToAttack(ImperialStarship &enemy) {
    RebelStarship::reactToAttack(enemy);
    enemy.takeDamage(getAttackPower());
}


// Metody StarCruiser
bool StarCruiser::isSpeedCorrect(const RebelStarship::Speed &speed) const {
    return (speed >= 99999 && speed <= 299795);
}

StarCruiser::StarCruiser(StarThing::ShieldPoints shield, RebelStarship::Speed speed, Attacking::AttackPower attackPower)
        : AttackingRebelStarship(shield, speed, attackPower) {
    assert(isSpeedCorrect(speed));
}


// Metody StarCruiser
bool XWing::isSpeedCorrect(const RebelStarship::Speed &speed) const {
    return (speed >= 299796 && speed <= 2997960);
}

XWing::XWing(StarThing::ShieldPoints shield, RebelStarship::Speed speed, Attacking::AttackPower attackPower)
        : AttackingRebelStarship(shield, speed, attackPower) {
    assert(isSpeedCorrect(speed));
}


std::shared_ptr<Explorer>
createExplorer(Explorer::ShieldPoints shield, Explorer::Speed speed) {
    return std::make_shared<Explorer>(shield, speed);
}

std::shared_ptr<StarCruiser>
createStarCruiser(StarCruiser::ShieldPoints shield, StarCruiser::Speed speed, XWing::AttackPower attack) {
    return std::make_shared<StarCruiser>(shield, speed, attack);
}

std::shared_ptr<XWing>
createXWing(StarCruiser::ShieldPoints shield, StarCruiser::Speed speed, XWing::AttackPower attack) {
    return std::make_shared<XWing>(shield, speed, attack);
}
