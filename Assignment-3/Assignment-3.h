//===- Assignment-3.h -- Taint analysis ------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2022>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
/*
 * Graph reachability, Andersen's pointer analysis and taint analysis
 *
 * Created on: Feb 18, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_3_H
#define SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_3_H

#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/PAGBuilder.h"
#include "WPA/Andersen.h"
#include <filesystem>
namespace fs = std::filesystem;

#define CUR_DIR() (fs::path(__FILE__).parent_path())

using namespace SVF;

class AndersenPTA : public SVF::AndersenBase {
 public:
	// Constructor
	AndersenPTA(SVF::PAG* _pag)
	: AndersenBase(_pag){};

	// dump constraint graph
	void dump_consCG(std::string name) {
		consCG->dump(name);
	};

	void analyze() override;

 private:
	// To be implemented
	void solveWorklist() override;

	// Helper: initialise points-to sets from all Addr constraints
	void processAllAddr();

	/// Add copy edge on constraint graph
	virtual bool addCopyEdge(SVF::NodeID src, SVF::NodeID dst) override {
		if (consCG->addCopyCGEdge(src, dst))
			return true;
		else
			return false;
	}
	/// Return the field object (GepObjVar) given a struct object and a field index
	inline NodeID getGepObjVar(NodeID id, const APOffset& apOffset) {
		return consCG->getGepObjVar(id,apOffset);
	}
};

class ICFGTraversal {
 public:
	typedef std::vector<const ICFGNode*> CallStack;
	typedef std::pair<const ICFGNode*, CallStack> ICFGNodeCallStackPair;

 public:
	ICFGTraversal(PAG* p)
	: pag(p) {}

	/// TODO: Configure source and sink function names from SrcSnk.txt
	void readSrcSnkFromFile(const std::string& filename);

	/// TODO: Convert and store a collected ICFG path
	void collectICFGPath(std::vector<unsigned>& path);

	/// TODO: Perform context-sensitive reachability and collect reachable paths
	void reachability(const ICFGNode* src, const ICFGNode* snk);

	/// TODO: Return true if the source return value aliases a sink argument
	bool aliasCheck(const CallICFGNode* src, const CallICFGNode* snk);

	// The driver method for taint checking
	void taintChecking();

	// Identify source nodes on ICFG (i.e., call instruction with its callee function named 'src')
	virtual std::set<const CallICFGNode*>& identifySources() {
		for (const CallICFGNode* cs : pag->getCallSiteSet()) {
			const FunObjVar* fun = cs->getCalledFunction();
			if (checker_source_api.find(fun->getName()) != checker_source_api.end()) {
				sources.insert(cs);
			}
		}
		return sources;
	}

	// Identify sink nodes on ICFG (i.e., call instruction with its callee function named 'sink')
	virtual std::set<const CallICFGNode*>& identifySinks() {
		for (const CallICFGNode* cs : pag->getCallSiteSet()) {
			const FunObjVar* fun = cs->getCalledFunction();
			if (checker_sink_api.find(fun->getName()) != checker_sink_api.end()) {
				sinks.insert(cs);
			}
		}
		return sinks;
	}

	const std::set<std::string>& getPaths() {
		return paths;
	}

 protected:
	std::set<const CallICFGNode*> sources;
	std::set<const CallICFGNode*> sinks;
	Set<std::pair<const ICFGNode*, CallStack>> visited;
	CallStack callstack;

	PAG* pag;
	std::set<std::string> paths;
	std::vector<unsigned> path;

 private:
	AndersenPTA* ander;

	// default source and sink function name API if SrcSnk.txt is not added
	std::set<std::string> checker_source_api;
	std::set<std::string> checker_sink_api;
};

#endif
