//===- Z3Mgr.cpp -- Z3 manager for symbolic execution ------------------//
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
 * Z3 manager for symbolic execution
 *
 * Created on: Feb 19, 2024
 */

#include "Z3SSEMgr.h"
#include "SVF-LLVM/LLVMUtil.h"
#include "Graphs/PAG.h"
#include <iomanip>
#include <set>

using namespace SVF;
using namespace SVFUtil;
using namespace llvm;
using namespace z3;

Z3SSEMgr::Z3SSEMgr(PAG* ir)
: Z3Mgr(ir->getSVFVarNum() * 10)
, svfir(ir) {
}


/*
 * Object must be either a constaint data or a location value (address-taken variable)
 * Constant data includes ConstantInt, ConstantFP, ConstantPointerNull and ConstantAggregate
 * Locations includes pointers to globals, heaps, stacks
 */
z3::expr Z3SSEMgr::createExprForObjVar(const ObjVar* objVar) {
	std::string str;
	raw_string_ostream rawstr(str);
	expr e(ctx);
	const BaseObjVar* obj = svfir->getBaseObject(objVar->getId());
	/// constant data
	if (obj->isConstDataOrAggData() || obj->isConstantArray() || obj->isConstantStruct()) {
		if (const ConstIntObjVar* consInt = SVFUtil::dyn_cast<ConstIntObjVar>(objVar)) {
			e = ctx.int_val((s32_t)consInt->getSExtValue());
		}
		else if (const ConstFPObjVar* consFp = SVFUtil::dyn_cast<ConstFPObjVar>(objVar)) {
			e = ctx.int_val(static_cast<u32_t>(consFp->getFPValue()));
		}
		else if (SVFUtil::isa<GlobalObjVar>(objVar)) {
			e = ctx.int_val(getVirtualMemAddress(objVar->getId()));
		}
		else if (obj->isConstantArray() || obj->isConstantStruct()) {
			assert(false && "implement this part");
		}
		else {
			std::cerr << obj->toString() << "\n";
			assert(false && "what other types of values we have?");
		}
	}
	/// locations (address-taken variables)
	else {
		e = ctx.int_val(getVirtualMemAddress(objVar->getId()));
	}
	return e;
}

std::string Z3SSEMgr::callingCtxToStr(const CallStack& callingCtx) {
	std::string str;
	std::stringstream rawstr(str);
	rawstr << "ctx:[ ";
	for (const auto &node : callingCtx) {
		rawstr << node->getId() << " ";
	}
	rawstr << "] ";
	return rawstr.str();
}

z3::expr Z3SSEMgr::getZ3Expr(SVF::u32_t idx, const CallStack& callingCtx) {
	u32_t varId = getInternalID(idx);
	assert(varId == idx && "SVFVar idx overflow > 0x7f000000?");
	std::string str;
	std::stringstream rawstr(str);
	const SVFVar *svfVar = svfir->getGNode(varId);
	if (const ObjVar* objVar = SVFUtil::dyn_cast<ObjVar>(svfVar)) {
		return createExprForObjVar(objVar);
	} else {

		// Check if svfVar does not have a value or it has a constant value
		if (!SVFUtil::isa<ConstDataValVar, ConstDataObjVar>(svfVar)) {
			// If there is a non-constant value, add callingCtx to z3 expr
			rawstr << callingCtxToStr(callingCtx);
		} else {
			// If there's no value or it's a constant, we do not add the callingCtx to z3 expr
		}
		rawstr << "ValVar" << varId;
		std::string name = rawstr.str();
		return ctx.int_const(name.c_str());
	}
}

/// Return the address expr of a ObjVar
z3::expr Z3SSEMgr::getMemObjAddress(u32_t idx) {
	NodeID objIdx = getInternalID(idx);
	assert(SVFUtil::isa<ObjVar>(svfir->getGNode(objIdx)) && "Fail to get the MemObj!");
	return createExprForObjVar(SVFUtil::cast<ObjVar>(svfir->getGNode(objIdx)));
}

z3::expr Z3SSEMgr::getGepObjAddress(z3::expr pointer, u32_t offset) {
	NodeID obj = getInternalID(z3Expr2NumValue(pointer));
	assert(SVFUtil::isa<ObjVar>(svfir->getGNode(obj)) && "Fail to get the base object address!");
	NodeID gepObj = svfir->getGepObjVar(obj, offset);
	/// TODO: check whether this node has been created before or not to save creation time
	if (obj == gepObj)
		return createExprForObjVar(SVFUtil::cast<ObjVar>(svfir->getGNode(obj)));
	else
		return createExprForObjVar(SVFUtil::cast<GepObjVar>(svfir->getGNode(gepObj)));
}

s32_t Z3SSEMgr::getGepOffset(const GepStmt* gep, const CallStack& callingCtx) {
	if (gep->getOffsetVarAndGepTypePairVec().empty())
		return gep->getConstantStructFldIdx();

	s32_t totalOffset = 0;
	for (int i = gep->getOffsetVarAndGepTypePairVec().size() - 1; i >= 0; i--) {
		const SVFVar* var = gep->getOffsetVarAndGepTypePairVec()[i].first;
		const SVFType* type = gep->getOffsetVarAndGepTypePairVec()[i].second;
		s32_t offset = 0;
		if (const ConstIntValVar* constInt = SVFUtil::dyn_cast<ConstIntValVar>(var)) {
			offset = constInt->getSExtValue();
		} else {
			offset = z3Expr2NumValue(getZ3Expr(var->getId(), callingCtx));
		}

		if (type == nullptr) {
			totalOffset += offset;
			continue;
		}

		/// Caculate the offset
		if (const SVFPointerType* pty = SVFUtil::dyn_cast<SVFPointerType>(type))
			totalOffset += offset * gep->getAccessPath().getElementNum(gep->getAccessPath().gepSrcPointeeType());
		else
			totalOffset += PAG::getPAG()->getFlattenedElemIdx(type, offset);
	}
	return totalOffset;
}

void Z3SSEMgr::printExprValues(const CallStack& callingCtx) {
	std::cout.flags(std::ios::left);
	std::cout << "\n-----------SVFVar and Value-----------\n";
	std::map<std::string, std::string> printValMap;
	std::map<NodeID, std::string> objKeyMap;
	std::map<NodeID, std::string> valKeyMap;
	for (PAG::iterator nIter = svfir->begin(); nIter != svfir->end(); ++nIter) {
		expr e = getEvalExpr(getZ3Expr(nIter->first, callingCtx));
		if (e.is_numeral()) {
			NodeID varID = nIter->second->getId();
			s32_t value = e.get_numeral_int64();
			std::stringstream exprName;
			std::stringstream valstr;
			if (const ValVar* valVar = SVFUtil::dyn_cast<ValVar>(nIter->second)) {
				exprName << "ValVar" << varID;
				if (isVirtualMemAddress(value))
					valstr << "\t Value: " << std::hex << "0x" << value << "\n";
				else
					valstr << "\t Value: " << std::dec << value << "\n";
				printValMap[exprName.str()] = valstr.str();
				valKeyMap[varID] = exprName.str();
			}
			else {
				exprName << "ObjVar" << varID << std::hex << " (0x" << getVirtualMemAddress(varID) << ") ";
				if (isVirtualMemAddress(value)) {
					expr storedValue = getEvalExpr(loadValue(e));
					if (storedValue.is_numeral()) {
						s32_t contentValue = z3Expr2NumValue(storedValue);
						if (isVirtualMemAddress(contentValue))
							valstr << "\t Value: " << std::hex << "0x" << contentValue << "\n";
						else
							valstr << "\t Value: " << std::dec << contentValue << "\n";
					}
					else
						valstr << "\t Value: NULL"
						       << "\n";
				}
				else
					valstr << "\t Value: NULL"
					       << "\n";
				printValMap[exprName.str()] = valstr.str();
				objKeyMap[varID] = exprName.str();
			}
		}
	}
	for (auto it = objKeyMap.begin(); it != objKeyMap.end(); ++it) {
		std::string printKey = it->second;
		std::cout << std::setw(25) << printKey << printValMap[printKey];
	}
	for (auto it = valKeyMap.begin(); it != valKeyMap.end(); ++it) {
		std::string printKey = it->second;
		std::cout << std::setw(25) << printKey << printValMap[printKey];
	}
	std::cout << "-----------------------------------------\n";
}
