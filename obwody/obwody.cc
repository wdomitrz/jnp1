/** @file
 * Implementacja rozwiązania zadania Obwody z IPP
 *
 * @author Dawid Biadun <db394217@students.mimuw.edu.pl>
 * @author Witalis Domitrz <witekdomitrz@gmail.com>
 * @date 06.10.2018
 */

#include <iostream>
#include <sstream>
#include <regex>
#include <list>
#include <unordered_map>
#include <set>

namespace {

    using ErrorInformation = const std::string;
    using Line = std::string;
    using NodesNumber = size_t;
    using LineNumber = size_t;
    using InputValidationRegex = const std::regex;
    using Kind = char;
    using SignReader = std::stringstream;
    using PartOfRegexText = std::string;
    using ElementSign = std::string;
    using ElementType = std::string;
    using ElementNumber = int;
    using Node = int;
    using LineReader = std::stringstream;
    using KindsList = std::list<Kind>;
    using ElementsSigns = std::set<ElementSign>;
    using ElementsNumbers = std::set<ElementNumber>;
    using Nodes = std::set<Node>;
    using TypeAndNumbers = std::pair<ElementType, ElementsNumbers>;
    using TypesToNumbers = std::unordered_map<ElementType, ElementsNumbers>;
    using ElementsInformations = std::unordered_map<Kind, TypesToNumbers>;

    ErrorInformation ERROR_IN_LINE = "Error in line ";

    // Wiersz z informacją o niepodłaczonym/niepodłączonych węźle/węzłach
    ErrorInformation ERROR_UNCONNECTED_NODES = "Warning, unconnected node(s): ";

    InputValidationRegex LINE_REGEX_PATTERN(

            (PartOfRegexText) "^\\s*(" +


            (PartOfRegexText) "((D|R|C|E)" +

            (PartOfRegexText) "(0|([1-9]\\d{0,8}))" +
            // Odstęp - biały znak
            (PartOfRegexText) "\\s+" +

            (PartOfRegexText) "([A-Z\\d][a-zA-Z\\d-,/]*)" +

            (PartOfRegexText) "((\\s+(0|([1-9]\\d{0,8}))){2}))|" +


            (PartOfRegexText) "((T)" +

            (PartOfRegexText) "(0|([1-9]\\d{0,8}))" +
            // Odstęp - biały znak
            (PartOfRegexText) "\\s+" +

            (PartOfRegexText) "([A-Z\\d][a-zA-Z\\d-,/]*)" +

            (PartOfRegexText) "((\\s+(0|([1-9]\\d{0,8}))){3}))" +

            (PartOfRegexText) ")\\s*$");

    KindsList kinds = {'T', 'D', 'R', 'C', 'E'};

    /* Struktura zawierająca informacje o wszystkich dodanych elementach i typach.
     * Opis struktury:
     * mapa:
     *  klucz (char): rodzaj elementu
     *  watość (mapa):
     *      klucz (std::string): typ elementu
     *      wartość (set):
     *          wartość (int): numer elementu
     */
    ElementsInformations allElements;

    ElementsSigns allElementsSigns;

    Nodes allNodes;

    Nodes unconnectedNodes;

    /**
     * @brief Sprawdza poprawność wiersza.
     * Sprawdza, czy wiersz spełnia wszystkie założenia opisane w zadaniu oprócz
     * unikalności oznaczeń elementów.
     * @param line  – linia do strawdzenia poprawności.
     * @return Wartość @c true, jeśli wiesz jest poprawny, w przeciwnym przypadku
     *         wartość @c false.
     */
    bool validateLine(const Line &line);

    /**
     * @brief Wypiosuje informację o błędzie.
     * Wypisuje informację o błędzie w wieszu wraz z jego numerem i treścią.
     * @param lineNo    – numer linii;
     * @param line      – treść wiersza.
     */
    void errorInLine(LineNumber lineNo, const Line &line);

    /**
     * @brief Parsuje linię.
     * Sprawdza, czy informacje zawarte w wierszu można uznać za poprawne, a
     * następnie parsuje wiesz zapisując zawarte w nim informacje.
     * @param line  – treść wiersza.
     * @return Wartość @c true, jeśli udało się poprawnie sparsować wiersz, w
     *         przeciwnym przypadku wartość @c false.
     */
    bool parseLine(const Line &line);

    /**
     * Wczytuje i przetwarza wejście.
     */
    void readElementsFromLines();

    template<typename KeyType, typename ValueType>
    /**
     * @brief Zamienia mapę na zbiór
     * Zamienia mapę na zbiór par (wartość, klucz) zawartych wcześniej w mapie.
     * @param mapToChange   – mapa do zamienienia na zbiór.
     * @return Nowopowstały zbiór z elementami tymisamymi, co w mapie.
     */
    std::set <std::pair<ValueType, KeyType>>
    mapToSet(std::unordered_map <KeyType, ValueType> &mapToChange);

    /**
     * Przetwarza informacje i wypisuje je na wyjście
     */
    void writeCorrectElementsInformations();

    /**
     * @brief Wypisuje niepodłączone węzły.
     * Sprawdza istnienie niepodłączonych węzłów i jeśli takie istniją, to wypisuje
     * je w formacie z treści zadania.
     */
    void listUnconnectedNodes();

}

/**
 * Główna funkcja programu.
 */
int main() {

    // Wczytanie i sparsowanie każdej z linii; wypisanie błędów
    readElementsFromLines();

    writeCorrectElementsInformations();

    listUnconnectedNodes();

    // Zakończenie programu z kodem wyjścia 0 - brak błędów
    return 0;
}

namespace {

    bool validateLine(const Line &line) {
        return std::regex_match(line, LINE_REGEX_PATTERN);
    }

    void errorInLine(LineNumber lineNo, const Line &line) {

        std::cerr << ERROR_IN_LINE << lineNo << ": " << line << "\n";
    }

    namespace parseLineHelpers {

        NodesNumber nodesCount(Kind kind) {

            // Sprawdzenie, czy element to Tranzystor - jedyny z trzema węzłami
            if (kind == 'T')
                return 3;
            return 2;
        }

        bool elementSignUnique(const ElementSign &sign) {
            return allElementsSigns.find(sign) == allElementsSigns.end();
        }

        namespace nodeParsingHelpers {

            void readNodes(LineReader &reader, Nodes &nodes, NodesNumber nodesNo) {
                for (NodesNumber i = 0; i < nodesNo; i++) {

                    Node node;

                    reader >> node;

                    nodes.insert(node);
                }
            }

            bool elementConnectedToMultipleNodes(Nodes &nodes) {
                return nodes.size() > 1;
            }

            void updateNode(Node node) {

                if (allNodes.find(node) == allNodes.end()) {
                    allNodes.insert(node);
                    unconnectedNodes.insert(node);
                } else
                    /* A jeśli już wystąpił, to usuwamy go z listy niepodłączonych
                     * (jak już go tam nie ma to erase nic nie robi)
                     */
                    unconnectedNodes.erase(node);
            }

            void updateNodesNumbers(const Nodes &nodes) {

                for (Node node : nodes) {
                    updateNode(node);
                }
            }

        }

        bool nodesParsing(LineReader &lineStream, NodesNumber nodesNo) {

            Nodes nodes;

            nodeParsingHelpers::readNodes(lineStream, nodes, nodesNo);

            if (!nodeParsingHelpers::elementConnectedToMultipleNodes(nodes))
                return false;

            nodeParsingHelpers::updateNodesNumbers(nodes);

            return true;
        }


        void insertElement(const ElementSign &sign, const ElementType &type) {

            allElementsSigns.insert(sign);

            SignReader signStream(sign);
            Kind kind;
            ElementNumber number;
            signStream >> kind >> number;

            allElements[kind][type].insert(number);
        }

    }

    bool parseLine(const Line &line) {

        LineReader lineStream(line);

        ElementSign sign;

        ElementType type;

        lineStream >> sign >> type;

        if (!parseLineHelpers::elementSignUnique(sign))
            return false;

        if (!parseLineHelpers::nodesParsing(lineStream,
                                            parseLineHelpers::nodesCount(sign[0])))
            return false;

        parseLineHelpers::insertElement(sign, type);

        return true;
    }

    void readElementsFromLines() {

        // Numer linii (od 1, zwięszkenie następuje na początku)
        LineNumber lineNo = 0;

        Line line;

        while (std::getline(std::cin, line)) {

            lineNo++;

            if (line.empty())
                continue;

            /* Sprawdzenie, czy wiersz jest zgodny z opisem z tresci zadania i
             * i czy jest poprawnym opisem nowego elementu, a jeśli tak, to
             * informacja o nim zostaje zapisana
             */
            if (!validateLine(line) || !parseLine(line))

                errorInLine(lineNo, line);
        }
    }

    template<typename KeyType, typename ValueType>
    std::set <std::pair<ValueType, KeyType>>
    mapToSet(std::unordered_map <KeyType, ValueType> &mapToChange) {

        std::set <std::pair<ValueType, KeyType>> outSet;

        for (auto const &pair : mapToChange) {
            outSet.insert(std::make_pair(pair.second, pair.first));
        }
        return outSet;
    }

    namespace writingHelpers {

        template<typename PrefixType, typename ElemType>

        void writeSetWithColonAndFixedPrefix(std::ostream &outStream, PrefixType prefix, std::set <ElemType> elements) {

            bool isFirstElement = true;

            for (auto const &element : elements) {

                if (!isFirstElement)
                    outStream << ", ";
                else
                    isFirstElement = false;

                outStream << prefix << element;
            }
        }

        void writeType(const ElementsNumbers &elements, const ElementType &type, const Kind kind) {

            writeSetWithColonAndFixedPrefix(std::cout, kind, elements);

            std::cout << ": " << type << "\n";
        }

        void writeKind(const Kind kind) {

            for (auto const &[elements, type] : mapToSet(allElements[kind])) {
                writeType(elements, type, kind);
            }
        }

    }

    void writeCorrectElementsInformations() {

        for (auto const &kind : kinds) {

            writingHelpers::writeKind(kind);
        }
    }

    void listUnconnectedNodes() {

        // Sprawdzenie węzła zerowego - masy
        if (allNodes.find(0) == allNodes.end())
            unconnectedNodes.insert(0);

        if (!unconnectedNodes.empty()) {

            std::cerr << ERROR_UNCONNECTED_NODES;

            writingHelpers::writeSetWithColonAndFixedPrefix(std::cerr, "", unconnectedNodes);

            std::cerr << "\n";
        }
    }

}