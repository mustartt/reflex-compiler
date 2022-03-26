//
// Created by henry on 2022-03-24.
//

#ifndef REFLEX_SRC_TYPECHECKER_TOPOLOGICALSORT_TOPSORT_H_
#define REFLEX_SRC_TYPECHECKER_TOPOLOGICALSORT_TOPSORT_H_

#include <vector>
#include <deque>
#include <stdexcept>

namespace reflex {

class CyclicError : public std::runtime_error {
  public:
    explicit CyclicError(const std::string &arg) : std::runtime_error(arg) {}
};

template<class T>
class TopologicalSort {
    std::vector<std::vector<size_t>> adjacencyList;
    std::vector<T> nodes;
  public:
    explicit TopologicalSort(std::vector<T> nodes)
        : nodes(std::move(nodes)), adjacencyList(nodes.size()) {};

    void addNode(T node);
    void addDirectedEdge(size_t from, size_t to);

    std::vector<T> sort();
};

template<class T>
void TopologicalSort<T>::addNode(T node) {
    nodes.push_back(node);
    adjacencyList.emplace_back();
}

template<class T>
void TopologicalSort<T>::addDirectedEdge(size_t from, size_t to) {
    adjacencyList[from].push_back(to);
}

template<class T>
std::vector<T> TopologicalSort<T>::sort() {
    size_t n = adjacencyList.size();
    std::vector<size_t> inDegree(n);
    for (auto &edges: adjacencyList) {
        for (auto to: edges) {
            inDegree[to]++;
        }
    }
    std::deque<size_t> q;
    for (int i = 0; i < n; i++) {
        if (inDegree[i] == 0) {
            q.push_back(i);
        }
    }
    int index = 0;
    std::vector<size_t> order(n);
    while (!q.empty()) {
        size_t at = q.front();
        q.pop_front();
        order[index++] = at;
        for (size_t to: adjacencyList[at]) {
            inDegree[to]--;
            if (inDegree[to] == 0) {
                q.push_back(to);
            }
        }
    }
    if (index != n) {
        throw CyclicError("Cycle in graph.");
    }
    std::vector<T> results;
    for (auto i: order) {
        results.push_back(nodes[i]);
    }
    return results;
}

}

#endif //REFLEX_SRC_TYPECHECKER_TOPOLOGICALSORT_TOPSORT_H_
