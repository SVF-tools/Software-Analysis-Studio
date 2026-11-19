//===- Assignment-4.h -- Automated assertion-based verification (Static symbolic execution) --//
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
 * Automated assertion-based verification (Static symbolic execution)
 *
 * Created on: Feb 19, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_4_H
#define SOFTWARE_SECURITY_ANALYSIS_ASSIGNMENT_4_H

#include "SVF-LLVM/PAGBuilder.h"
#include "Z3SSEMgr.h"
#include <stdlib.h>

namespace SVF {

	class SSE {
	 public:
		typedef std::vector<const ICFGNode*> CallStack;
		typedef std::pair<const ICFGEdge*, CallStack> ICFGEdgeStackPair;

		/// Constructor
		SSE(PAG* s, ICFG* i)
		: svfir(s)
		, icfg(i) {
			z3Mgr = new Z3SSEMgr(s);
		}
		/// Destructor
		virtual ~SSE() {
			delete z3Mgr;
		}

		/// Identify source which is the program entry ICFGNode (GlobalICFGNode)
		std::set<const ICFGNode*>& identifySources() {
			sources.insert(icfg->getGlobalICFGNode());
			return sources;
		}

		/// Identify the sink nodes which are assertion ICFGNodes
		std::set<const ICFGNode*>& identifySinks() {
			for (const CallICFGNode* cs : svfir->getCallSiteSet()) {
				const FunObjVar* fun = cs->getCalledFunction();
				if (isAssertFun(fun))
					sinks.insert(cs);
			}
			return sinks;
		}

		/// Return true if this function is an assert function
		inline bool isAssertFun(const FunObjVar* fun) const {
			return (fun != NULL
			        && (fun->getName() == "assert" || fun->getName() == "svf_assert" || fun->getName() == "sink"));
		}

		/// reset z3 solver
		virtual void resetSolver() {
			getSolver().reset();
			callingCtx.clear();
		}

		/// TODO: Implementing the collection the ICFG paths
		virtual void collectAndTranslatePath();

		/// Depth-first-search ICFGTraversal on ICFG from src node to snk node
		void reachability(const ICFGEdge* curNode, const ICFGNode* snk);

		void analyse();

		virtual void handleCall(const CallCFGEdge* call);
		virtual void handleRet(const RetCFGEdge* ret);
		virtual bool handleIntra(const IntraCFGEdge* edge) {
			if (edge->getCondition()) {
				if (handleBranch(edge) == false)
					return false;
				else {
					// if edge is from "br if.end" to stmt after if.end, should handle it as non branch
					return handleNonBranch(edge);
				}
			}
			else {
				return handleNonBranch(edge);
			}
		}
		bool handleNonBranch(const IntraCFGEdge* edge);
		bool handleBranch(const IntraCFGEdge* edge);

		/// Encode the path into Z3 constraints and return true if the path is feasible, false otherwise.
		bool translatePath(std::vector<const ICFGEdge*>& path);

		/// Return true if svf_assert check is successful
		bool assertchecking(const ICFGNode* inode) {
			assert_checked++;
			const CallICFGNode* callnode = SVFUtil::cast<CallICFGNode>(inode);
			assert(callnode && isAssertFun(callnode->getCalledFunction())
			       && "last node is not an assert call?");
			DBOP(std::cout << "\n## Analyzing " << callnode->toString() << "\n");
			z3::expr arg0 = getZ3Expr(callnode->getActualParms().at(0)->getId());
			addToSolver(arg0 == getCtx().int_val(0));
			if (getSolver().check() != z3::unsat) {
				DBOP(printExprValues());
				std::stringstream ss;
				ss << "The assertion is unsatisfiable!! ("<< inode->toString() << ")" << "\n";
				ss << "Counterexample: " << getSolver().get_model() << "\n";
				SVFUtil::outs() << ss.str() << std::endl;
				assert(false);
				return false;
			}
			else {
				DBOP(printExprValues());
				std::stringstream ss;
				ss << "The assertion is successfully verified!! ("<< inode->toString() << ")" << "\n";
				SVFUtil::outs() << ss.str() << std::endl;
				return true;
			}
		}

		std::set<std::string> getPaths() {
			return paths;
		}

		void pushCallingCtx(const ICFGNode* c) {
			callingCtx.push_back(c);
		}

		void popCallingCtx() {
			callingCtx.pop_back();
		}

		inline z3::solver& getSolver() {
			return z3Mgr->getSolver();
		}

		inline z3::context& getCtx() {
			return z3Mgr->getCtx();
		}

		/// Add expr to Z3 solver
		void addToSolver(z3::expr e) {
			DBOP(std::cout << "==> " << e.simplify() << "\n");
			getSolver().add(e);
		}

		/// Return Z3 expression based on ValVar ID
		inline z3::expr getZ3Expr(NodeID idx) const {
			return z3Mgr->getZ3Expr(idx, callingCtx);
		}

		/// Return Z3 expression based on ObjVar ID
		inline z3::expr getMemObjAddress(NodeID idx) const {
			return z3Mgr->getMemObjAddress(idx);
		}
		/// Return int value from an expression if it is a numeral, otherwise return an approximate value
		inline z3::expr getEvalExpr(z3::expr e) {
			return z3Mgr->getEvalExpr(e);
		}

		/// Dump values of all exprs
		inline void printExprValues() {
			z3Mgr->printExprValues(callingCtx);
		}

		static u32_t assert_checked;
		static u32_t assert_num;

	 private:
		Z3SSEMgr* z3Mgr;
		ICFG* icfg;
		std::set<std::string> paths;

	 protected:
		PAG* svfir;
		Set<ICFGEdgeStackPair> visited;
		CallStack callstack;
		CallStack callingCtx;
		std::vector<const ICFGEdge*> path;

		std::set<const ICFGNode*> sources;
		std::set<const ICFGNode*> sinks;
	};

} // namespace SVF

#endif
