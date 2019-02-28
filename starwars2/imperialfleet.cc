/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#include "imperialfleet.h"


// Metody SingleImperialStarship
SingleImperialStarship::SingleImperialStarship(StarThing::ShieldPoints shield, Attacking::AttackPower attackPower)
        : Starship(shield),
          AttackingGivenPower(attackPower) {}

StarThing::ShieldPoints SingleImperialStarship::getShield() const {
    return Starship::getShield();
}

void SingleImperialStarship::takeDamage(Attacking::AttackPower damage) {
    return Starship::takeDamage(damage);
}

Attacking::AttackPower SingleImperialStarship::getAttackPower() const {
    return AttackingGivenPower::getAttackPower();
}

bool SingleImperialStarship::isAlive() const {
    return Starship::isAlive();
}

size_t SingleImperialStarship::countMe() const {
    return Starship::countMe();
}


// Metody DeathStar
DeathStar::DeathStar(StarThing::ShieldPoints shield, Attacking::AttackPower attackPower) : SingleImperialStarship(
        shield, attackPower) {}


// Metody ImperialDestroyer
ImperialDestroyer::ImperialDestroyer(StarThing::ShieldPoints shield, Attacking::AttackPower attackPower)
        : SingleImperialStarship(shield, attackPower) {}


// Metody TIEFighter
TIEFighter::TIEFighter(StarThing::ShieldPoints shield, Attacking::AttackPower attackPower) : SingleImperialStarship(
        shield, attackPower) {}


// Metody Squadron
Squadron::Squadron(std::vector<std::shared_ptr<ImperialStarship>> &ships) : ships(ships) {}

Squadron::Squadron(std::initializer_list<std::shared_ptr<ImperialStarship>> ships) : ships(ships) {}

void Squadron::takeDamage(Attacking::AttackPower damage) {
    for (auto &ship : ships)
        ship->takeDamage(damage);
}

StarThing::ShieldPoints Squadron::getShield() const {
    ShieldPoints shield = 0;
    for (const auto &ship : ships)
        shield += ship->getShield();
    return shield;
}

Attacking::AttackPower Squadron::getAttackPower() const {
    AttackPower attackPower = 0;
    for (const auto &ship : ships)
        if (ship->isAlive())
            attackPower += ship->getAttackPower();
    return attackPower;
}

bool Squadron::isAlive() const {
    for (const auto &ship : ships)
        if (ship->isAlive())
            return true;
    return false;
}

size_t Squadron::countMe() const {
    size_t counter = 0;
    for (const auto &ship : ships)
        counter += ship->countMe();
    return counter;
}


std::shared_ptr<DeathStar>
createDeathStar(DeathStar::ShieldPoints shield, DeathStar::AttackPower attack) {
    return std::make_shared<DeathStar>(shield, attack);
}

std::shared_ptr<ImperialDestroyer>
createImperialDestroyer(ImperialDestroyer::ShieldPoints shield, ImperialDestroyer::AttackPower attack) {
    return std::make_shared<ImperialDestroyer>(shield, attack);
}

std::shared_ptr<TIEFighter>
createTIEFighter(TIEFighter::ShieldPoints shield, TIEFighter::AttackPower attack) {
    return std::make_shared<TIEFighter>(shield, attack);
}

std::shared_ptr<Squadron>
createSquadron(std::vector<std::shared_ptr<ImperialStarship>> &v) {
    return std::make_shared<Squadron>(v);
}

std::shared_ptr<Squadron>
createSquadron(std::initializer_list<std::shared_ptr<ImperialStarship>> l) {
    return std::make_shared<Squadron>(l);
}
