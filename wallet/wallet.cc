/*
 * Authors:
 *  Magdalena Augustyńska
 *  Witalis Domitrz
 */

#include "wallet.h"

#include <regex>

namespace {

	static const NumberOfUnits UNITS_PER_B_LOCAL = 100000000;

	static const int PRECISION = 8;

	using Clock = std::chrono::system_clock;

	using ValidationRegex = const std::regex;
	using PartOfRegexText = std::string;

	// Regex do walidacji czy napis jest poprawnym napisem binarnym.
	// Nie dopuszczamy napisów pustych i liczb niecałkowitych.
	// Dopuszczamy zera wiodące.
	// Maksymalna znacząca (bez zer wiodących) długość napisu to 25,
	// bo sufit(log_2(21000000)) = 25
	// Nie dopuszczamy białych znaków
	const ValidationRegex BINARY_NUMBER_REGEX(
	    // Początek napisu
	    (PartOfRegexText) "^" +
	    // Zera wiodące
	    (PartOfRegexText) "0*" +
	    // Zancząca część napisu
	    (PartOfRegexText) "[01]{1,25}" +
	    // Koniec napisu
	    (PartOfRegexText) "$");

	// Regex do walidacji czy napis jest poprawną liczbą z treści zadania.
	// Nie dopuszczamy napisów pustych.
	// Dopuszaczamy zera wiodące.
	// Nie dopuszczamy notacji: .23
	const ValidationRegex NUMBER_REGEX(
	    // Początek napisu
	    (PartOfRegexText) "^\\s*" +
	    // Zera wiodące
	    (PartOfRegexText) "0*" +
	    // Część odzpowiedzialna za część całkowitą liczby
	    // (długości maksymlnie 8, bo jest 21000000 monet (bez zer wiodących))
	    (PartOfRegexText) "\\d{1,8}" +
	    // Część odpowiedzialna za opcjonalną część po przecinku
	    // Dopiszczamy przecinkek na końcu: 42.
	    (PartOfRegexText) + "([,.]\\d{0,8})?" +
	    // Koniec napisu
	    (PartOfRegexText) "\\s*$");

	std::string unitsToString(const NumberOfUnits n) {

		std::stringstream ss;
		ss << std::fixed << std::setprecision(PRECISION) << (long double) n / (long double) UNITS_PER_B_LOCAL;
		std::string s =  ss.str();

		size_t indexOfDot;
		for (indexOfDot = 0 ; indexOfDot < s.size() ; indexOfDot++)
			if (s[indexOfDot] == '.')
				break;

		size_t lastWantedCharacterIndex = s.size() - 1;

		if (indexOfDot != s.size()) {

			s[indexOfDot] = ',';

			while (s[lastWantedCharacterIndex] == '0')
				lastWantedCharacterIndex--;

			if (lastWantedCharacterIndex == indexOfDot)
				lastWantedCharacterIndex--;
		}

		return s.substr(0, lastWantedCharacterIndex + 1);
	}

	NaturalNumber binaryToNumber(const std::string & s) {
		if (!std::regex_match(s, BINARY_NUMBER_REGEX))
			throw std::invalid_argument("Not a binary representation of integer!");

		return std::stoi(s, nullptr, 2);
	}

	long double stringToB(std::string & s) {

		if (!std::regex_match(s, NUMBER_REGEX))
			throw std::invalid_argument("Not a representation of number!");

		std::replace(s.begin(), s.end(), ',', '.');

		return std::stold(s);
	}
}

const Wallet &Empty() {
	static const Wallet emptyWallet = Wallet();
	return emptyWallet;
}

const NumberOfUnits Wallet::UNITS_PER_B = UNITS_PER_B_LOCAL;

const NumberOfUnits Wallet::MAX_NUMBER_OF_UNITS = 21 * 1000 * 1000 * UNITS_PER_B;

NumberOfUnits Wallet::unitsInUse = 0;

bool Wallet::unitsInRangeAndRegister(NumberOfUnits units) {
	if (units > MAX_NUMBER_OF_UNITS - unitsInUse)
		return false;
	unitsInUse += units;
	return true;
}

void Wallet::unitsUnregister(NumberOfUnits units) {
	unitsInUse -= units;
}

void Wallet::registerOperation() {
	operationsHistory.insert(Operation(unitsInThisWallet));
}

Wallet::Wallet(void) : Wallet(0) {}

Wallet::Wallet(NaturalNumber n) {

	if (n < 0)
		throw std::invalid_argument("Natural numbers are positive");

	unitsInThisWallet = (NumberOfUnits) n * UNITS_PER_B;

	if (!unitsInRangeAndRegister(unitsInThisWallet))
		throw std::invalid_argument("Too many BajtekCoins!");

	registerOperation();
}

Wallet::Wallet(std::string s) {

	auto n = (NumberOfUnits) (stringToB(s) * UNITS_PER_B);

	if (!unitsInRangeAndRegister(n))
		throw std::invalid_argument("Too many BajtekCoins!");

	unitsInThisWallet = n;
	registerOperation();
}

Wallet::Wallet(const char* chrs) : Wallet((std::string) chrs) {
	if (chrs == nullptr)
		throw std::invalid_argument("nullptr is not a valid word!");
}

Wallet::Wallet(Wallet &&w2) noexcept {
	unitsInThisWallet = w2.unitsInThisWallet;
	w2.unitsInThisWallet = 0;
	operationsHistory.swap(w2.operationsHistory);
	registerOperation();
}

Wallet::Wallet(Wallet &&w1, Wallet &&w2) {
	operationsHistory.insert(w1.operationsHistory.begin(), w1.operationsHistory.end());
	operationsHistory.insert(w2.operationsHistory.begin(), w2.operationsHistory.end());
	unitsInThisWallet = w1.unitsInThisWallet + w2.unitsInThisWallet;
	w1.unitsInThisWallet = 0;
	w2.unitsInThisWallet = 0;
	registerOperation();
	w1.registerOperation();
	w2.registerOperation();
}

Wallet Wallet::fromBinary(const std::string &s) {

	return Wallet(binaryToNumber(s));
}

Wallet &Wallet::operator=(Wallet &&w2) noexcept {
	if (this != &w2) {
		unitsUnregister(unitsInThisWallet);
		unitsInThisWallet = w2.unitsInThisWallet;
		w2.unitsInThisWallet = 0;
		operationsHistory.swap(w2.operationsHistory);
		registerOperation();
	}
	return *this;
}

Wallet operator+(Wallet &&w1, Wallet &w2) {
	Wallet w0;
	w0.unitsInThisWallet = w1.unitsInThisWallet + w2.unitsInThisWallet;
	w1.unitsInThisWallet = 0;
	w2.unitsInThisWallet = 0;
	w0.operationsHistory.clear();
	w0.operationsHistory.insert(w1.operationsHistory.begin(), w1.operationsHistory.end());
	w0.operationsHistory.insert(w2.operationsHistory.begin(), w2.operationsHistory.end());
	w0.registerOperation();
	w2.registerOperation();
	return w0;
}

Wallet operator+(Wallet &&w1, Wallet &&w2) {
	Wallet w0;
	w0.unitsInThisWallet = w1.unitsInThisWallet + w2.unitsInThisWallet;
	w1.unitsInThisWallet = 0;
	w2.unitsInThisWallet = 0;
	w0.operationsHistory.clear();
	w0.operationsHistory.insert(w1.operationsHistory.begin(), w1.operationsHistory.end());
	w0.operationsHistory.insert(w2.operationsHistory.begin(), w2.operationsHistory.end());
	w0.registerOperation();
	return w0;
}

Wallet operator-(Wallet &&w1, Wallet &w2) {
	if (w1.unitsInThisWallet < w2.unitsInThisWallet)
		throw std::invalid_argument("Wallet cannot have negative BajtekCoins!");
	Wallet w0;
	w0.unitsInThisWallet = w1.unitsInThisWallet - w2.unitsInThisWallet;
	w1.unitsInThisWallet = 0;
	w2.unitsInThisWallet *= 2;
	w0.operationsHistory.clear();
	w0.registerOperation();
	w2.registerOperation();
	return w0;
}

Wallet operator-(Wallet &&w1, Wallet &&w2) {
	if (w1.unitsInThisWallet < w2.unitsInThisWallet)
		throw std::invalid_argument("Wallet cannot have negative BajtekCoins!");
	Wallet w0;
	w0.unitsInThisWallet = w1.unitsInThisWallet -  w2.unitsInThisWallet;
	w1.unitsInThisWallet = 0;
	w2.unitsInThisWallet *= 2;
	w0.operationsHistory.clear();
	w0.registerOperation();
	return w0;
}

Wallet Wallet::operator*(NaturalNumber n) const {

	Wallet w0;

	if (n < 0)
		throw std::invalid_argument("Natural number are positive");

	if (!unitsInRangeAndRegister(n * unitsInThisWallet))
		throw std::invalid_argument("Too many BajtekCoins!");

	w0.operationsHistory.clear();
	w0.unitsInThisWallet = (NumberOfUnits) n * unitsInThisWallet;
	w0.registerOperation();

	return w0;
}

Wallet operator*(NaturalNumber n, const Wallet &w) {
	return w * n;
}

Wallet &Wallet::operator+=(Wallet &w) {
	unitsInThisWallet += w.unitsInThisWallet;
	w.unitsInThisWallet = 0;
	registerOperation();
	w.registerOperation();
	return *this;
}

Wallet &Wallet::operator+=(Wallet &&w) {
	unitsInThisWallet += w.unitsInThisWallet;
	w.unitsInThisWallet = 0;
	registerOperation();
	return *this;
}

Wallet &Wallet::operator-=(Wallet &w) {
	w.unitsInThisWallet += unitsInThisWallet;
	unitsInThisWallet = 0;
	registerOperation();
	w.registerOperation();
	return *this;
}

Wallet &Wallet::operator-=(Wallet &&w) {
	w.unitsInThisWallet += unitsInThisWallet;
	unitsInThisWallet = 0;
	registerOperation();
	return *this;
}

Wallet &Wallet::operator*=(const NaturalNumber n) {

	if (n < 0)
		throw std::invalid_argument("Natural number are positive");

	if (n != 0 && !unitsInRangeAndRegister((n - 1) * unitsInThisWallet))
		throw std::invalid_argument("Too many BajtekCoins!");

	if (n == 0)
		unitsUnregister(unitsInThisWallet);

	unitsInThisWallet *= n;
	registerOperation();
	return *this;
}

bool operator==(const Wallet &lhs, const Wallet &rhs) {
	return lhs.unitsInThisWallet == rhs.unitsInThisWallet;
}

bool operator!=(const Wallet &lhs, const Wallet &rhs) {
	return !(rhs == lhs);
}

bool operator<(const Wallet &lhs, const Wallet &rhs) {
	return lhs.unitsInThisWallet < rhs.unitsInThisWallet;
}

bool operator>(const Wallet &lhs, const Wallet &rhs) {
	return rhs < lhs;
}

bool operator<=(const Wallet &lhs, const Wallet &rhs) {
	return !(rhs < lhs);
}

bool operator>=(const Wallet &lhs, const Wallet &rhs) {
	return !(lhs < rhs);
}

std::ostream &operator<<(std::ostream &os, const Wallet &wallet) {
	os << "Wallet[" << unitsToString(wallet.unitsInThisWallet) << " B]";
	return os;
}

NumberOfUnits Wallet::getUnits() const {
	return unitsInThisWallet;
}

size_t Wallet::opSize() const {
	return operationsHistory.size();
}

const Operation Wallet::operator[](size_t k) const {
	if (k >= operationsHistory.size())
		throw std::invalid_argument("Index out of range");
	return *std::next(operationsHistory.begin(), k);
}

Wallet::~Wallet() {
	unitsUnregister(unitsInThisWallet);
}

Operation::Operation(const NumberOfUnits unitsAfterOperation) : timeOfOperation(Clock::now()),
	unitsAfterOperation(unitsAfterOperation) {}

NumberOfUnits Operation::getUnits() const {
	return unitsAfterOperation;
}

std::string Operation::timeToString(const TimeType time) {
	std::time_t timeHelper = Clock::to_time_t(time);
	auto timeFormated = localtime(&timeHelper);

	char buffer[11];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeFormated);
	return buffer;
}

bool Operation::operator==(const Operation &rhs) const {
	return timeOfOperation == rhs.timeOfOperation;
}

bool Operation::operator!=(const Operation &rhs) const {
	return !(rhs == *this);
}

bool Operation::operator<(const Operation &rhs) const {
	return timeOfOperation < rhs.timeOfOperation;
}

bool Operation::operator>(const Operation &rhs) const {
	return rhs < *this;
}

bool Operation::operator<=(const Operation &rhs) const {
	return !(rhs < *this);
}

bool Operation::operator>=(const Operation &rhs) const {
	return !(*this < rhs);
}

std::ostream &operator<<(std::ostream &os, const Operation &operation) {
	os << "Wallet balance is " << unitsToString(operation.unitsAfterOperation) << " B after operation made at day "
	   << Operation::timeToString(operation.timeOfOperation);
	return os;
}
