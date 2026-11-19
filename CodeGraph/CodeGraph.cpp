//=== Software-Analysis-Teaching CodeGraph.cpp -- -------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
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
 // PAG ICFG Generation
 //
 // Author: Guanqin Zhang  email: 152585@uts.edu.au
 */

#include "SVF-LLVM/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SABER/LeakChecker.h"
#include "SVF-LLVM/PAGBuilder.h"


using namespace SVF;
using namespace llvm;
using namespace std;


int main(int argc, char ** argv) {

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
            arg_num, arg_value, "Teaching-Software-Analysis Assignment 4", "[options]"
    );
 
    LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

    /// Build Program Assignment Graph (PAG or PAG)
    PAGBuilder builder;
    PAG *pag = builder.build();
    //dump pag
    pag->dump(PAG::getPAG()->getModuleIdentifier() + ".pag");
    /// ICFG
    ICFG *icfg = pag->getICFG();
    //dump icfg
    icfg->dump(PAG::getPAG()->getModuleIdentifier() + ".icfg");

        // iterate each ICFGNode on ICFG
    for(ICFG::iterator i = icfg->begin(); i != icfg->end(); i++)
    {
        ICFGNode *n = i->second;
        // SVFUtil::outs() << n->toString() << "\n";
        // for(ICFGEdge* edge : n->getOutEdges()){
        //     SVFUtil::outs() << edge->toString() << "\n";
        // }
    }

    // iterate each SVFVar on PAG
    for(PAG::iterator p = pag->begin(); p != pag->end();p++)
    {
        SVFVar *n = p->second;
        // SVFUtil::outs() << n->toString() << "\n";
        // for(SVFStmt* edge : n->getOutEdges()){
        //     SVFUtil::outs() << edge->toString() << "\n";
        // }
    }


    return 0;
}
