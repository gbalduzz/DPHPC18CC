//
// Created by michael on 31.10.18.
//

#ifndef MINCUT_DOUBLE_LINKED_HOOK_TREE_H
#define MINCUT_DOUBLE_LINKED_HOOK_TREE_H

#include "hook_tree.hpp"

namespace graph {

    class DoubleLinkedHookTree : public HookTree {

    public:

        DoubleLinkedHookTree(int n);

        virtual void hook(VertexIdx i, VertexIdx j);

        virtual VertexIdx representative(VertexIdx i) const;

        virtual bool isStar(VertexIdx i) const;

        virtual bool isSingleton(VertexIdx i) const;

        virtual void makeStar(VertexIdx i);

    protected:

        struct Node {
            VertexIdx parent = -1; // id in nodes_ array of parent node
            std::vector<VertexIdx> children; // list of children ids in nodes_ array
        };

        std::vector<Node> nodes_;

        virtual std::vector<VertexIdx> getChildrenIds(VertexIdx i, int level = 0) const;

    };

}

#endif //MINCUT_DOUBLE_LINKED_HOOK_TREE_H
