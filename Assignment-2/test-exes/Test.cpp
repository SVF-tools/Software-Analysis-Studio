//===- SVF-Teaching Assignment 2-------------------------------------//
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
 // Assignment 2 : Source Sink ICFG DFS Traversal
 //
 // 
 */

#include <iostream>
#include "../Assignment-2.h"

void Test1(std::vector<std::string>& moduleNameVec)
{
    // Directly call buildSVFModule
    LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    /// Build Program Assignment Graph (PAG)
    PAGBuilder builder;
    PAG *pag = builder.build();
    ICFG *icfg = pag->getICFG();
    icfg->dump("icfg");
    std::vector<const ICFGNode *> path;
    std::stack<const ICFGNode *>callstack;
    std::set<const ICFGNode *> visited;
    ICFGTraversal *gt = new ICFGTraversal(pag);
    for (const CallICFGNode *src : gt->identifySources())
    {
        for (const CallICFGNode *snk : gt->identifySinks())
        {
            gt->reachability(src, snk);
        }
    }
    for(auto path : gt->getPaths())
        std::cerr << path << "\n";

    // get the filename test1.ll or test2.ll or test3.ll
    std::string fullpath = moduleNameVec[0];
    std::string filename = fullpath.substr(fullpath.rfind('/') + 1);
    if (filename == "test1.ll") {
        // assert if "START->3->4->5->END"
        assert(gt->getPaths().size() == 1 && "test1.ll Error: path size cannot match");
        assert(gt->getPaths().count("START->3->4->5->END") 
               && "Path START->3->4->5->END not found in test1.ll");
        SVFUtil::outs() << SVFUtil::sucMsg("test1.ll pass\n");
    }
    else if (filename == "test2.ll") {
        // assert if START->3->4->5->6->7->8->9->END
        // START->3->4->5->6->7->END
        // START->5->6->7->8->9->END
        // START->5->6->7->END
        assert(gt->getPaths().size() == 4 && "test2.ll Error: path size cannot match");

        assert(gt->getPaths().count("START->5->6->7->8->9->END")
               && "Path START->5->6->7->8->9->END not found in test2.ll");
        assert(gt->getPaths().count("START->5->6->7->END")
               && "Path START->5->6->7->END not found in test2.ll");
        assert(gt->getPaths().count("START->3->4->5->6->7->8->9->END")
               && "Path START->3->4->5->6->7->8->9->END not found in test2.ll");
        assert(gt->getPaths().count("START->3->4->5->6->7->END")
               && "Path START->3->4->5->6->7->END not found in test2.ll");
        SVFUtil::outs() << SVFUtil::sucMsg("test2.ll pass\n");

    } else if (filename == "test3.ll") {
        // assert if START->6->7->8->1->5->2->9->10->END
        assert(gt->getPaths().size() == 1 && "test3.ll Error: path size cannot match");
        assert(gt->getPaths().count("START->6->7->8->1->5->2->9->10->END")
               && "Path START->6->7->8->1->5->2->9->10->END not found in test3.ll");
        SVFUtil::outs() << SVFUtil::sucMsg("test3.ll pass\n");
    }

    PAG::releasePAG();
    LLVMModuleSet::releaseLLVMModuleSet();
    delete gt;
}



int main(int argc, char *argv[])
{
    int arg_num = 0;
    int extraArgc = 1;
    char **arg_value = new char *[argc + extraArgc];
    for (; arg_num < argc; ++arg_num) {
        arg_value[arg_num] = argv[arg_num];
    }

    // You may comment it to see the details of the analysis
    arg_value[arg_num++] = (char*) "-stat=false";

    std::vector<std::string> moduleNameVec;
    moduleNameVec = OptionBase::parseOptions(
            arg_num, arg_value, "Teaching-Software-Analysis Assignment 2", "[options] <input-bitcode...>"
    );

    Test1(moduleNameVec);
    return 0;
}
