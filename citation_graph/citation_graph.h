/*
 * Authors:
 *  Tomasz Grześkiewicz
 *  Witalis Domitrz
 */

#ifndef JNP5_CITATION_GRAPH_H
#define JNP5_CITATION_GRAPH_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <algorithm>

class PublicationAlreadyCreated : public std::exception {
    virtual const char *what() const throw() {
        return "PublicationAlreadyCreated";
    }
};

class PublicationNotFound : public std::exception {
    virtual const char *what() const throw() {
        return "PublicationNotFound";
    }
};

class TriedToRemoveRoot : public std::exception {
    virtual const char *what() const throw() {
        return "TriedToRemoveRoot";
    }
};

template<class Publication>
class CitationGraph {

    // Klasa wierzchołka grafu
    class Node {
    public:
        Publication publication;
        std::vector<std::weak_ptr<Node>> parents;
        std::set<std::shared_ptr<Node>> children;

        Node(typename Publication::id_type const &id, std::vector<std::weak_ptr<Node>> parents) :
                publication(Publication(id)), parents(parents) {}
    };

    std::map<typename Publication::id_type const, std::weak_ptr<Node> > allPublications; // Może typename Publication::id_type const& (z &)

    std::shared_ptr<Node> root;

public:

    // Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(typename Publication::id_type const &id) const {

        auto possiblePtr = allPublications.find(id);
        return possiblePtr != allPublications.end() && !possiblePtr->second.expired();
    }

private:

    // Zwraca wskaźnik do obiektu reprezentującego wierzchłek z publikacją o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
    // nie istnieje.
    std::shared_ptr<Node> getNode(typename Publication::id_type const &id) const {

        if (!exists(id))
            throw PublicationNotFound();

        auto found = allPublications.find(id);
        return found->second.lock();
    }

    // Porównywanie po pierwszym elemencie do użycia w remove
    struct pairFirstElemCompare {
        bool operator()(const std::pair<std::shared_ptr<Node>, typename std::set<std::shared_ptr<Node>>::iterator> &lhs,
                        const std::pair<std::shared_ptr<Node>, typename std::set<std::shared_ptr<Node>>::iterator> &rhs) const noexcept {
            return lhs.first < rhs.first;
        }
    };

public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(typename Publication::id_type const &stem_id) {

        auto root = std::make_shared<Node>(stem_id, std::vector<std::weak_ptr<Node>>({}));
        allPublications[stem_id] = std::weak_ptr<Node>(root);
    }

    // Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
    // noexcept.
    CitationGraph(CitationGraph<Publication> &&other) noexcept {

        std::swap(root, other.root);
        std::swap(allPublications, other.allPublications);
    }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept {

        if (this != &other) {
            std::swap(root, other.root);
            std::swap(allPublications, other.allPublications);
        }
        return *this;
    }

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
    // wtedy, gdy metoda Publication::get_id jest noexcept.
    typename Publication::id_type get_root_id() const noexcept(noexcept(std::declval<Publication &>().get_id())) {

        return root->publication.get_id();
    }

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const {

        auto publicationNodePtr = getNode(id);

        std::vector<typename Publication::id_type> children_ids;

        for (auto &child : publicationNodePtr->children)
            children_ids.emplace_back(child->publication.get_id());

        return children_ids;
    }

    // Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<typename Publication::id_type> get_parents(typename Publication::id_type const &id) const {

        auto publicationNodePtr = getNode(id);

        std::vector<typename Publication::id_type> parent_ids;

        for (auto &parent : publicationNodePtr->parents)
            if (!parent.expired())
                // Stworzony shared_ptr znika po jego użyciu
                parent_ids.emplace_back(parent.lock()->publication.get_id());

        return parent_ids;
    }

    // Zwraca referencję do obiektu reprezentującego publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
    // nie istnieje.
    Publication &operator[](typename Publication::id_type const &id) const {

        return getNode(id)->publication;
    }

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // podanych identyfikatorach parent_ids. Zgłasza wyjątek PublicationAlreadyCreated,
    // jeśli publikacja o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound,
    // jeśli któryś z wyspecyfikowanych poprzedników nie istnieje albo lista poprzedników jest pusta.
    void create(typename Publication::id_type const &id, std::vector<typename Publication::id_type> const &parent_ids) {

        if (exists(id))
            throw PublicationAlreadyCreated();

        if (parent_ids.empty())
            throw PublicationNotFound();

        // Wektor na rodziców nowego wierzchołka
        std::vector<std::weak_ptr<Node> > parents;
        // Wektor wskaźników na ojców nowego wierzchołka
        std::vector<Node *> parentsPtr;
        // Wektor na ojców i pozycje ich nowych dzieci w ich zbiorach
        std::vector<std::pair<Node *, std::pair<typename std::set<std::shared_ptr<Node>>::iterator, bool> > > parentsPtrsAndNewChildPosition;

        for (const auto &parent_id : parent_ids) {

            // Jeśli getNode rzuci wyjątek, to jest wszystko dobrze, bo edycja
            // obiektu nie została jeszcze rozpoczęta
            auto helper = getNode(parent_id);
            parents.emplace_back(std::weak_ptr<Node>(helper));
            parentsPtr.emplace_back(helper.get());
        }

        // Stworzenie nowego węzła, ale jeszcze bez dodania go do grafu
        auto newNode = std::make_shared<Node>(id, parents);

        try {

            // Próby wprowadzenia zmian w obiekcie, które mogą rzucać wyjątki
            for (const auto &parentPtr : parentsPtr) {
                parentsPtrsAndNewChildPosition.push_back(
                        std::make_pair(parentPtr, parentPtr->children.insert(std::shared_ptr<Node>(newNode))));
            }

            allPublications[id] = std::weak_ptr<Node>(newNode);
        } catch (...) {

            // W przypadku wyjątku cofamy zmiany i przekazujemy wyjątek dalej
            for (const auto &parentPtrAndNewChildPosition : parentsPtrsAndNewChildPosition)
                if (parentPtrAndNewChildPosition.second.second)
                    // Usuwanie elementu z konkretnej pozycji, z porównywaniem noexcept (shared_ptr ma noexcept
                    // porównywanie) która jest w secie jest noexcept
                    // http://www.cplusplus.com/reference/set/set/erase/
                    parentPtrAndNewChildPosition.first->children.erase(
                            parentPtrAndNewChildPosition.second.first);

            throw;
        }
    }

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
    // parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
    // o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
    // poprzednik nie istnieje
    void create(typename Publication::id_type const &id, typename Publication::id_type const &parent_id) {

        return create(id, std::vector<typename Publication::id_type>({parent_id}));
    }


    // Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
    // jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(typename Publication::id_type const &child_id, typename Publication::id_type const &parent_id) {

        auto childPublicationNodePtr = getNode(child_id);
        auto parentPublicationNodePtr = getNode(parent_id);

        if (parentPublicationNodePtr->children.find(std::shared_ptr<Node>(childPublicationNodePtr)) !=
            parentPublicationNodePtr->children.end())
            return;


        childPublicationNodePtr->parents.emplace_back(std::weak_ptr<Node>(parentPublicationNodePtr));

        try {
            parentPublicationNodePtr->children.emplace(std::shared_ptr<Node>(childPublicationNodePtr));
        } catch (std::exception &err) {
            childPublicationNodePtr->parents.pop_back();
            throw;
        }

    }

    // Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
    // PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
    // TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    // W wypadku rozspójnienia grafu, zachowujemy tylko spójną składową zawierającą źródło.
    void remove(typename Publication::id_type const &id) {

        // Sprawdzenie istnienia i zgłaszanie wyjątku jest w getNode
        auto publicationNodePtr = getNode(id);

        if (publicationNodePtr == root)
            throw TriedToRemoveRoot();


        // Zbiór na wskaźniki na ojców i iterator na tego syna do usunięcia (tworzymy najpierw, bo find może rzucać wyjątki
        // Trzymam zbiór po to, żeby nie usuwać wielokrotnie z tego samego zbioru jeśli jakiś ojciec był cytowany wiele razy
        std::set<std::pair<std::shared_ptr<Node>, typename std::set<std::shared_ptr<Node>>::iterator>,
                pairFirstElemCompare> parentsPtrsAndOldChildPtr;

        // Stworzenie zbiorów starych dzieci z ojcami
        for (auto &parent : publicationNodePtr->parents)
            if (!parent.expired()) {
                auto helper = parent.lock();
                auto childHelper = helper->children.find(publicationNodePtr);
                if (childHelper != helper->children.end())
                    parentsPtrsAndOldChildPtr.insert(std::make_pair(helper, childHelper));
            }

        // Usunięcie dziecka z list synów ojców
        for (auto &parentPtrAndOldChildPtr : parentsPtrsAndOldChildPtr)
            // Usuwanie elementu z konkretnej pozycji, z porównywaniem noexcept (shared_ptr ma noexcept
            // porównywanie) która jest w secie jest noexcept
            // http://www.cplusplus.com/reference/set/set/erase/
            parentPtrAndOldChildPtr.first->children.erase(parentPtrAndOldChildPtr.second);

        // Usunięcie wierzchołka. Powoduje kaskadowe usunięcie potomków
    }
};

#endif //JNP5_CITATION_GRAPH_H
