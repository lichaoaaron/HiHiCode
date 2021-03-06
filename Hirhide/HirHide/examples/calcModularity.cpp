#include <cstdio>
#include <iostream>
#include "../Graph.h"

#include "../os.h"

using std::cout;
using std::endl;

map<string, string> Graph::config;



void calc(Communities & truth, vector<Communities> & cs, vector<string> & cs_name, FILE * fp)
{

	for (size_t i = 0; i < cs.size(); ++i)
	{
		printf("process detected %u\n", i);
		Communities & detected = cs[i];

		double nmi = detected.calcNMI(truth);

		vector<int> v_index;
		vector<double> v_value;
		string outdir = "f1/";

		double p = Communities::Precision(detected, truth, v_index, v_value, outdir).first;
		double r = Communities::Recall(detected, truth, v_index, v_value, outdir).first;
		double f1 = Communities::PR(detected, truth, outdir).first;


		double jp = Communities::JaccardPrecision(detected, truth, v_index, v_value, outdir).first;
		double jr = Communities::JaccardRecall(detected, truth, v_index, v_value, outdir).first;
		double jf1 = Communities::JaccardF1Score(detected, truth, outdir).first;



		fprintf(fp, "%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", cs_name[i].c_str(),nmi, f1, p, r, jf1, jp, jr);

	}

}

int main(int argc, char *argv[])
{


	string graph_path;
	if (argc >= 2)
		graph_path = argv[1];
	else
		graph_path = "F:/Yun/Bio/fenceng/20180103/newCommunity/";
	//F:\HICODE_SUB\syn\3000_ori

	Graph g;
	g.load(graph_path + "graph/graph");

	FILE * fp = fopen((graph_path + "detected_files.txt").c_str(), "r");
	if (!fp)
		printf("fopen detected_files.txt failed");
	vector<string> detected_path;
	vector<Communities> detected;
	char buff[1024];
	while (!feof(fp))
	{
		fgets(buff, 1000, fp);
		string sfn = buff;
		if (sfn.size() > 1 && sfn[sfn.size() - 1] == '\n') sfn = sfn.substr(0, sfn.size() - 1);
		detected_path.push_back(sfn);

		Communities cs;
		cs.load(graph_path + sfn);
		//cs.print();
		detected.push_back(cs);
	}
	fclose(fp);

	fp = fopen((graph_path + "truth_files.txt").c_str(), "r");
	if (!fp)
		printf("fopen truth_files.txt failed");
	vector<string> truth_path;
	vector<Communities> truth;
	while (!feof(fp))
	{
		fgets(buff, 1000, fp);
		string sfn = buff;
		if (sfn.size() > 1 && sfn[sfn.size() - 1] == '\n') sfn = sfn.substr(0, sfn.size() - 1);
		truth_path.push_back(sfn);

		Communities cs;
		cs.load(graph_path + sfn);

		//cs.print();
		truth.push_back(cs);
	}
	fclose(fp);

	//showVector(truth_path);



	string outdir = "modularity/";
	os::mkdir(outdir);
	string outdir2 = "f1/";
	os::mkdir(outdir2);

	fp = fopen((outdir + "truth_modularity.csv").c_str(), "w");
	fprintf(fp, "truth, modularity\n");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, "%s,", tpath.c_str());

		double m = g.calcModularity(truth[truth_i]);
		fprintf(fp, "%lf\n", m);
	}

	fclose(fp);

	fp = fopen((outdir + "detected_modularity.csv").c_str(), "w");
	fprintf(fp, "detected, modularity\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		
		fprintf(fp, "%s,", detected_path[i].c_str());
		double m = g.calcModularity(detected[i]);
		fprintf(fp, "%lf\n", m);	


	}
	fclose(fp);

	os::moveDir(outdir, graph_path);
	return 0;



	fp = fopen((outdir + "f1_Truth_Unweighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::f1(truth[truth_i], detected[i]);
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);


	fp = fopen((outdir + "f1_Detected_weighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::wf1(detected[i], truth[truth_i]);
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);


	fp = fopen((outdir + "f1_Detected_Unweighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::f1(detected[i], truth[truth_i]);
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);


	vector<int> tmp;
	vector<double> tmpd;

	fp = fopen((outdir + "Precision_Truth_weighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::Precision( truth[truth_i], detected[i], tmp, tmpd, outdir2).first;
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);


	fp = fopen((outdir + "Precision_Truth_Unweighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::Precision( truth[truth_i], detected[i],tmp, tmpd, outdir2).second;
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);


	fp = fopen((outdir + "Precision_Detected_weighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::Precision(detected[i], truth[truth_i], tmp, tmpd, outdir2).first;
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);


	fp = fopen((outdir + "Precision_Detected_Unweighted.csv").c_str(), "w");
	for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
	{


		string tpath = truth_path[truth_i];
		for (int i = 0; i < tpath.size(); ++i)
		{
			if (tpath[i] == '\\' || tpath[i] == '/')
				tpath[i] = '_';
		}
		fprintf(fp, ",%s", tpath.c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i < detected.size(); ++i)
	{
		printf("process detected %d..\n", i);
		fprintf(fp, "%s", detected_path[i].c_str());
		for (int truth_i = 0; truth_i < truth.size(); ++truth_i)
		{
			printf("process truth %d..\n", truth_i);



			//double pr = Communities::PR(detected[i], truth[truth_i], outdir).first;
			double wf1 = Communities::Precision(detected[i], truth[truth_i], tmp, tmpd, outdir2).second;
			fprintf(fp, ",%lf", wf1);
		}
		fprintf(fp, "\n");


	}
	fclose(fp);

	

	os::moveDir(outdir, graph_path);

	printf("------------\ndone\n");
	return 0;
}