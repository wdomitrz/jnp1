/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#ifndef REBELFLEET_H
#define REBELFLEET_H

#include <cassert>
#include <memory>
#include "imperialfleet.h"
#include "helper.h"

class RebelStarship : public Starship {
public:
    using Speed = unsigned long long;

private:
    Speed speed;

protected:
    // W celu wymuszenia implementacji isSpeedCorrect
    virtual bool isSpeedCorrect(const Speed &speed) const = 0;

    RebelStarship(ShieldPoints shield, Speed speed);

public:
    virtual void reactToAttack(ImperialStarship &enemy);

    Speed getSpeed() const;
};

class Explorer : public RebelStarship {
protected:
    bool isSpeedCorrect(const Speed &speed) const override;

public:
    Explorer(ShieldPoints shield, Speed speed);
};


// Podklasa atakującego statku rebelii
class AttackingRebelStarship : public RebelStarship, public AttackingGivenPower {
protected:
    AttackingRebelStarship(ShieldPoints shield, Speed speed, AttackPower attackPower);

public:
    void reactToAttack(ImperialStarship &enemy) override;
};

class StarCruiser : public AttackingRebelStarship {
protected:
    bool isSpeedCorrect(const Speed &speed) const override;

public:
    StarCruiser(ShieldPoints shield, Speed speed, AttackPower attackPower);
};

class XWing : public AttackingRebelStarship {
protected:
    bool isSpeedCorrect(const Speed &speed) const override;

public:
    XWing(ShieldPoints shield, Speed speed, AttackPower attackPower);
};


std::shared_ptr<Explorer>
createExplorer(Explorer::ShieldPoints shield, Explorer::Speed speed);

std::shared_ptr<StarCruiser>
createStarCruiser(StarCruiser::ShieldPoints shield, StarCruiser::Speed speed, XWing::AttackPower attack);

std::shared_ptr<XWing>
createXWing(StarCruiser::ShieldPoints shield, StarCruiser::Speed speed, XWing::AttackPower attack);

#endif //REBELFLEET_H
