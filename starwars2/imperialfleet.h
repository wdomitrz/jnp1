/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#ifndef IMPERIALFLEET_H
#define IMPERIALFLEET_H

#include <vector>
#include <initializer_list>
#include <memory>
#include "helper.h"

// Klasa wszystkich statków imperialnych - łącznie z Squadron
class ImperialStarship : public virtual Attacking, public virtual StarThing {
};

// Klasa pojedynczych statków imperialnych
class SingleImperialStarship : public Starship, public AttackingGivenPower, public ImperialStarship {
protected:
    SingleImperialStarship(ShieldPoints shield, AttackPower attackPower);

public:
    ShieldPoints getShield() const override;

    void takeDamage(AttackPower damage) override;

    AttackPower getAttackPower() const override;

    bool isAlive() const override;

    size_t countMe() const override;
};


class DeathStar : public SingleImperialStarship {
public:
    DeathStar(ShieldPoints shield, AttackPower attackPower);
};

class ImperialDestroyer : public SingleImperialStarship {
public:
    ImperialDestroyer(ShieldPoints shield, AttackPower attackPower);
};

class TIEFighter : public SingleImperialStarship {
public:
    TIEFighter(ShieldPoints shield, AttackPower attackPower);
};


class Squadron : public ImperialStarship {
    std::vector<std::shared_ptr<ImperialStarship>> ships;

public:
    explicit Squadron(std::vector<std::shared_ptr<ImperialStarship>> &ships);

    Squadron(std::initializer_list<std::shared_ptr<ImperialStarship>> ships);

    void takeDamage(AttackPower damage) override;

    ShieldPoints getShield() const override;

    AttackPower getAttackPower() const override;

    bool isAlive() const override;

    size_t countMe() const override;
};


std::shared_ptr<DeathStar>
createDeathStar(DeathStar::ShieldPoints shield, DeathStar::AttackPower attack);

std::shared_ptr<ImperialDestroyer>
createImperialDestroyer(ImperialDestroyer::ShieldPoints shield, ImperialDestroyer::AttackPower attack);

std::shared_ptr<TIEFighter>
createTIEFighter(TIEFighter::ShieldPoints shield, TIEFighter::AttackPower attack);

std::shared_ptr<Squadron>
createSquadron(std::vector<std::shared_ptr<ImperialStarship>> &v);

std::shared_ptr<Squadron>
createSquadron(std::initializer_list<std::shared_ptr<ImperialStarship>> l);

#endif //IMPERIALFLEET_H
