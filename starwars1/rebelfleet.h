/*
 * Authors:
 *  Jakub Krauz
 *  Witalis Domitrz
 */


#ifndef STARWARS_REBELFLEET_H
#define STARWARS_REBELFLEET_H

#include <type_traits>
#include <cassert>

template<typename U, bool canAttack, int minSpeed, int maxSpeed>
class RebelStarship {
    U shield;
    U speed;
    U attackPower;
public:

    // Klasy Explorer, StarCruiser i XWing mają publiczną składową valueType
    // reprezentującą typ U, którym zostały sparametryzowane.
    using valueType = U;

    template<bool _canAttack = canAttack, typename = std::enable_if_t <_canAttack>>
    constexpr RebelStarship(U shield, U speed, U attackPower) : shield(shield), speed(speed), attackPower(attackPower) {
        assert(minSpeed <= speed && speed <= maxSpeed);
    }

    template<bool _canAttack = canAttack, typename = std::enable_if_t<!_canAttack>>
    constexpr RebelStarship(U shield, U speed) : shield(shield), speed(speed), attackPower(0) {
        assert(minSpeed <= speed && speed <= maxSpeed);
    }

    // U getShield() – zwraca wytrzymałości tarczy,
    U getShield() const {
        return shield;
    }

    // U getSpeed() – zwraca prędkość,
    U getSpeed() const {
        return speed;
    }

    // U getAttackPower() – zwraca siłę ataku statku.
    template<typename T=U>
    typename std::enable_if<canAttack, T>::type getAttackPower() const {
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

// Klasy Explorer, StarCruiser i XWing
template<typename U>
using Explorer = RebelStarship<U, false, 299796, 2997960>;

template<typename U>
using StarCruiser = RebelStarship<U, true, 99999, 299795>;

template<typename U>
using XWing = RebelStarship<U, true, 299796, 2997960>;

// Sprawdzanie, czy statek może atakować (czy ma metodę getAttackPower())
template<typename T>
using canAttack = decltype(std::declval<T &>().getAttackPower());

#endif // STARWARS_REBELFLEET_H
