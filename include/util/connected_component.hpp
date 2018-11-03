/**
 * Dummy for testing purposes
 * 
 * */

#ifndef CONNECTED_COMPONENT_H

#define CONNECTED_COMPONENT_H

#include "graph/edge.hpp"
#include <vector>

namespace util
{
    class ConnectedComponent {
        public: 

            ConnectedComponent(std::vector<graph::Label> node_list) : node_list(node_list){} 
            /**
             * Returns true iff a node with label i and a node with label j are in this specific connected component
             * */
            bool are_connected(graph::Label label_i, graph::Label label_j){
                //There's a find method in the <algorithm> package that runs in O(n). 
                //However, since we need to find both values, it should be faster to do it on our own.
                for(int i = 0; i < node_list.size(); i++) {

                }

            }
        
        private: 
            std::vector<graph::Label> node_list; 
    }; 
} // util


#endif