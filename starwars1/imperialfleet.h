/*
 * Authors:
 *  Jakub Krauz
 *  Witalis Domitrz
 */


#ifndef STARWARS_IMPERIALFLEET_H
#define STARWARS_IMPERIALFLEET_H

#include "rebelfleet.h"
#include <experimental/type_traits>

template<typename U>
class ImperialStarship {
    U shield;
    U attackPower;
public:

    // Klasy DeathStar, ImperialDestroyer i TIEFighter mają publiczną składową
    // valueType reprezentującą typ U, którym zostały sparametryzowane.
    using valueType = U;

    constexpr ImperialStarship(U shield, U attackPower) : shield(shield), attackPower(attackPower) {}

    // U getShield() – zwraca wytrzymałości tarczy,
    U getShield() const {
        return shield;
    }

    // U getAttackPower() – zwraca siłę ataku statku.
    U getAttackPower() const {
        return attackPower;
    }

    // void takeDamage(U damage) – zmniejsza wytrzymałości tarczy o damage, ale nie
    // więcej niż statek ma aktualnie.
    void takeDamage(U damage) {
        if (shield <= damage)
            shield = 0;
        else
            shield -= damage;
    }
};

// Klasy DeathStar, ImperialDestroyer i TIEFighter
template<typename U>
using DeathStar = ImperialStarship<U>;

template<typename U>
using ImperialDestroyer = ImperialStarship<U>;

template<typename U>
using TIEFighter = ImperialStarship<U>;

// Sprawdzanie, czy statek jest imperialny
template<typename I>
using isImperial = std::is_same<I, ImperialStarship<typename I::valueType>>;

// Funkcja dostępna tylko dla statku imperium i rebelii (statku, który nie jest statkiem imperium)
// Bo "Kto nie jest z nami jest przeciwko nam!" ;)
template<typename I, typename R, typename = std::enable_if_t <isImperial<I>::value>, typename = std::enable_if_t<!isImperial<R>::value>>
void attack(I &imperialShip, R &rebelShip) {

    if(imperialShip.getShield() == 0 || rebelShip.getShield() == 0)
        return;

    rebelShip.takeDamage(imperialShip.getAttackPower());

    if constexpr (std::experimental::is_detected_v < canAttack, R >) {

        imperialShip.takeDamage(rebelShip.getAttackPower());
    }
}

#endif // STARWARS_IMPERIALFLEET_H
