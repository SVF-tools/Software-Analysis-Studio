#include "Assignment-4.h"
#include "SVF-LLVM/LLVMUtil.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"

using namespace SVF;
using namespace SVFUtil;
u32_t SSE::assert_checked = 0;

/*
 // Software-Analysis-Studio Assignment 4 main function entry
 // To run your program with a test case, set "program" to "${workspaceFolder}/bin/ass4"
 // and set "args" to bitcode from Assignment-4/Tests/testcases/sse in .vscode/launch.json.
 // For example: "args": ["${workspaceFolder}/Assignment-4/Tests/testcases/sse/test1.ll"]
 */
int main(int argc, char** argv) {
	int arg_num = 0;
	int extraArgc = 4;
	char** arg_value = new char*[argc + extraArgc];
	for (; arg_num < argc; ++arg_num) {
		arg_value[arg_num] = argv[arg_num];
	}
	std::vector<std::string> moduleNameVec;

	int orgArgNum = arg_num;
	arg_value[arg_num++] = (char*)"-model-arrays=true";
	arg_value[arg_num++] = (char*)"-pre-field-sensitive=false";
	arg_value[arg_num++] = (char*)"-model-consts=true";
	arg_value[arg_num++] = (char*)"-stat=false";
	assert(arg_num == (orgArgNum + extraArgc) && "more extra arguments? Change the value of extraArgc");

	moduleNameVec = OptionBase::parseOptions(arg_num,
	                                         arg_value,
	                                         "Software-Analysis-Studio Assignment 4",
	                                         "[options] <input-bitcode...>");

	LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(".svf");

	SVFIRBuilder builder;
	SVFIR* svfir = builder.build();

	CallGraph* callgraph = AndersenWaveDiff::createAndersenWaveDiff(svfir)->getCallGraph();
	builder.updateCallGraph(callgraph);

	/// ICFG
	ICFG* icfg = svfir->getICFG();
	icfg->updateCallGraph(callgraph);
	icfg->dump(moduleNameVec[0] + ".icfg");

	SSE* sse = new SSE(svfir, icfg);
	sse->addToSolver(sse->getZ3Expr(0) == 0);
	sse->analyse();

	SVF::LLVMModuleSet::releaseLLVMModuleSet();
	SVF::SVFIR::releaseSVFIR();

	delete[] arg_value;
	delete sse;
	if (SSE::assert_checked > 0) {
		return 0;
	}
	else {
		std::cerr << "No assertion was checked!" << std::endl;
		return 1;
	}
}
