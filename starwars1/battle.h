/*
 * Authors:
 *  Jakub Krauz
 *  Witalis Domitrz
 */


#ifndef STARWARS_BATTLE_H
#define STARWARS_BATTLE_H

#include "imperialfleet.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <array>
#include <tuple>

template<class T>
using canAttack = decltype(std::declval<T &>().getAttackPower());


template<typename T, T t0, T t1, typename... S>
class SpaceBattle {

    // Zmienne do zapisania informacji o zakończonej bitwie
    bool hasFinishedFlag = false;
    std::string resultOfTheBattle;

    // Sprawdzenie, czy bitwa już się skończyła
    bool hasFinished() {

        // Sprawdzenie czy już kiedyś bitwa się skończyła. Jeśli tak to nie ma
        // sensu nic robić oprócz wypisywania wiadomości.
        if (hasFinishedFlag)
            return true;

        size_t imperialFleetCounter = countImperialFleet();
        size_t rebelFleetCounter = countRebelFleet();

        if (imperialFleetCounter == 0 || rebelFleetCounter == 0) {

            hasFinishedFlag = true;

            if (imperialFleetCounter != 0)
                resultOfTheBattle = "IMPERIUM WON\n";
            else if (rebelFleetCounter != 0)
                resultOfTheBattle = "REBELLION WON\n";
            else
                resultOfTheBattle = "DRAW\n";

            return true;
        }

        return false;
    }

    T currentTime;

    std::tuple<S...> allStarships;

    // Wyliczenie liczby wszystkich czasów ataków
    static constexpr size_t calculateNumberOfAttacks() {
        size_t out = 0;
        for (T t = 0; t * t <= t1; t++) {
            out++;
        }
        return out;
    }

    // Czasy, w których należy zaatakować
    std::array<T, calculateNumberOfAttacks()> attackTimes;

    // Sprawdzenie, czy teraz jest czas ataku
    bool isNowAttackTime() {
        return std::find(attackTimes.begin(), attackTimes.end(), currentTime) != attackTimes.end();
    }

    // Funkcja sprawiająca, że podany statek atakuje dany statek rebelii, a jeśli jest to statek imperium, to nic się
    // nie dzieje
    template<typename Z, typename V>
    void attackedByImperialShip(Z &imperialStarship, V &rebelStarship) {

        // Sprawdzenie, czy statek jest statkiem rebelii
        if constexpr (isImperial<Z>::value && !isImperial<V>::value) {
            // jeśli oba statki nie nie zostały jeszcze zniszczone,
            if (imperialStarship.getShield() != 0 && rebelStarship.getShield() != 0)
                // statek Imperium atakuje statek Rebelii.
                attack(imperialStarship, rebelStarship);
        }
    }

    // Funkcja sprawiająca, że statek imperium atakuje wszystkie możliwe statki, a statek rebelii nic nie robi
    template<typename Z>
    void attackByImperialShip(Z &imperialStarship) {

        // Jeśli to nie jest statek imperium, to nic się nie dzieje
        if constexpr (isImperial<Z>::value) {

            // dla każdego statku Rebelii
            std::apply([&](auto &...starship) {
                (..., attackedByImperialShip(imperialStarship, starship));
            }, allStarships);
        }
    }

    // Atak statków imperium na statki rebelii
    void imperalAttack() {

        // dla każdego statku Imperium
        std::apply([&](auto &...starship) {
            (..., attackByImperialShip(starship));
        }, allStarships);
    }

    // Przesunięcie aktualnego czasu o timeStep
    void moveTime(T timeStep) {

        currentTime += timeStep;

        currentTime %= (t1 + 1);
    }

    template<typename I>
    bool isImperialShip(const I &) {
        return isImperial<I>::value;
    }

    // Sprawdzenie, czy dany statek jest rebeliancki (nie jest imperialny)
    // Bo "Kto nie jest z nami jest przeciwko nam!" ;)
    template<typename R>
    bool isRebelShip(const R &starshilp) {
        // "Kto nie jest z nami jest przeciwko nam"
        return !isImperialShip(starshilp);
    }

    // Wygenerowanie wszystkich czasów ataków
    constexpr void generateAttackTimes() {
        for (size_t i = 0; i < attackTimes.size(); i++) {
            attackTimes[i] = i * i;
        }
    }

    // Funkcja do zliczania statków imperium
    template<typename Z>
    void countImperials(const Z &starship, size_t &counter) {
        if (isImperialShip(starship) && starship.getShield() > 0)
            counter++;
    }

    // Funkcja do zliczania statków rebelii
    template<typename Z>
    void countRebels(const Z &starship, size_t &counter) {
        if (isRebelShip(starship) && starship.getShield() > 0)
            counter++;
    }

public:

    explicit SpaceBattle(S... fleet) : currentTime(t0), allStarships(std::tuple<S...>(fleet...)) {
        static_assert(t0 >= 0, "t0 should not be negative");
        static_assert(t0 < t1, "t0 is too late");
        generateAttackTimes();
    }

    // size_t countImperialFleet() – zwraca liczbę niezniszczonych statków Imperium;
    size_t countImperialFleet() {

        size_t counter = 0;

        std::apply([&](const auto &...starship) {
            (..., countImperials(starship, counter));
        }, allStarships);

        return counter;
    }

    // size_t countRebelFleet() - zwraca liczbę niezniszczonych statków Rebelii;
    size_t countRebelFleet() {

        size_t counter = 0;

        std::apply([&](const auto &...starship) {
            (..., countRebels(starship, counter));
        }, allStarships);

        return counter;
    }

    // void tick(T timeStep) – na początku sprawdza aktualny czas; jeśli jest to
    // czas ataku, to następuje atak statków Imperium na statki Rebelii; na koniec czas
    // przesuwa się o timeStep.
    void tick(T timeStep) {

        if (hasFinished()) {
            std::cout << resultOfTheBattle;
            return;
        }

        if (isNowAttackTime())
            imperalAttack();

        moveTime(timeStep);
    }
};

#endif //STARWARS_BATTLE_H
