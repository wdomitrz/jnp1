/*
 * Authors:
 *  Wojciech Kuźmiński
 *  Witalis Domitrz
 */

#ifndef HELPER_H
#define HELPER_H

#include <cstddef> // Z size_t

// Nadklasa rzeczy atakujących
class Attacking {
public:
    using AttackPower = unsigned long long;
protected:
    virtual ~Attacking() = default;

public:
    virtual AttackPower getAttackPower() const = 0;
};


// Nadklasa obiektu z możliwością ataku z daną siłą
class AttackingGivenPower : public virtual Attacking {
    AttackPower attackPower;

protected:
    explicit AttackingGivenPower(AttackPower attackPower);

public:
    AttackPower getAttackPower() const override;
};


// Nadklasa dla klas mających wyrzymałość, mogących być zniszczonymi (lub nie),
// przyjmujących obrażenia i zgłaszających się do liczenia
class StarThing {
public:
    using ShieldPoints = unsigned long long;
protected:
    virtual ~StarThing() = default;

public:
    virtual ShieldPoints getShield() const = 0;

    virtual void takeDamage(Attacking::AttackPower damage) = 0;

    virtual bool isAlive() const = 0;

    // Sposób w jaki zgłaszam się do liczenia (jako ile obiektów)
    virtual size_t countMe() const = 0;
};


class Starship : public virtual StarThing {
    ShieldPoints shield;

protected:
    explicit Starship(ShieldPoints shield);

public:
    ShieldPoints getShield() const override;

    void takeDamage(Attacking::AttackPower damage) override;

    // Statek żyje, gdy ma niezerową tarczę
    bool isAlive() const override;

    size_t countMe() const override;
};

#endif //HELPER_H