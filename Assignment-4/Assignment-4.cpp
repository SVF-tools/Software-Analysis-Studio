//===- Assignment-4.cpp -- Automated assertion-based verification (Static symbolic execution) --//
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

#include "Assignment-4.h"
#include "Util/Options.h"

using namespace SVF;
using namespace SVFUtil;
using namespace llvm;
using namespace z3;

/// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path (once for any loop) from
/// You will need to collect each path from src node to snk node and then add the path to the `paths` set by
/// calling the `collectAndTranslatePath` method which is then trigger the path translation.
/// This implementation, slightly different from Assignment-1, requires ICFGNode* as the first argument.
void SSE::reachability(const ICFGEdge* curEdge, const ICFGNode* sink) {
	
}

/// TODO: collect each path once this method is called during reachability analysis, and
/// Collect each program path from the entry to each assertion of the program. In this function,
/// you will need (1) add each path into the paths set, (2) call translatePath to convert each path into Z3 expressions.
/// Note that translatePath returns true if the path is feasible, false if the path is infeasible. (3) If a path is feasible,
/// you will need to call assertchecking to verify the assertion (which is the last ICFGNode of this path).
void SSE::collectAndTranslatePath() {
	
}

/// TODO: Implement handling of function calls
void SSE::handleCall(const CallCFGEdge* calledge) {
	const ICFGNode* srcNode = calledge->getSrcNode();
	DBOP(std::cout << "\n## Analyzing "<< srcNode->toString() << "\n");

	CallICFGNode* callNode = SVFUtil::cast<CallICFGNode>(calledge->getSrcNode());
	FunEntryICFGNode* FunEntryNode = SVFUtil::cast<FunEntryICFGNode>(calledge->getDstNode());

	assert(callNode->getSVFStmts().size()==callNode->getActualParms().size() && "Numbers of CallPEs and ActualParms not the same?");

}

/// TODO: Implement handling of function returns
void SSE::handleRet(const RetCFGEdge* retEdge) {
    DBOP(std::cout << "\n## Analyzing "<< retEdge->getDstNode()->toString() << "\n");

    FunExitICFGNode* FunExitNode = SVFUtil::cast<FunExitICFGNode>(retEdge->getSrcNode());
    RetICFGNode* retNode = SVFUtil::cast<RetICFGNode>(retEdge->getDstNode());

    assert(retNode->getSVFStmts().size()<=1 && "We can only has one RetPE per function!");

}

/// TODO: Implement handling of branch statements inside a function
/// Return true if the path is feasible, false otherwise.
/// A given branch on the ICFG looks like the following:
///       	     ICFGNode1 (condition %cmp)
///       	     1	/    \  0
///       	  ICFGNode2   ICFGNode3
/// edge->getCondition() returns the branch condition variable (%cmp) of type SVFValue* (for if/else) or a numeric condition variable (for switch).
/// Given the condition variable, you could obtain the SVFVar ID via "edge->getCondition())->getId()"
/// edge->getCondition() returns nullptr if this IntraCFGEdge is not a branch.
/// edge->getSuccessorCondValue() returns the actual condition value (1/0 for if/else) when this branch/IntraCFGEdge is executed. For example, the successorCondValue is 1 on the edge from ICFGNode1 to ICFGNode2, and 0 on the edge from ICFGNode1 to ICFGNode3
bool SSE::handleBranch(const IntraCFGEdge* edge) {
	assert(edge->getCondition() && "not a conditional control-flow transfer?");
	expr cond = getZ3Expr(edge->getCondition()->getId());
	expr successorVal = getCtx().int_val((int) edge->getSuccessorCondValue());

	DBOP(std::cout << "@@ Analyzing Branch " << edge->toString() << "\n");

    return true;
}

/// TODO: Translate AddrStmt, CopyStmt, LoadStmt, StoreStmt, GepStmt and CmpStmt
/// Translate AddrStmt, CopyStmt, LoadStmt, StoreStmt, GepStmt, BinaryOPStmt, CmpStmt, SelectStmt, and PhiStmt
bool SSE::handleNonBranch(const IntraCFGEdge* edge) {
	const ICFGNode* dstNode = edge->getDstNode();
	const ICFGNode* srcNode = edge->getSrcNode();
	DBOP(if(!SVFUtil::isa<CallICFGNode>(dstNode) && !SVFUtil::isa<RetICFGNode>(dstNode)) std::cout << "\n## Analyzing "<< dstNode->toString() << "\n");

	for (const SVFStmt *stmt : dstNode->getSVFStmts())
	{
		if (const AddrStmt *addr = SVFUtil::dyn_cast<AddrStmt>(stmt))
		{
			// TODO: Implement handling of AddrStmt
		}
		else if (const CopyStmt *copy = SVFUtil::dyn_cast<CopyStmt>(stmt))
		{
			// TODO: Implement handling of CopyStmt
		}
		else if (const LoadStmt *load = SVFUtil::dyn_cast<LoadStmt>(stmt))
		{
			// TODO: Implement handling of LoadStmt
		}
		else if (const StoreStmt *store = SVFUtil::dyn_cast<StoreStmt>(stmt))
		{
			// TODO: Implement handling of StoreStmt
		}
		else if (const GepStmt *gep = SVFUtil::dyn_cast<GepStmt>(stmt))
		{
			// TODO: Implement handling of GepStmt
		}
		/// Given a CmpStmt "r = a > b"
		/// cmp->getOpVarID(0)/cmp->getOpVarID(1) returns the first/second operand, i.e., "a" and "b"
		/// cmp->getResID() returns the result operand "r" and cmp->getPredicate() gives you the predicate ">"
		/// Find the comparison predicates in "class CmpStmt:Predicate" under SVF/svf/include/SVFValues/SVFStatements.h
		/// You are only required to handle integer predicates, including ICMP_EQ, ICMP_NE, ICMP_UGT, ICMP_UGE, ICMP_ULT, ICMP_ULE, ICMP_SGT, ICMP_SGE, ICMP_SLE, ICMP_SLT
		/// We assume integer-overflow-free in this assignment
		else if (const CmpStmt *cmp = SVFUtil::dyn_cast<CmpStmt>(stmt))
		{
			// TODO: Implement handling of CmpStmt
		}
		else if (const BinaryOPStmt *binary = SVFUtil::dyn_cast<BinaryOPStmt>(stmt))
		{
			expr op0 = getZ3Expr(binary->getOpVarID(0));
			expr op1 = getZ3Expr(binary->getOpVarID(1));
			expr res = getZ3Expr(binary->getResID());
			switch (binary->getOpcode())
			{
			case BinaryOperator::Add:
				addToSolver(res == op0 + op1);
				break;
			case BinaryOperator::Sub:
				addToSolver(res == op0 - op1);
				break;
			case BinaryOperator::Mul:
				addToSolver(res == op0 * op1);
				break;
			case BinaryOperator::SDiv:
				addToSolver(res == op0 / op1);
				break;
			case BinaryOperator::SRem:
				addToSolver(res == op0 % op1);
				break;
			case BinaryOperator::Xor:
				addToSolver(res == bv2int(int2bv(32, op0) ^ int2bv(32, op1), 1));
				break;
			case BinaryOperator::And:
				addToSolver(res == bv2int(int2bv(32, op0) & int2bv(32, op1), 1));
				break;
			case BinaryOperator::Or:
				addToSolver(res == bv2int(int2bv(32, op0) | int2bv(32, op1), 1));
				break;
			case BinaryOperator::AShr:
				addToSolver(res == bv2int(ashr(int2bv(32, op0), int2bv(32, op1)), 1));
				break;
			case BinaryOperator::Shl:
				addToSolver(res == bv2int(shl(int2bv(32, op0), int2bv(32, op1)), 1));
				break;
			default:
				assert(false && "implement this part");
			}
		}
		else if (const BranchStmt *br = SVFUtil::dyn_cast<BranchStmt>(stmt))
		{
			DBOP(std::cout << "\t skip handled when traversal Conditional IntraCFGEdge \n");
		}
		else if (const SelectStmt *select = SVFUtil::dyn_cast<SelectStmt>(stmt)) {
			expr res = getZ3Expr(select->getResID());
			expr tval = getZ3Expr(select->getTrueValue()->getId());
			expr fval = getZ3Expr(select->getFalseValue()->getId());
			expr cond = getZ3Expr(select->getCondition()->getId());
			addToSolver(res == ite(cond == getCtx().int_val(1), tval, fval));
		}
		else if (const PhiStmt *phi = SVFUtil::dyn_cast<PhiStmt>(stmt)) {
			expr res = getZ3Expr(phi->getResID());
			bool opINodeFound = false;
			for(u32_t i = 0; i < phi->getOpVarNum(); i++){
				assert(srcNode && "we don't have a predecessor ICFGNode?");
				if (srcNode->getFun()->postDominate(srcNode->getBB(),phi->getOpICFGNode(i)->getBB()))
				{
					expr ope = getZ3Expr(phi->getOpVar(i)->getId());
					addToSolver(res == ope);
					opINodeFound = true;
				}
			}
			assert(opINodeFound && "predecessor ICFGNode of this PhiStmt not found?");
		}
	}

	return true;
}

/// Traverse each program path
bool SSE::translatePath(std::vector<const ICFGEdge*>& path) {
	for (const ICFGEdge* edge : path) {
		if (const IntraCFGEdge* intraEdge = SVFUtil::dyn_cast<IntraCFGEdge>(edge)) {
			if (handleIntra(intraEdge) == false)
				return false;
		}
		else if (const CallCFGEdge* call = SVFUtil::dyn_cast<CallCFGEdge>(edge)) {
			handleCall(call);
		}
		else if (const RetCFGEdge* ret = SVFUtil::dyn_cast<RetCFGEdge>(edge)) {
			handleRet(ret);
		}
		else
			assert(false && "what other edges we have?");
	}

	return true;
}

/// Program entry
void SSE::analyse() {
	for (const ICFGNode* src : identifySources()) {
		assert(SVFUtil::isa<GlobalICFGNode>(src) && "reachability should start with GlobalICFGNode!");
		for (const ICFGNode* sink : identifySinks()) {
			const IntraCFGEdge startEdge(nullptr, const_cast<ICFGNode*>(src));
			/// start traversing from the entry to each assertion and translate each path
			reachability(&startEdge, sink);
			resetSolver();
		}
	}
}
