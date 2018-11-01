//
// Created by michael on 31.10.18.
//

#include "graph/double_linked_hook_tree.hpp"

namespace graph {

    DoubleLinkedHookTree::DoubleLinkedHookTree(int n) : HookTree(n) {
        nodes_.resize(n);
    }

    void DoubleLinkedHookTree::hook(graph::HookTree::VertexIdx i, graph::HookTree::VertexIdx j) {
        VertexIdx rootI = representative(i);
        VertexIdx rootJ = representative(j);

        nodes_[rootI].parent = rootJ;
    }

    DoubleLinkedHookTree::VertexIdx DoubleLinkedHookTree::representative(graph::HookTree::VertexIdx i) const {
        VertexIdx rootId = i;

        while(nodes_[rootId].parent >= 0) {
            rootId = nodes_[rootId].parent;
        }

        return rootId;
    }

    bool DoubleLinkedHookTree::isStar(graph::HookTree::VertexIdx i) const {
        VertexIdx rootId = representative(i);

        // check if any children of the root have children of their own => it's not a star
        for(VertexIdx childId : nodes_[rootId].children) {
            if(nodes_[childId].children.size() > 0) {
                return false;
            }
        }

        return true;
    }

    bool DoubleLinkedHookTree::isSingleton(graph::HookTree::VertexIdx i) const {
        return nodes_[i].parent == -1 && nodes_[i].children.size() == 0; // node is root AND does not have any children
    }

    void DoubleLinkedHookTree::makeStar(graph::HookTree::VertexIdx i) {
        VertexIdx rootId = representative(i);

        // loop over all grand children to set theirs root and remove children
        for(VertexIdx grandChildId : getChildrenIds(rootId, 1)) {
            nodes_[grandChildId].parent = rootId; // set parent of grandchild to root
            nodes_[grandChildId].children.resize(0); // grandchild cannot have any children now
        }

        // loop over all children to remove their children
        for(VertexIdx childId : nodes_[rootId].children) {
            nodes_[childId].children.resize(0);
        }
    }


    std::vector<DoubleLinkedHookTree::VertexIdx> DoubleLinkedHookTree::getChildrenIds(graph::HookTree::VertexIdx i, int level) const {

        std::vector<VertexIdx> result;

        for(VertexIdx childId : nodes_[i].children) {

            // check if current children should already be added
            if(level == 0) {
                result.push_back(childId);
            }

            // add children one level deeper
            for(VertexIdx grandChildId : getChildrenIds(childId, std::max(level-1, 0))) {
                result.push_back(grandChildId);
            }
        }

        return  result;

    }


}
