/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#ifndef BATTLE_H
#define BATTLE_H

#include <memory>
#include "rebelfleet.h"
#include "imperialfleet.h"


class SpaceBattleI {
protected:
    virtual ~SpaceBattleI() = default;

    class SpaceTimeI {
    public:
        using Time = unsigned long long;

    protected:
        virtual ~SpaceTimeI() = default;

    public:
        // virtual w celu dodania możliwości rozszerzania czasów międzygwiezdnych
        // (na naprzykład czasy systemów)
        virtual bool isNowAttackTime() const = 0;

        virtual void moveTime(Time timeStep) = 0;
    };

public:
    class BuilderI {
    protected:
        virtual ~BuilderI() = default;

    public:
        virtual BuilderI &startTime(SpaceTimeI::Time t) = 0;

        virtual BuilderI &maxTime(SpaceTimeI::Time t) = 0;

        virtual BuilderI &ship(const std::shared_ptr<RebelStarship> &starship) = 0;

        virtual BuilderI &ship(const std::shared_ptr<ImperialStarship> &starship) = 0;
    };

    virtual size_t countImperialFleet() const = 0;

    virtual size_t countRebelFleet() const = 0;

    virtual void tick(SpaceTimeI::Time timeStep) = 0;
};


class SpaceBattle : public SpaceBattleI {
protected:
    class SpaceTime : public SpaceTimeI {
    protected:
        Time currentTime;
        Time t1;

    public:
        SpaceTime(Time t0, Time t1);

        bool isNowAttackTime() const override;

        void moveTime(Time timeStep) override;
    };

private:
    SpaceTime spaceTime;

    std::vector<std::shared_ptr<ImperialStarship>> imperials;
    std::vector<std::shared_ptr<RebelStarship>> rebels;

    // Zmienne do zapisania informacji o zakończonej bitwie
    bool hasFinishedFlag = false;
    std::string resultOfTheBattle;

    SpaceBattle(SpaceTime::Time t0, SpaceTime::Time t1, std::vector<std::shared_ptr<ImperialStarship>> &imperials,
                std::vector<std::shared_ptr<RebelStarship>> &rebels);

    template<typename Ship>
    size_t countShips(const std::vector<std::shared_ptr<Ship>> &ships) const {
        size_t counter = 0;
        for (const auto &ship : ships)
            counter += ship->countMe();
        return counter;
    }

    // Sprawdzenie, czy bitwa już się skończyła
    bool hasFinished();

    void attack();

public:
    class Builder : public BuilderI {
        bool ist0Set = false;
        bool ist1Set = false;
        SpaceTime::Time t0;
        SpaceTime::Time t1;
        std::vector<std::shared_ptr<ImperialStarship>> imperials;
        std::vector<std::shared_ptr<RebelStarship>> rebels;

    public:
        Builder &startTime(SpaceTime::Time t) override;

        Builder &maxTime(SpaceTime::Time t) override;

        Builder &ship(const std::shared_ptr<RebelStarship> &starship) override;

        Builder &ship(const std::shared_ptr<ImperialStarship> &starship) override;

        virtual SpaceBattle build();
    };

    size_t countImperialFleet() const override;

    size_t countRebelFleet() const override;

    void tick(SpaceTime::Time timeStep) override;
};

#endif //BATTLE_H
