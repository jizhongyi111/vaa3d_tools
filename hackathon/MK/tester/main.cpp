#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iterator>
#include <map>
#include <string>

#include <boost\filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "SWCtester.h"
#include "ImgManager.h"
#include "ImgProcessor.h"

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	/********* specify function *********/
	//const char* funcNameC = argv[1];
	//string funcName(funcNameC);
	string funcName = "interAccuracy";
	/************************************/

	if (!funcName.compare("2DblobMerge"))
	{
		//const char* inputSWCnameC = argv[2];
		//string inputSWCname(inputSWCnameC);
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory_442_swcLumps_2Dlabel\\478293723.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);

		SWCtester mySWCtester;
		vector<connectedComponent> outputConnCompList = mySWCtester.connComponent2DmergeTest(inputSWCnameQ);
	}
	else if (!funcName.compare("swcID"))
	{
		string refSWCname = "H:\\IVSCC_mouse_inhibitory_442_swcROIcropped\\319215569.swc";
		string subjSWCname = "H:\\IVSCC_mouse_inhibitory_442_swcROIcropped_centroids3D\\319215569.swc";
		QString refSWCnameQ = QString::fromStdString(refSWCname);
		QString subjSWCnameQ = QString::fromStdString(subjSWCname);
		NeuronTree refTree = readSWC_file(refSWCnameQ);
		NeuronTree subjTree = readSWC_file(subjSWCnameQ);
		NeuronTree outputTree = NeuronStructUtil::swcIdentityCompare(subjTree, refTree, 50, 20);
		QString outputSWCname = "H:\\testOutput\\test.swc";
		writeSWC_file(outputSWCname, outputTree);
	}
	else if (!funcName.compare("cleanUpZ"))
	{
		string inputSWCname = "H:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D_diffTree\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);
		NeuronTree outputTree = NeuronStructUtil::swcZclenUP(inputTree);
		QString outputSWCname = "H:\\IVSCC_mouse_inhibitory\\testOutput\\test.swc";
		writeSWC_file(outputSWCname, outputTree);
	}
	else if (!funcName.compare("morphCheck"))
	{
		NeuronStructExplorer mySWCanalyzer;
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D_zCleaned_MSTcut_zRatio\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);
		profiledTree currTree(inputTree);
		/*vector<segUnit> segs = mySWCanalyzer.MSTtreeTrim(currTree.segs);
		NeuronTree outputTree;
		for (vector<segUnit>::iterator it = segs.begin(); it != segs.end(); ++it)
		{
			for (QList<NeuronSWC>::iterator nodeIt = it->nodes.begin(); nodeIt != it->nodes.end(); ++nodeIt)
				outputTree.listNeuron.push_back(*nodeIt);
		}
		QString outputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput\\testMSTzCutMorph.swc";
		writeSWC_file(outputSWCname, outputTree);*/
	}
	else if (!funcName.compare("profiledTreeTest"))
	{
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D_zCleaned_MSTcut\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);
		
		profiledTree testTree(inputTree);
		NeuronTree outputTree;
		/*for (vector<segUnit>::iterator it = testTree.segs.begin(); it != testTree.segs.end(); ++it)
		{
			for (QList<NeuronSWC>::iterator nodeIt = it->nodes.begin(); nodeIt != it->nodes.end(); ++nodeIt)
				outputTree.listNeuron.push_back(*nodeIt);
		}
		
		QString outputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput2\\test2.swc";
		writeSWC_file(outputSWCname, outputTree);*/
	}
	else if (!funcName.compare("MSTtest"))
	{
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D_diffTree_zCleaned\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);

		NeuronStructExplorer mySWCanalyzer;
		NeuronTree outputTree = mySWCanalyzer.SWC2MSTtree(inputTree);
		QString outputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput2\\testMSTz.swc";
		writeSWC_file(outputSWCname, outputTree);
	}
	else if (!funcName.compare("MSTcut"))
	{
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput2\\testMSTz.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);

		NeuronTree outputTree = NeuronStructExplorer::MSTtreeCut(inputTree, 20);
		QString outputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput2\\testMSTzCut.swc";
		writeSWC_file(outputSWCname, outputTree);
	}
	else if (!funcName.compare("branchBreak"))
	{
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\MST2nd\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);
		profiledTree inputProfiledTree(inputTree);
		NeuronTree outputTree = NeuronStructExplorer::MSTbranchBreak(inputProfiledTree, true);
		QString outputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput\\test.swc";
		writeSWC_file(outputSWCname, outputTree);
	}
	else if (!funcName.compare("segElongate"))
	{
		string inputSWCname = "H:\\IVSCC_mouse_inhibitory\\testInput\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);
		profiledTree inputProfiledTree(inputTree);

		NeuronStructExplorer mySWCExplorer;
		profiledTree elongatedTree = mySWCExplorer.itered_segElongate(inputProfiledTree);

		QString outputSWCname = "H:\\IVSCC_mouse_inhibitory\\testOutput\\test.swc";
		writeSWC_file(outputSWCname, elongatedTree.tree);
	}
	else if (!funcName.compare("dotRemove"))
	{
		string inputSWCname = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\MST2nd_branchBreak_noSpike_elong\\319215569.swc";
		QString inputSWCnameQ = QString::fromStdString(inputSWCname);
		NeuronTree inputTree = readSWC_file(inputSWCnameQ);
		profiledTree inputProfiledTree(inputTree);

		NeuronStructExplorer mySWCExplorer;
		NeuronTree outputTree = mySWCExplorer.singleDotRemove(inputProfiledTree);

		QString outputSWCname = "Z:\\IVSCC_mouse_inhibitory\\testOutput\\test.swc";
		writeSWC_file(outputSWCname, outputTree);
	}
	else if (!funcName.compare("treeUnion"))
	{
		string inputSWCname1 = "H:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\MST2nd_tiled30_branchBreak_noSpike_elong_noDots\\319215569.swc";
		string inputSWCname2 = "H:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\diffTree_zCleaned_MST_zRatio_branchBreak_noSpike_elong\\319215569.swc";
		QString inputSWCname1Q = QString::fromStdString(inputSWCname1);
		QString inputSWCname2Q = QString::fromStdString(inputSWCname2);
		NeuronTree inputTree1 = readSWC_file(inputSWCname1Q);
		NeuronTree inputTree2 = readSWC_file(inputSWCname2Q);
		profiledTree inputProfiledTree1(inputTree1);
		profiledTree inputProfiledTree2(inputTree2);

		NeuronStructExplorer mySWCExplorer;
		profiledTree outputProfiledTree = mySWCExplorer.treeUnion_MSTbased(inputProfiledTree1, inputProfiledTree2);

		QString outputSWCname = "H:\\IVSCC_mouse_inhibitory\\testOutput\\test.swc";
		writeSWC_file(outputSWCname, outputProfiledTree.tree);
	}
	else if (!funcName.compare("selfDist"))
	{
		ofstream outputTest("Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\test.txt");

		string inputSWCname1 = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\diffTree_zCleaned_MST_zRatio_branchBreak_noSpike_elong_noDotSeg2_tile30elong30_tile60elong30_RESULT\\319215569.swc";
		string inputSWCname2 = "Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\442_swcROIcropped\\319215569.swc";
		QString inputSWCnameQ1 = QString::fromStdString(inputSWCname1);
		QString inputSWCnameQ2 = QString::fromStdString(inputSWCname2);
		NeuronTree inputTree1 = readSWC_file(inputSWCnameQ1);
		NeuronTree inputTree2 = readSWC_file(inputSWCnameQ2);
		map<string, float> swcStats = NeuronStructUtil::selfNodeDist(inputTree1.listNeuron);
		
		outputTest << swcStats["mean"] << " " << swcStats["std"] << endl;
	}
	else if (!funcName.compare("accuracy"))
	{
		const char* inputPathNameC = argv[1];
		string inputPathName(inputPathNameC);
		ofstream outputFile("Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\accuracy.txt");
		outputFile << "case num\t" << "rate\t" << "node num" << endl;

		for (filesystem::directory_iterator swcIt(inputPathName); swcIt != filesystem::directory_iterator(); ++swcIt)
		{
			string swcName = swcIt->path().filename().string();
			string swcFullName = inputPathName + "\\" + swcName;
			QString swcFullNameQ = QString::fromStdString(swcFullName);
			NeuronTree nt = readSWC_file(swcFullNameQ);

			int TP = 0;
			int FP = 0;
			for (QList<NeuronSWC>::iterator it = nt.listNeuron.begin(); it != nt.listNeuron.end(); ++it)
			{
				if (it->type == 2) ++TP;
				else if (it->type == 3) ++FP;
			}
			double rate = double(TP) / double(TP + FP);

			string caseNum = swcName.substr(0, 9);
			outputFile << caseNum << "\t" << rate << "\t" << nt.listNeuron.size() << endl;
		}
	}
	else if (!funcName.compare("interAccuracy"))
	{
		const char* inputPathNameC = argv[1];
		string inputPathName(inputPathNameC);
		ofstream outputFile("Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\interAccuracy.txt");
		outputFile << "case num\t" << "avgDist\t" << "structure diff" << endl;

		for (filesystem::directory_iterator swcIt(inputPathName); swcIt != filesystem::directory_iterator(); ++swcIt)
		{
			string csvName = swcIt->path().filename().string();
			string csvFullName = inputPathName + "\\" + csvName;
			//cout << csvFullName << endl;
			ifstream inputCSV(csvFullName);
			string inputLine;
			int count = 0;
			while (getline(inputCSV, inputLine))
			{
				if (inputLine.find("from neuron 1 to 2") != string::npos)
				{
					++count;
					vector<string> inputs;
					boost::split(inputs, inputLine, boost::is_any_of(" "));
					//for (vector<string>::iterator checkit = inputs.begin(); checkit != inputs.end(); ++checkit) cout << *checkit << " ";
					//cout << inputs.size() << " ";
					float measure = stof(inputs.back());
					string caseName = csvName.substr(0, 9);
					
					if (count == 1) outputFile << caseName << "\t" << measure << "\t";
					else if (count == 2) outputFile << measure << endl;
				}
			}
			inputLine.clear();
		}
	}
	else if (!funcName.compare("makeLinkerFile"))
	{
		const char* inputPathNameC = argv[1];
		string inputPathName(inputPathNameC);
		//ofstream outputFile("Z:\\IVSCC_mouse_inhibitory\\442_swcROIcropped_centroids2D\\interAccuracy.txt");
		//outputFile << "case num\t" << "avgDist\t" << "structure diff" << endl;

		for (filesystem::directory_iterator swcIt(inputPathName); swcIt != filesystem::directory_iterator(); ++swcIt)
		{
			string swcName = swcIt->path().filename().string();
			string swcFullName = inputPathName + "\\" + swcName;
			NeuronStructUtil::linkerFileGen_forSWC(swcFullName);
		}
	}

	return 0;
}


