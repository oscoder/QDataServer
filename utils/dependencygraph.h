#ifndef UTILS_DEPENDENCYGRAPH_H
#define UTILS_DEPENDENCYGRAPH_H

#include "utils_global.h"

#include <QtCore/QBitArray>
#include <QtCore/QMultiMap>
#include <QtCore/QSet>
#include <QtCore/QVector>

namespace Utils {

//! Utility to sort things by dependencies
/*!
 * When no dependency is specified for two nodes, the final order of the two
 * nodes is \e undefined, unless the \a sortIndependent template argument is
 * set to \c true -- in that case the two nodes will be ordered by their
 * values.
 *
 * Example:
 *
 * \code
 * DependencyGraph<QString, true> dg;
 * dg.addNode("A");
 * dg.addNode("B");
 * dg.addNode("C");           // Graphical representation
 * dg.addNode("D");           //
 * dg.addNode("E");           //
 * dg.addNode("F");           //   [ A ] [ B ] [ D ]
 * dg.addNode("G");           //      \   / \   / |
 * dg.addNode("H");           //       \ /   \ /  |
 * dg.addNode("I");           //      [ C ] [ E ] |
 * dg.addEdge("C", "A");      //       / \   /    |
 * dg.addEdge("C", "B");      //      /   \ /     |
 * dg.addEdge("E", "B");      //     /   [ F ]    |
 * dg.addEdge("E", "D");      //    /    / | \    |
 * dg.addEdge("F", "C");      //    |   /  |  \   |
 * dg.addEdge("F", "E");      //    |  /   |   \  |
 * dg.addEdge("G", "C");      //   [ G ] [ H ] [ I ]
 * dg.addEdge("G", "F");      //
 * dg.addEdge("H", "F");
 * dg.addEdge("I", "F");
 * dg.addEdge("I", "D");
 * \endcode
 *
 * Here is how the nodes will be sorted:
 *
 * \code
 * dg.sort() --> A, B, C, D, E, F, G, H, I
 * \endcode
 */
template<class T, bool sortIndependent = false>
class DependencyGraph
{
public:
    DependencyGraph();

public:
    void addNode(const T &node);
    void addEdge(const T &dependentNode, const T &requiredNode);
    QList<T> sort() const;

private:
    inline bool hasEdge(int dependentNodeIdx, int requiredNodeIdx) const;

private:
    /*! \cond false */
    template<bool b> class BoolToType
    {
    };
    /*! \endcond */

    QList<T> m_nodes;
    QVector<QBitArray> m_edges;

    mutable bool m_cacheIsValid;
    mutable QList<T> m_cache;
};

//! Uses order of addition (FIFO) as key for independent nodes ordering
/*!
 * Instead of comparing values, ordering for two nodes with no dependency
 * specified is derived from the order they were added. The ordering is derived
 * as FIFO (First-In-First-Out). For LIFO (Last-In-First-Out) ordering use
 * LifoDependencyGraph instead.
 *
 * Consider these two dependency graphs as an example:
 *
 * (The numbers next to the node names tell you the order which the nodes has
 * been added in.)
 *
 * \code
 * FifoDependencyGraph<UniqueId> dg1;
 * dg1.addNode("A");
 * dg1.addNode("B");
 * dg1.addNode("C");           //
 * dg1.addNode("D");           // Graphical representation
 * dg1.addNode("E");           //
 * dg1.addNode("F");           //   [A#1] [B#2] [D#4]
 * dg1.addNode("G");           //      \   / \   / |
 * dg1.addNode("H");           //       \ /   \ /  |
 * dg1.addNode("I");           //      [C#3] [E#5] |
 * dg1.addEdge("C", "A");      //       / \   /    |
 * dg1.addEdge("C", "B");      //      /   \ /     |
 * dg1.addEdge("E", "B");      //     /   [F#6]    |
 * dg1.addEdge("E", "D");      //    /    / | \    |
 * dg1.addEdge("F", "C");      //    |   /  |  \   |
 * dg1.addEdge("F", "E");      //    |  /   |   \  |
 * dg1.addEdge("G", "C");      //   [G#7] [H#8] [I#9]
 * dg1.addEdge("G", "F");      //
 * dg1.addEdge("H", "F");
 * dg1.addEdge("I", "F");
 * dg1.addEdge("I", "D");
 * \endcode
 *
 * \code
 * FifoDependencyGraph<UniqueId> dg2;
 * dg2.addNode("A");
 * dg2.addNode("B");
 * dg2.addNode("D");           //
 * dg2.addNode("C");           // Graphical representation
 * dg2.addNode("E");           //
 * dg2.addNode("F");           //   [A#1] [B#2] [D#3]
 * dg2.addNode("H");           //      \   / \   / |
 * dg2.addNode("I");           //       \ /   \ /  |
 * dg2.addNode("G");           //      [C#4] [E#5] |
 * dg2.addEdge("C", "A");      //       / \   /    |
 * dg2.addEdge("C", "B");      //      /   \ /     |
 * dg2.addEdge("E", "B");      //     /   [F#6]    |
 * dg2.addEdge("E", "D");      //    /    / | \    |
 * dg2.addEdge("F", "C");      //    |   /  |  \   |
 * dg2.addEdge("F", "E");      //    |  /   |   \  |
 * dg2.addEdge("G", "C");      //   [G#9] [H#7] [I#8]
 * dg2.addEdge("G", "F");      //
 * dg2.addEdge("H", "F");
 * dg2.addEdge("I", "F");
 * dg2.addEdge("I", "D");
 * \endcode
 *
 * Here is how the nodes of these two graphs will be sorted:
 *
 * \code
 * dg1.sort() --> A, B, C, D, E, F, G, H, I
 * dg2.sort() --> A, B, D, C, E, F, H, I, G
 * \endcode
 *
 * As you should see, the two graphs are \e identical. The only difference is
 * the order the nodes has been added into the graphs. And that's why the
 * results of sort() differs.
 *
 * If the LifoDependencyGraph class were used, the results would be
 *
 * \code
 * dg1.sort() --> D, B, E, A, C, F, I, H, G
 * dg2.sort() --> D, B, E, A, C, F, G, I, H
 * \endcode
 *
 * \see LifoDependencyGraph
 */
template<class T>
class FifoDependencyGraph
{
public:
    void addNode(const T &node);
    void addEdge(const T &dependentNode, const T &requiredNode);
    QList<T> sort() const;

private:
    QList<T> m_nodes;
    DependencyGraph<int, true> m_slave;
};

//! Uses order of addition (LIFO) as key for independent nodes ordering
/*!
 * See FifoDependencyGraph for more information, examples and comparison.
 */
template<class T>
class LifoDependencyGraph
{
public:
    void addNode(const T &node);
    void addEdge(const T &dependentNode, const T &requiredNode);
    QList<T> sort() const;

private:
    QList<T> m_nodes;
    DependencyGraph<int, true> m_slave;
};

//! In addition to FIFO ordering allows to group (interlace) independent nodes
/*!
 * Example: consider set of nodes
 *
 * \code
 * A1, B1, C1, A2, B2, C2, ... An, Bn, Cn
 * \endcode
 *
 * If you need to simulate they were added in order
 *
 * \code
 * A1, A2, ..., An, B1, B2, ..., Bn, C1, C2, ..., Cn
 * \endcode
 *
 * but you do not know the number of node tripples following (the \c n number),
 * you can use this:
 *
 * \code
 * StripedFifoDependencyGraph<QString> dg;
 * dg.addNode("A1", 1);
 * dg.addNode("B1", 2);
 * dg.addNode("C1", 3);
 * dg.addNode("A2", 1);
 * dg.addNode("B2", 2);
 * dg.addNode("C2", 3);
 * ...
 * dg.addNode("A7", 1);
 * dg.addNode("B7", 2);
 * dg.addNode("C7", 3);
 * ...
 * dg.addNode("An", 1);
 * dg.addNode("Bn", 2);
 * dg.addNode("Cn", 3);
 * \endcode
 *
 *
 * \see StripedLifoDependencyGraph
 */
template<class T>
class StripedFifoDependencyGraph
{
public:
    void addNode(const T &node, int stripe);
    void addEdge(const T &dependentNode, const T &requiredNode);
    QList<T> sort() const;

private:
    QList<T> m_nodes;
    QList<int> m_stripes;
    typedef QPair<int/* stripe*/, int/* nodeIdx*/> SlaveNode;
    DependencyGraph<SlaveNode, true> m_slave;
};

//! In addition to LIFO ordering allows to group (interlace) independent nodes
/*!
 * See StripedFifoDependencyGraph for more information and example.
 */
template<class T>
class StripedLifoDependencyGraph
{
public:
    void addNode(const T &node, int stripe);
    void addEdge(const T &dependentNode, const T &requiredNode);
    QList<T> sort() const;

private:
    QList<T> m_nodes;
    QList<int> m_stripes;
    typedef QPair<int/* stripe*/, int/* -nodeIdx*/> SlaveNode;
    DependencyGraph<SlaveNode, true> m_slave;
};

template<class T, bool sortIndependent>
DependencyGraph<T, sortIndependent>::DependencyGraph()
    : m_cacheIsValid(false)
{
}

//! Adds a node
template<class T, bool sortIndependent>
void DependencyGraph<T, sortIndependent>::addNode(const T &node)
{
    Q_ASSERT(!m_nodes.contains(node));

    m_nodes.append(node);

    m_cacheIsValid = false;
}

//! Adds an edge to express dependency between two nodes
template<class T, bool sortIndependent>
void DependencyGraph<T, sortIndependent>::addEdge(const T &dependentNode, const
        T &requiredNode)
{
    Q_ASSERT(m_nodes.contains(dependentNode));
    Q_ASSERT(m_nodes.contains(requiredNode));

    const int dependentNodeIdx = m_nodes.indexOf(dependentNode);
    const int requiredNodeIdx = m_nodes.indexOf(requiredNode);

    const int requiredSize = qMax(dependentNodeIdx, requiredNodeIdx) + 1;
    if (m_edges.size() < requiredSize) {
        for (int i = 0; i < m_edges.size(); i++) {
            m_edges[i].resize(requiredSize);
        }
        while (m_edges.size() < requiredSize) {
            m_edges.append(QBitArray(requiredSize));
        }
    }

    m_edges[dependentNodeIdx].setBit(requiredNodeIdx);

    m_cacheIsValid = false;
}

template<class T, bool sortIndependent>
inline bool DependencyGraph<T, sortIndependent>::hasEdge(int dependentNodeIdx,
        int requiredNodeIdx) const
{
    return qMax(dependentNodeIdx, requiredNodeIdx) < m_edges.size() &&
        m_edges.at(dependentNodeIdx).testBit(requiredNodeIdx);
}

//! Performs topological sort on the dependency graph
template<class T, bool sortIndependent>
QList<T> DependencyGraph<T, sortIndependent>::sort() const
{
    // To not force 'T' to have defined comparison operator
    struct Helper
    {
        static QList<int> nodeNumbers(const QList<T> &nodes, BoolToType<true>
                sortedByValue)
        {
            Q_UNUSED(sortedByValue);
            QMap<T, int> nodeNumbersByValues;
            foreach (T node, nodes) {
                nodeNumbersByValues.insert(node, nodeNumbersByValues.count());
            }
            return nodeNumbersByValues.values();
        }
        static QList<int> nodeNumbers(const QList<T> &nodes, BoolToType<false>
                sortedByValue)
        {
            Q_UNUSED(sortedByValue);
            QList<int> nodeNumbers;
            nodeNumbers.reserve(nodes.count());
            for (int i = 0; i < nodes.count(); i++) {
                nodeNumbers.append(i);
            }
            return nodeNumbers;
        }
    };

    if (!m_cacheIsValid) {
        m_cache.clear();

        QList<int> remainingNodeNumbers = Helper::nodeNumbers(m_nodes,
                BoolToType<sortIndependent>());

        while (true) {
            int independentNodeNumber = -1;
            foreach (int nodeNumber, remainingNodeNumbers) {
                bool independent = true;
                foreach (int node2Number, remainingNodeNumbers) {
                    if (hasEdge(nodeNumber, node2Number)) {
                        independent = false;
                        break;
                    }
                }

                if (independent) {
                    independentNodeNumber = nodeNumber;
                    break;
                }
            }

            if (independentNodeNumber == -1)
                break;

            m_cache.append(m_nodes.at(independentNodeNumber));
            remainingNodeNumbers.removeOne(independentNodeNumber);
        }

        Q_ASSERT_X(remainingNodeNumbers.isEmpty(), Q_FUNC_INFO,
                "Circular dependency detected!");

        m_cacheIsValid = true;
    }

    return m_cache;
}

//! Adds a node
template<class T>
void FifoDependencyGraph<T>::addNode(const T &node)
{
    Q_ASSERT(!m_nodes.contains(node));
    m_slave.addNode(m_nodes.count());
    m_nodes.append(node);
}

//! Adds an edge to express dependency between two nodes
template<class T>
void FifoDependencyGraph<T>::addEdge(const T &dependentNode, const T
        &requiredNode)
{
    Q_ASSERT(m_nodes.contains(dependentNode));
    Q_ASSERT(m_nodes.contains(requiredNode));

    m_slave.addEdge(m_nodes.indexOf(dependentNode),
            m_nodes.indexOf(requiredNode));
}

//! Performs topological sort on the dependency graph
template<class T>
QList<T> FifoDependencyGraph<T>::sort() const
{
    QList<T> retv;
    foreach (int i, m_slave.sort()) {
        retv.append(m_nodes.at(i));
    }
    return retv;
}

//! Adds a node
template<class T>
void LifoDependencyGraph<T>::addNode(const T &node)
{
    Q_ASSERT(!m_nodes.contains(node));
    m_slave.addNode(-m_nodes.count());
    m_nodes.append(node);
}

//! Adds an edge to express dependency between two nodes
template<class T>
void LifoDependencyGraph<T>::addEdge(const T &dependentNode, const T
        &requiredNode)
{
    Q_ASSERT(m_nodes.contains(dependentNode));
    Q_ASSERT(m_nodes.contains(requiredNode));

    m_slave.addEdge(-m_nodes.indexOf(dependentNode),
            -m_nodes.indexOf(requiredNode));
}

//! Performs topological sort on the dependency graph
template<class T>
QList<T> LifoDependencyGraph<T>::sort() const
{
    QList<T> retv;
    foreach (int i, m_slave.sort()) {
        retv.append(m_nodes.at(-i));
    }
    return retv;
}

//! Adds a node
template<class T>
void StripedFifoDependencyGraph<T>::addNode(const T &node, int stripe)
{
    Q_ASSERT(!m_nodes.contains(node));
    m_slave.addNode(SlaveNode(stripe, m_nodes.count()));
    m_nodes.append(node);
    m_stripes.append(stripe);
}

//! Adds an edge to express dependency between two nodes
template<class T>
void StripedFifoDependencyGraph<T>::addEdge(const T &dependentNode, const T
        &requiredNode)
{
    Q_ASSERT(m_nodes.contains(dependentNode));
    Q_ASSERT(m_nodes.contains(requiredNode));

    const int dIdx = m_nodes.indexOf(dependentNode);
    const int rIdx = m_nodes.indexOf(requiredNode);
    m_slave.addEdge(SlaveNode(m_stripes.at(dIdx), dIdx),
            SlaveNode(m_stripes.at(rIdx), rIdx));
}

//! Performs topological sort on the dependency graph
template<class T>
QList<T> StripedFifoDependencyGraph<T>::sort() const
{
    QList<T> retv;
    foreach (const SlaveNode &i, m_slave.sort()) {
        retv.append(m_nodes.at(i.second));
    }
    return retv;
}

//! Adds a node
template<class T>
void StripedLifoDependencyGraph<T>::addNode(const T &node, int stripe)
{
    Q_ASSERT(!m_nodes.contains(node));
    m_slave.addNode(SlaveNode(stripe, -m_nodes.count()));
    m_nodes.append(node);
    m_stripes.append(stripe);
}

//! Adds an edge to express dependency between two nodes
template<class T>
void StripedLifoDependencyGraph<T>::addEdge(const T &dependentNode, const T
        &requiredNode)
{
    Q_ASSERT(m_nodes.contains(dependentNode));
    Q_ASSERT(m_nodes.contains(requiredNode));

    const int dIdx = m_nodes.indexOf(dependentNode);
    const int rIdx = m_nodes.indexOf(requiredNode);
    m_slave.addEdge(SlaveNode(m_stripes.at(dIdx), -dIdx),
            SlaveNode(m_stripes.at(rIdx), -rIdx));
}

//! Performs topological sort on the dependency graph
template<class T>
QList<T> StripedLifoDependencyGraph<T>::sort() const
{
    QList<T> retv;
    foreach (const SlaveNode &i, m_slave.sort()) {
        retv.append(m_nodes.at(-i.second));
    }
    return retv;
}

} //namespace Utils

#endif //UTILS_DEPENDENCYGRAPH_H
