/*
 * Authors:
 *  Magdalena Augustyńska
 *  Witalis Domitrz
 */

#ifndef WALLET_WALLET_H
#define WALLET_WALLET_H

#include <string>
#include <set>
#include <ostream>
#include <chrono>

// W ramach tego zadania należy zaimplementować portfel (klasa Wallet) oraz
//    historię operacji na nim.
class Wallet;

class Operation;

using NaturalNumber = int;

using NumberOfUnits = unsigned long long;

// Należy zaimplementować również globalną funkcję Empty(), która zwróci obiekt
//    reprezentujący pusty portfel. Modyfikowanie zwróconego obiektu powinno być
//    zabronione. W szczególności konstrukcja "Empty() += Wallet(1);" powinna zostać
//    zgłoszone jako błąd kompilacji.
const Wallet &Empty();

class Wallet {

	using Operations = std::multiset<Operation>;

	static const NumberOfUnits UNITS_PER_B;

	static const NumberOfUnits MAX_NUMBER_OF_UNITS;

	static NumberOfUnits unitsInUse;

	static bool unitsInRangeAndRegister(NumberOfUnits units);

	static void unitsUnregister(NumberOfUnits);

	NumberOfUnits unitsInThisWallet;

	Operations operationsHistory;

	void registerOperation();

public:

	template<typename T> Wallet(T arg) = delete;

	// Tworzy pusty portfel. Historia portfela ma jeden wpis.
	Wallet();

	// Tworzy portfel z n B, gdzie n jest liczbą naturalną. Historia portfela ma
	//    jeden wpis.
	Wallet(NaturalNumber);

	// Tworzy portfel na podstawie napisu str określającego ilość B. Napis może
	//    zawierać część ułamkową (do 8 miejsc po przecinku). Część ułamkowa jest
	//    oddzielona przecinkiem lub kropką. Białe znaki na początku i końcu napisu
	//    powinny być ignorowane. Historia portfela ma jeden wpis.
	explicit Wallet(std::string);

	// Pozwala na uzucie char* jako argumentu konstruktora
	explicit Wallet(const char*);

	//  Konstruktor przenoszący. Historia operacji w1 to historia operacji w2
	//    i jeden nowy wpis.
	Wallet(Wallet &&w2) noexcept;

	// Tworzy portfel, którego historia operacji to suma historii operacji w1
	//    i w2 plus jeden wpis, całość uporządkowana wg czasów wpisów. Po operacji
	//    w w0 jest w1.getUnits() + w2.getUnits() jednostek, a portfele w1 i w2 są
	//    puste.
	Wallet(Wallet &&w1, Wallet &&w2);

	// Metoda klasowa tworząca portfel na podstawie napisu str, który jest zapisem
	//    ilości B w systemie binarnym. Kolejność bajtów jest grubokońcówkowa
	//    (ang. big endian).
	static Wallet fromBinary(const std::string &);

	//  Przypisanie. Jeżeli oba obiekty są tym samym obiektem, to nic nie robi, wpp.
	//    historia operacji w1 to historia operacji w2 i jeden nowy wpis. Dostępne
	//    jest tylko przypisanie przenoszące, nie przypisanie kopiujące, np.
	Wallet &operator=(Wallet &&) noexcept;

	// Dodawanie
	friend Wallet operator+(Wallet &&, Wallet &);

	friend Wallet operator+(Wallet &&, Wallet &&);

	// Odejmowanie, analogicznie jak dodawanie, ale po odejmowaniu w w2 jest dwa
	//    razy więcej jednostek, niż było w w2 przed odejmowaniem.
	//    Historia operacji powstałego obiektu zależy od implementacji.
	friend Wallet operator-(Wallet &&, Wallet &);

	friend Wallet operator-(Wallet &&, Wallet &&);

	// Pomnożenie przez liczbę naturalną.
	//    Historia operacji powstałego obiektu zależy od implementacji.
	Wallet operator*(NaturalNumber) const;

	friend Wallet operator*(NaturalNumber, const Wallet &);

	// Po operacji w2 ma 0 B i dodatkowy wpis w historii, a w1 ma
	//    w1.getUnits() + w2.getUnits() jednostek i jeden dodatkowy wpis w historii.
	Wallet &operator+=(Wallet &);

	Wallet &operator+=(Wallet &&);

	// Analogicznie do dodawania.
	Wallet &operator-=(Wallet &);

	Wallet &operator-=(Wallet &&);

	// Pomnożenie zawartości portfela przez liczbę naturalną.
	//    Dodaje jeden wpis w historii w1.
	Wallet &operator*=(NaturalNumber);

	// Operatory porównujące wartości portfeli w1 i w2, gdzie op to jeden z:
	//    ==, <, <=, != , >, >=.
	friend bool operator==(const Wallet &lhs, const Wallet &rhs);

	friend bool operator!=(const Wallet &lhs, const Wallet &rhs);

	friend bool operator<(const Wallet &lhs, const Wallet &rhs);

	friend bool operator>(const Wallet &lhs, const Wallet &rhs);

	friend bool operator<=(const Wallet &lhs, const Wallet &rhs);

	friend bool operator>=(const Wallet &lhs, const Wallet &rhs);

	// Wypisuje "Wallet[b B]" na strumień os, gdzie b to zawartość portfela w B.
	//    Wypisywana liczba jest bez białych znaków, bez zer wiodących oraz zer na
	//    końcu w rozwinięciu dziesiętnym oraz z przecinkiem jako separatorem
	//    dziesiętnym.
	friend std::ostream &operator<<(std::ostream &os, const Wallet &wallet);

	// Zwraca liczbę jednostek w portfelu.
	NumberOfUnits getUnits() const;

	// Zwraca liczbę operacji wykonanych na portfelu.
	size_t opSize() const;

	// Zwraca k-tą operację na portfelu. Pod indeksem 0 powinna być najstarsza
	//    operacja. Przypisanie do w[k] powinno być zabronione na etapie kompilacji.
	const Operation operator[](size_t) const;

	~Wallet();
};

class Operation {

	using TimeType = std::chrono::time_point<std::chrono::system_clock>;

	static std::string timeToString(const TimeType time);

	const TimeType timeOfOperation;

	const NumberOfUnits unitsAfterOperation;

public:

	// Stworzenie operacji teraz z dana liczba jednostek - explicit aby nie pozwolić nieświadomie rzutować NumberOfUnits
	// na operation co mogłoby prowadzić do porównania operacji i liczby
	explicit Operation(NumberOfUnits unitsAfterOperation);

	// Zwraca liczbę jednostek w portfelu po operacji.
	NumberOfUnits getUnits() const;

	// Operatory porównujące czas utworzenia (z dokładnością do milisekund)
	//    operacji o1 i o2, gdzie op to jeden z: ==, <, <=, != , >, >=.
	bool operator==(const Operation &rhs) const;

	bool operator!=(const Operation &rhs) const;

	bool operator<(const Operation &rhs) const;

	bool operator>(const Operation &rhs) const;

	bool operator<=(const Operation &rhs) const;

	bool operator>=(const Operation &rhs) const;

	// Wypisuje na strumień os "Wallet balance is b B after operation made at day d".
	//    Liczba b jak przy wypisywaniu portfela. Czas d w formacie yyyy-mm-dd.
	friend std::ostream &operator<<(std::ostream &os, const Operation &operation);
};

#endif //WALLET_WALLET_H
