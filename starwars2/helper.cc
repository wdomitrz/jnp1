/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#include "helper.h"


// Metody AttackingGivenPower
AttackingGivenPower::AttackingGivenPower(Attacking::AttackPower attackPower) : attackPower(attackPower) {}

Attacking::AttackPower AttackingGivenPower::getAttackPower() const { return attackPower; }


// Metody Starship
Starship::Starship(StarThing::ShieldPoints shield) : shield(shield) {}

StarThing::ShieldPoints Starship::getShield() const { return shield; }

void Starship::takeDamage(Attacking::AttackPower damage) {
    ShieldPoints damageReveived = damage;
    if (damageReveived > shield)
        shield = 0;
    else
        shield -= damageReveived;
}

bool Starship::isAlive() const {
    return getShield() != 0;
}

size_t Starship::countMe() const {
    // Pojedynczy statek żyje, lub nie
    return isAlive() ? 1 : 0;
}
