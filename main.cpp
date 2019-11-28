// -- STL -- //
#include <iostream>
#include <iterator>
#include <chrono>  // for high_resolution_clock

// -- OpenCV -- //
#include <opencv2/core/persistence.hpp>

// -- Project -- //
#include "graph.h"

using namespace std;
using namespace cv;

cv::Mat readFromFile(std::string _file)
{
	cv::Mat matrix;
	cv::FileStorage fs(_file, FileStorage::READ);
	fs["data"] >> matrix;
	return matrix;
}

cv::Mat generateCase1()
{
	cv::Mat matrix(6, 6, CV_32FC1);
	matrix.setTo(0.0f);
	matrix.at<float>(0, 1) = 0.8f;
	matrix.at<float>(1, 2) = 0.1f;
	matrix.at<float>(1, 4) = 0.9f;
	matrix.at<float>(1, 3) = 0.3f;
	matrix.at<float>(2, 5) = 0.5f;
	matrix.at<float>(4, 5) = 0.4f;
	return matrix;
}
cv::Mat generateCase2()
{
	cv::Mat matrix(5, 5, CV_32FC1);
	matrix.setTo(0.0f);
	matrix.at<float>(0, 2) = 0.5f;
	matrix.at<float>(1, 2) = 0.5f;
	matrix.at<float>(2, 3) = 0.5f;
	matrix.at<float>(2, 4) = 0.5f;	
	return matrix;
}
cv::Mat generateCase3()
{
	cv::Mat matrix(6, 6, CV_32FC1);
	matrix.setTo(0.0f);
	matrix.at<float>(0, 3) = 0.65f;
	matrix.at<float>(0, 4) = 0.39f;
	matrix.at<float>(0, 5) = 0.48f;

	matrix.at<float>(1, 2) = 0.77f;
	matrix.at<float>(1, 3) = 0.48f;
	matrix.at<float>(1, 4) = 0.66f;
	matrix.at<float>(1, 5) = 0.31f;

	matrix.at<float>(2, 3) = 0.48f;
	matrix.at<float>(2, 4) = 0.74f;
	matrix.at<float>(2, 5) = 0.36f;

	matrix.at<float>(3, 4) = 0.10f;
	matrix.at<float>(3, 5) = 0.89f;	
	
	return matrix;
}
cv::Mat generateCase4()
{
	cv::Mat matrix(7, 7, CV_32FC1);
	matrix.setTo(0.0f);
	matrix.at<float>(0, 2) = 0.9f;	
	matrix.at<float>(1, 3) = 0.8f;	
	matrix.at<float>(2, 4) = 0.3f;
	matrix.at<float>(2, 5) = 0.1f;
	matrix.at<float>(2, 6) = 0.1f;
	matrix.at<float>(3, 4) = 0.3f;
	matrix.at<float>(3, 5) = 0.1f;
	matrix.at<float>(3, 6) = 0.1f;
	matrix.at<float>(4, 6) = 0.5f;
	matrix.at<float>(4, 5) = 0.4f;

	return matrix;
}

void writeCypher(const cv::Mat& _mat, std::string _fileName)
{
	/* example: writing a Graph in Cypher
	MERGE (a:Loc {name:'A'})
	MERGE (b:Loc {name:'B'})
	MERGE (c:Loc {name:'C'})
	MERGE (d:Loc {name:'D'})
	MERGE (e:Loc {name:'E'})
	MERGE (f:Loc {name:'F'})

	MERGE (a)-[:ROAD {cost:50}]->(b)
	MERGE (a)-[:ROAD {cost:50}]->(c)
	MERGE (a)-[:ROAD {cost:100}]->(d)
	MERGE (b)-[:ROAD {cost:40}]->(d)
	MERGE (c)-[:ROAD {cost:40}]->(d)
	MERGE (c)-[:ROAD {cost:80}]->(e)
	MERGE (d)-[:ROAD {cost:30}]->(e)
	MERGE (d)-[:ROAD {cost:80}]->(f)
	MERGE (e)-[:ROAD {cost:40}]->(f);
	*/
	std::ofstream myfile(_fileName);
	if(myfile.is_open())
	{
		for (int row = 0; row < _mat.rows; ++row)
		{
			myfile << "MERGE (id" << row << ":Node {name:'" << row << "'})" << endl;
		}

		for (int row = 0; row < _mat.rows; ++row)
		{
			for (int col = 0; col < _mat.cols; ++col)
			{
				float val = _mat.at<float>(row, col);
				if (val > 0.0f)
				{
					myfile << "MERGE (id" << row << ")-[:Link {cost:" << 1.0 - val << "}]->(id" << col << ")" << endl;
				}
			}			
		}
	}
	myfile.close();
}

int main()
{		
	//cv::Mat matrix = readFromFile("data.yml");	
	//cv::Mat matrix = generateCase1();
	//cv::Mat matrix = generateCase2();
	//cv::Mat matrix = generateCase3();
	cv::Mat matrix = generateCase4();

	// Export to Cypher, so the graph can be visualized using Neo4j
	writeCypher(matrix, "cypher_data.txt");
	
	// Run Graph
	bool verbose = false;
	std::cout << "Input Graph: " << matrix.rows << " x " << matrix.cols << " nodes." << std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	Graph graph(matrix, verbose);
	auto finish = std::chrono::high_resolution_clock::now();

	std::vector<Graph::Path> paths = graph.getBestPaths();
	std::cout << "Found " << paths.size() << " paths: " << std::endl;
	for(auto p : paths)
		p.printf();

	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	
	// Finish
	cout << "Press Enter to Finish...";
	cin.ignore();
    return 0;
}