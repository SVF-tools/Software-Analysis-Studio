#include "Assignment-3.h"

#include "Graphs/CallGraph.h"
#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/PAGBuilder.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"

using namespace std;
void checkICFGcase(std::string moduleName, const std::set<std::string>& res, const std::set<std::string>& expected) {
	std::stringstream ss;
	ss << "\n wrong paths generated - " << moduleName << " failed !";
	assert(res.size() == expected.size() && ss.str().c_str());
	for (auto path : res) {
		assert(expected.find(path) != expected.end() && ss.str().c_str());
	}
}

void TestICFG(std::vector<std::string>& moduleNameVec) {
	LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

	/// Build Program Assignment Graph (PAG)
	PAGBuilder builder;
	PAG* pag = builder.build();
	ICFG* icfg = pag->getICFG();
	// If you want to test your own case, please change the dump name
	ICFGTraversal* gt = new ICFGTraversal(pag);
	const fs::path& config = CUR_DIR() / "Tests/SrcSnk.txt";

	gt->readSrcSnkFromFile(config);
	for (const CallICFGNode* src : gt->identifySources()) {
		for (const CallICFGNode* snk : gt->identifySinks()) {
			gt->reachability(src, snk);
		}
	}
	std::string moduleName = moduleNameVec[0].substr(moduleNameVec[0].find_last_of('/') + 1);

	// print gt->getPaths()
	for (auto path : gt->getPaths())
		std::cerr << path << "\n";
	if (moduleName == "test1.ll") {
		std::set<std::string> expected = {"START->3->4->5->END"};
		assert(gt->getPaths() == expected && " \n wrong paths generated - test1 failed !");
		checkICFGcase(moduleName, gt->getPaths(), expected);
	}
	else if (moduleName == "test2.ll") {
		std::set<std::string> expected = {"START->6->7->8->1->5->2->9->10->END"};
		checkICFGcase(moduleName, gt->getPaths(), expected);
	}
	else if (moduleName == "test3.ll") {
		std::set<std::string> expected =
		    {
		        "START->12->13->14->3->8->9->1->7->2->10->11->4->15->16->END",
		    };
		checkICFGcase(moduleName, gt->getPaths(), expected);
	}


	for (auto path : gt->getPaths())
		std::cerr << path << "\n";
	PAG::releasePAG();
	LLVMModuleSet::releaseLLVMModuleSet();
	delete gt;
}

void TestPTA(std::vector<std::string>& moduleNameVec) {
	LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	SVF::PAGBuilder builder;
	SVF::PAG* pag = builder.build();
	AndersenPTA* andersenPTA = new AndersenPTA(pag);
	andersenPTA->analyze();
	delete andersenPTA;
	SVF::LLVMModuleSet::releaseLLVMModuleSet();
	SVF::PAG::releasePAG();
}

void TestTaint(std::vector<std::string>& moduleNameVec) {
	LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	/// Build Program Assignment Graph (PAG)
	SVF::PAGBuilder builder;
	SVF::PAG* pag = builder.build();

	ICFGTraversal* taint = new ICFGTraversal(pag);

	taint->taintChecking();
	std::cerr << "######################Tainted Information Flow (" + std::to_string(taint->getPaths().size())
	                 + " found)######################\n";
	std::cerr << "---------------------------------------------\n";
	for (std::string path: taint->getPaths())  {
		std::string originPath = path;
		const std::string prefix = "START->";
		const std::string suffix = "->END";

		if (path.find(prefix) == 0) {
			path.erase(0, prefix.length());
		}
		if (path.rfind(suffix) == path.length() - suffix.length()) {
			path.erase(path.length() - suffix.length(), suffix.length());
		}
		std::vector<std::string> numbers;
		std::vector<std::string> tokens;
		size_t start = 0, end = 0;
		while ((end = path.find("->", start)) != std::string::npos) {
			tokens.push_back(path.substr(start, end - start));
			start = end + 2;
		}
		tokens.push_back(path.substr(start));
		NodeID srcID = std::stoi(tokens[0]);
		NodeID dstID = std::stoi(tokens.back());
		const ICFGNode* srcNode = PAG::getPAG()->getICFG()->getICFGNode(srcID);
		const ICFGNode* dstNode = PAG::getPAG()->getICFG()->getICFGNode(dstID);
		// print the source and sink node
		std::stringstream ss;
		ss << originPath << std::endl << "Source: " << srcNode->toString() <<
		    "\nSink: " << dstNode->toString()
		   << "\n---------------------------------------------\n";
		SVFUtil::outs() << ss.str() << std::endl;
	}
	if (taint->getPaths().empty()) {
		SVFUtil::outs() << "No tainted information flow found" << std::endl;
	}
	else {

	}
	// print taint->getPaths() 
	for (auto path : taint->getPaths())
		std::cerr << path << "\n";
		
	std::string moduleName = moduleNameVec[0].substr(moduleNameVec[0].find_last_of('/') + 1);
	if (moduleName == "test1.ll") {
		set<string> expected = {"START->6->1->5->2->7->8->9->10->END"};
		assert(taint->getPaths() == expected && " \n wrong paths generated - test1 failed !");
		cout << "\n test1 passed !" << endl;
	}
	SVF::PAG::releasePAG();
	SVF::LLVMModuleSet::releaseLLVMModuleSet();
}

int main(int argc, char** argv) {
	int arg_num = 0;
	int extraArgc = 1;
	char** arg_value = new char*[argc + extraArgc];
	bool ptaEnabled = false;
	bool taintEnabled = false;
	bool icfgEnabled = false;
	int cur_arg = 0;
	for (; arg_num < argc; ++arg_num) {
		if (strcmp(argv[arg_num], "-pta") == 0) {
			ptaEnabled = true;
		}
		else if (strcmp(argv[arg_num], "-taint") == 0) {
			taintEnabled = true;
		}
		else if (strcmp(argv[arg_num], "-icfg") == 0) {
			icfgEnabled = true;
		}
		else {
			arg_value[cur_arg++] = argv[arg_num];
		}
	}
	// only one can be true
	// If no analysis is specified, the default is set to taint analysis (-taint)
	if (ptaEnabled + taintEnabled + icfgEnabled == 0) {
		// default to taint
		taintEnabled = true;
	}
	assert((ptaEnabled + taintEnabled + icfgEnabled) == 1 && "only one analysis can be enabled");

	// You may comment it to see the details of the analysis
	arg_value[cur_arg++] = (char*)"-stat=false";

	std::vector<std::string> moduleNameVec;
	moduleNameVec = OptionBase::parseOptions(cur_arg,
	                                         arg_value,
	                                         "Teaching-Software-Analysis Assignment 1",
	                                         "[options] <input-bitcode...>");
	if (ptaEnabled) {
		TestPTA(moduleNameVec);
	}
	else if (taintEnabled) {
		TestTaint(moduleNameVec);
	}
	else if (icfgEnabled) {
		TestICFG(moduleNameVec);
	}
	delete[] arg_value;
	return 0;
}
