//===- SVF-Teaching Assignment 1-------------------------------------//
//
//     SVF: Static Value-Flow Analysis Framework for Source Code
//
// Copyright (C) <2013->
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // SVF-Teaching Assignment 1 : Graph Traversal
 //
 // 
 */


#include "../Assignment-1.h"
#include <assert.h>
#include <map>
#include <fstream>	// for GraphBuilderFromFile
#include <string>	// for GraphBuilderFromFile
#include <sstream>	// for GraphBuilderFromFile
using namespace std;

int totalPassedCases = 0;
//test
//build graph in brief way
void build_g(Graph* g, Node* n1, Node* n2)
{
    Edge *edge = new Edge(n1, n2);
    n1->addOutEdge(edge);
    g->addNode(n1);
    g->addNode(n2);

}

/*
 * You can build a Graph from a file written by yourself
 *
 * The file should follow the format:
 * Node:  nodeID 
 * Edge:  nodeID -> NodeID 
 *
 * like:
1 
2 
3 
1 -> 2
2 -> 3
 */
void Test(std::string file){

    std::map<int,Node*> id2Nodes;
    std::set<std::string> expected;
    Graph *g = new Graph();
    int source = 0;
    int target = 0;
    string line;
    ifstream myfile(file.c_str());
    if (myfile.is_open())
    {
        while (myfile.good())
        {
            getline(myfile, line);

            int token_count = 0;
            string tmps;
            istringstream ss(line);
            while (ss.good())
            {
                ss >> tmps;
                token_count++;
            }

            if (token_count == 0 || token_count == 1)
                continue;

            /// create nodes
            else if (token_count == 2)
            {
                string dummy;
                int nodeId;
                string nodetype;
                istringstream ss(line);
                ss >> dummy;
                ss >> nodeId;
                Node *node = new Node(nodeId);
                id2Nodes[nodeId] = node;
                g->addNode(node);
            }
            /// create edges
            else if (token_count == 3)
            {
                int nodeSrc;
                int nodeDst;
                string ed;
                istringstream ss(line);
                ss >> nodeSrc;
                ss >> ed;
                ss >> nodeDst;
                assert(id2Nodes.find(nodeSrc)!=id2Nodes.end() && "src node has not been created");
                assert(id2Nodes.find(nodeDst)!=id2Nodes.end() && "dst node has not been created");
                Edge *edge = new Edge(id2Nodes[nodeSrc], id2Nodes[nodeDst]);
                id2Nodes[nodeSrc]->addOutEdge(edge);
            }
            /// correct answer "1 => 2 START: 1->2->4->5->END"
            else if (token_count == 5){
                string dummy;
                string start;
                string end;
                istringstream ss(line);
                ss >> source;
                ss >> dummy;
                ss >> target;
                ss >> start;
                ss >> end;
                expected.insert(start + " " + end);
            }
            else{
              assert(false && "format not supported");
            }
        }
    }

    std::set<const Node *> visited;
    std::vector<const Node *> path;
    GraphTraversal *dfs = new GraphTraversal();
    assert(id2Nodes.find(source)!=id2Nodes.end() && "source node was not found!");
    assert(id2Nodes.find(target)!=id2Nodes.end() && "target node was not found!");
    dfs->DFS(visited, path, id2Nodes[source], id2Nodes[target]);

    string filename = file.substr(file.find_last_of("/") + 1);
    if(dfs->getPaths() == expected){
      std::cerr << "Testcase " << filename << " passed!\n";
      totalPassedCases++;
    }
    else
      std::cerr << "Testcase " << filename << " failed!\n";

    delete dfs;
    delete g;

}

int main(int argc, char *argv[])
{

  assert(argc == 2 && "only allow one input file to build graph");

  Test(argv[1]);

  return 0;
}