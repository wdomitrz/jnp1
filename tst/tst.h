//
// Created by witalis on 1/16/19.
//

#ifndef FUNKCYJNE_TST_H
#define FUNKCYJNE_TST_H

#include <string>
#include <memory>

namespace Detail {
    // Funkcja fold na zakresie wyznaczonym przez iteratory działa następująco:
    //   functor(...functor(functor(acc, *first), *(first + 1))..., *(last - 1))
    // W szczególnym przypadku first == last fold zwraca acc.
    template<typename Iter, typename Acc, typename Functor>
    inline Acc fold(Iter first, Iter last, Acc acc, Functor functor) {
        return first == last ? acc : fold(std::next(first), last, functor(acc, *first), functor);
    }
} // namespace Detail

template<typename C = char>
class TST {

    using TSTptr = std::shared_ptr<const TST>;

    // Potoków trzymam w tablicy, bo tak dostaję mniejszą powtarzalność kodu
    static const int SONS_NO = 3;
    const std::array<TSTptr, SONS_NO> sons;
    enum sonsIds {
        LEFT, CENTER, RIGHT
    };

    // C może być czym innym niż char (w ogólności, więc lepiej trzymac przez wskaźnik)
    // Dodatkowo daje to możliwość ustawienia nullptr - brak litery
    const std::shared_ptr<const C> letter;
    const bool isWord;

    // Zamienia TSTptr na TST, a nullptr na puste drzewo
    static TST getTST(const TSTptr &tree) noexcept {
        return tree == nullptr ? TST() : *tree;
    }

    TST(const std::array<TSTptr, SONS_NO> &sons, const std::shared_ptr<const C> &letter, const bool isWord) noexcept :
            sons(sons), letter(letter), isWord(isWord) {}

    TST(const TSTptr &leftPtr, const TSTptr &centerPtr, const TSTptr &rightPtr, const std::shared_ptr<const C> &letter,
        const bool isWord) noexcept :
            sons({leftPtr, centerPtr, rightPtr}), letter(letter), isWord(isWord) {}

    TST(const TSTptr &centerPtr, const std::shared_ptr<const C> &letter, const bool isWord) noexcept :
            TST(nullptr, centerPtr, nullptr, letter, isWord) {}

    TST getSon(const int which) const noexcept {
        return getTST(sons[which]);
    }

    TST showSon(const int which) const {
        // jeśli jesteśmy puści, to rzucamy wyjątek, w przeciwnym przypadku zwracamy syna
        return empty() ? throw std::logic_error("This node is empty") : getSon(which);
    }

    // Zawsze do tej metody przekazuję taki idx, że jest on indeksem jakiejś litery w słowie
    // str (to znaczy idx < strSize))
    // str.size() przekazuję jako dodaktowy argument, żeby obliczać to tylko raz
    TST add(const std::basic_string<C> &str, size_t idx, size_t strSize) const noexcept {
        // Zmienna pomocnicza do zwiększenia czytelności
        bool isLastLetter = strSize - 1 == idx;
        return empty() ?
               // Jeśli obsługujemy puste drzewo, to przerabiamy je na drzewo z poddrzewem z dodaną resztą słowa, lub ostatni element tego słowa
               TST(isLastLetter ? nullptr : std::make_shared<const TST>(TST().add(str, idx + 1, strSize)),
                   std::make_shared<const C>(str[idx]), isLastLetter) :
               // Jeśli obsługujemy niepuste poddrzewo, to
               str[idx] == *letter ?
               // Jeśli trafiliśmy z literą
               isLastLetter ?
               // Jeśli jest to ostatnia litera to zaznaczamy zakończenie słowa
               // (to słowo nie może mieć zaznaczonego istnienia, bo sprawdzamy to przed wywołaniem, więc nie ma potrzeby sprawdzania,
               // Czy tu już wcześniej kończyło się słowo - na pewno tak nie było
               TST(sons, letter, true) :
               // Jeśli to nie jest ostatnia litera, tworzymy nowy wierzchołek z rekurencyjnym potomkiem
               TST(sons[LEFT], std::make_shared<const TST>(getSon(CENTER).add(str, idx + 1, strSize)), sons[RIGHT], letter, isWord) :
               // Jeśli nie trafiliśmy z literą, to wywołujemy się na odpowiednim synu rekurencyjnie
               str[idx] < *letter ?
               // Odpowiednio lewym
               TST(std::make_shared<const TST>(getSon(LEFT).add(str, idx, strSize)), sons[CENTER], sons[RIGHT], letter, isWord) :
               // Lub prawym
               TST(sons[LEFT], sons[CENTER], std::make_shared<const TST>(getSon(RIGHT).add(str, idx, strSize)), letter, isWord);
    }

    // Zawsze do tej metody przekazuję taki idx, że jest on indeksem jakiejś litery w słowie
    // str (to znaczy idx < strSize)
    // str.size() przekazuję jako dodaktowy argument, żeby obliczać to tylko raz
    bool exist(const std::basic_string<C> &str, size_t idx, size_t strSize) const noexcept {
        // Zmienna pomocnicza do zwiększenia czytelności
        bool isLastLetter = strSize - 1 == idx;
        // Jeśli jesteśmy w pustym wierzchołku, to zwracamy fałsz
        return !empty() &&
               (str[idx] == *letter ?
                // Jeśli trafiliśmy z literą
                isLastLetter ?
                // I jest to ostatnia litera, to sprawdzamy, czy kończy się tu słowo
                isWord :
                // A jeśli nie (jest to ostatnia litera), to wykonujemy się rekurencyjnie na swoim środkowym synu
                getSon(CENTER).exist(str, idx + 1, strSize) :
                // Jeśli nie trafiliśmy z literą, to musimy pójść do odpowiednio lewego, lub prawego syna
                getSon(str[idx] < *letter ? LEFT : RIGHT).exist(str, idx, strSize));
    }

    // Szukamy długości prefiksu zaczynając od indeksu idx
    // str.size() przekazuję jako dodaktowy argument, żeby obliczać to tylko raz
    size_t lengthOfPrefix(const std::basic_string<C> &str, size_t idx, size_t strSize) const noexcept {
        return empty() || strSize == idx ?
               // Jeśli jesteśmy w pustym drzewie, albo nie mamy już słowa, to nie przedłużymy prefiksu bardziej
               idx :
               // Jeśli mamy niepuste poddrzewo i nie przeszliśmy całego słowa, to
               str[idx] == *letter ?
               // Jeśli trafiliśmy z literą, to wykonujemy się rekurencyjnie na środkowym synu z kolejnym indeksem
               getTST(sons[CENTER]).lengthOfPrefix(str, idx + 1, strSize) :
               // Jeśli nie trafiliśmy z literą, to musimy pójść do odpowiednio lewego, lub prawego syna
               getTST(sons[str[idx] < *letter ? LEFT : RIGHT]).lengthOfPrefix(str, idx, strSize);
    }

public:

    TST &operator=(const TST &) = delete;

    TST &operator=(TST &&) = delete;

    TST(const TST &) noexcept = default;

    TST(TST &&) noexcept = default;

    TST() noexcept : TST(nullptr, nullptr, nullptr, nullptr, false) {}

    TST(const std::basic_string<C> &str) : TST(TST() + str) {}

    TST(const C *str) : TST(std::basic_string<C>(str)) {
        if (str == nullptr)
            throw std::logic_error("nullptr is not w valid string");
    }

    TST operator+(const std::basic_string<C> &str) const {
        // Nic nie robię z dodawaniem pustego, lub istniejącego już w drzewie słowa
        return (str.size() == 0 || exist(str)) ? *this : add(str, 0, str.size());
    }

    TST operator+(const C *str) const {
        return str == nullptr ? throw std::logic_error("nullptr is not w valid string") : operator+(std::basic_string<C>(str));
    }

    const C value() const {
        return empty() ? throw std::logic_error("This node is empty") : *letter;
    }

    bool word() const {
        return empty() ? throw std::logic_error("This node is empty") : isWord;
    }

    TST left() const {
        return showSon(LEFT);
    }

    TST center() const {
        return showSon(CENTER);
    }

    TST right() const {
        return showSon(RIGHT);
    }

    bool empty() const noexcept {
        return letter == nullptr;
    }

    bool exist(const std::basic_string<C> &str) const noexcept {
        // Puste słowo nigdy nie istnieje w naszym drzewie (moodle)
        return str.size() != 0 && exist(str, 0, str.size());
    }

    // Wyszukuje najdłuższy wspólny prefiks słowa str i słów zawartych w danym
    // drzewie. Przykład: jeśli tst składa się ze słów "category", "functor"
    // oraz "theory" to tst.prefix("catamorphism") daje rezultat "cat".
    std::basic_string<C> prefix(const std::basic_string<C> &str) const noexcept {
        return str.substr(0, lengthOfPrefix(str, 0, str.size()));
    }

    template<typename Acc, typename Functor>
    Acc fold(Acc acc, Functor functor) const noexcept {
        return empty() ? acc : right().fold(center().fold(left().fold(functor(acc, value()), functor), functor), functor);
    }

    size_t size() const noexcept {
        return this->fold((size_t) 0, [](auto acc, auto) { return acc + 1; });
    }
};

#endif //FUNKCYJNE_TST_H
