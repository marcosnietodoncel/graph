/*
* Graph - resolution
*
* GNU General Publick License v3.0 (https://www.gnu.org/licenses/gpl-3.0.html)
*
* Graph is a simple library for solving a Path Finding problem on non-cyclic directed Graphs,
* being the output a list of Paths connecting Nodes.
* The algorithm, unlike other Shortest Path algorithms, doesn't compute the sum of costs
* between nodes of the Path, but computes the average. As a result, this algorithm is more
* suited for problems where interconnected nodes represent the same object or entity, and where
* the edge between two nodes represents the likelihood these two nodes correspond to the same entity.
*
* The algorithm, called ENBP (End-Node Back Propagation) is documented and to be published in paper
* form. This note will be uptated should the paper be accepted for publication.
*
* As Graph depends on other libraries, the user must adhere to and keep in place any
* licencing terms of those libraries:
*
* * OpenCV v3.0.0 (or any other version) (http://opencv.org)
*
* License Agreement for OpenCV
* ------------------------------------------------------------------------
* BSD 2-Clause License (http://opensource.org/licenses/bsd-license.php)
* The dependence of the "Non-free" module of OpenCV is excluded from VCD.
*
*/

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

void Graph::computePaths(const cv::Mat& _weights, std::vector<Graph::Path>& _paths)
{
	if (verbose)
		std::cout << "Computing Paths from Graph(" << _weights.rows << " nodes):" << std::endl;
	Mat rows, cols;
	// Compute sums of matrix in cols and rows
	reduce(_weights, rows, 1, CV_REDUCE_SUM, CV_32F);
	reduce(_weights, cols, 0, CV_REDUCE_SUM, CV_32F);

	// Find end-nodes with incoming edge
	vector<int> finalNodes;
	if(verbose)
		cout << "Final nodes: " << endl;
	for (int i = 0; i<numNodes; ++i)
	{
		if (rows.at<float>(i, 0) == 0.0f && cols.at<float>(0, i) != 0.0f) // rows==0 so there is no departing edge, and cols!=0 so there are incoming edges
		{
			finalNodes.push_back(i);
			if (verbose)
				cout << "\tNode " << i << endl;
		}
	}

	// Iterate and create Paths
	for (size_t i = 0; i<finalNodes.size(); ++i)
	{
		Path path;
		iteration(path, finalNodes[i], _paths, _weights, cols);
	}

	// Reverse paths
	for (size_t p = 0; p<_paths.size(); ++p)
	{
		_paths[p].reversePath();
		if (verbose)
		{
			std::cout << "Path[" << p << "]";
			_paths[p].printf();
		}
	}
}
Graph::Graph(const cv::Mat& _weights, bool _verbose):verbose(_verbose)
{
	// Weights or likelihood should be a squared matrix
	assert(!_weights.empty());
	assert(_weights.cols == _weights.rows);
	assert(_weights.type() == CV_32F);

	// Number of nodes (Nodes here represent each element of the graph)
	numNodes = _weights.cols;

	// Compute paths using the weights matrix
	computePaths(_weights, paths);

	// ----------------------
	// GREEDY ALGORITHM
	// ----------------------
	// Determine best Path, remove it and recompute Graph
	int bestPathId = -1;
	float bestWeight = 0.0f;
	for (int i = 0; i<(int)paths.size(); ++i)
	{
		float val = paths[i].getWeight();
		if (val > bestWeight)
		{
			bestWeight = val;
			bestPathId = i;
		}
	}

	if (paths.size() > 0)
	{
		if (verbose)
			std::cout << "Best path: " << bestPathId << std::endl;
		bestPaths.push_back(paths[bestPathId]);

		// Find other best paths
		vector<Path> currentPaths = paths;
		int numNodesCurrent = numNodes;
		int currentBestPathId = bestPathId;
		Mat weightsCurrent = _weights;
		while (true)
		{
			// Recompute matrix and create new Graph
			Mat weightsRe;
			Mat colsRe, rowsRe;

			int numNodesBestPath = currentPaths[currentBestPathId].getNumNodes();
			if (numNodesBestPath >= numNodesCurrent)
			{
				// No more nodes!
				break;
			}
			else
			{
				// More nodes, select next best path
				weightsCurrent.copyTo(weightsRe);

				// Compose the weightsRe matrix
				vector<int> nodesBestPath = currentPaths[currentBestPathId].getNodes();
				if (verbose)
					std::cout << "Removing nodes: ";
				for (size_t n = 0; n<nodesBestPath.size(); ++n)
				{
					if (verbose)
						std::cout << nodesBestPath[n] << " ";
					weightsRe.row(nodesBestPath[n]).setTo(0.0f);
					weightsRe.col(nodesBestPath[n]).setTo(0.0f);
				}
				if (verbose)
					std::cout << std::endl;

				vector<Path> remainingPaths;
				/*	for( size_t i=0; i<currentPaths.size(); ++i )
				if( i != currentBestPathId )
				remainingPaths.push_back( currentPaths[i] );
				*/

				computePaths(weightsRe, remainingPaths);
				if (remainingPaths.size() > 0)
				{
					// Determine best Path, remove it and recompute Graph
					int bestPathId = -1;
					float bestWeight = 0.0f;
					for (int i = 0; i<(int)remainingPaths.size(); ++i)
					{
						float val = remainingPaths[i].getWeight();
						if (val > bestWeight)
						{
							bestWeight = val;
							bestPathId = i;
						}
					}
					bestPaths.push_back(remainingPaths[bestPathId]);

					// Iterate
					currentPaths = remainingPaths;
					numNodesCurrent -= numNodesBestPath;
					currentBestPathId = bestPathId;
					weightsCurrent = weightsRe;
				}
				else
					break;
			}
		}
	}

	// ----------------------
	// Add isolated Nodes as (best) Paths
	// ----------------------
	for (int n = 0; n<numNodes; ++n)
	{
		// Check if this node can be found in any of the best paths
		bool found = false;
		for (size_t bp = 0; bp<bestPaths.size(); ++bp)
		{
			vector<int> nodes = bestPaths[bp].getNodes();
			if (find(nodes.begin(), nodes.end(), n) != nodes.end())
			{
				found = true;
				break;
			}
		}
		if (found)
			continue;
		else
		{
			Path singleNodePath;
			singleNodePath.addNode(n);

			bestPaths.push_back(singleNodePath);
		}
	}

	// Printf best paths
	if (_verbose)
	{
		std::cout << "Best paths: " << std::endl;
		for (size_t i = 0; i<bestPaths.size(); ++i)
		{
			std::cout << "[" << i << "] - ";
			bestPaths[i].printf();
		}
	}
}
void Graph::iteration(Graph::Path _currentPath, int _currentNode, std::vector<Graph::Path>& _pathVector, const cv::Mat& _weights, const cv::Mat& _cols)
{
	// Copy of current Path for have a static base path not modified in recursive iterations
	Path currentPathCopy = _currentPath;

	// Count number of backconnections
	vector<int> backConnections;
	for (int j = _currentNode; j >= 0; --j)
		if (_weights.at<float>(j, _currentNode) > 0.0f)
			backConnections.push_back(j);

	// Create a path for each backconnection
	for (size_t j = 0; j<backConnections.size(); ++j)
	{
		if (j == 0)
		{
			// The first connection updates the path, doesn't create a new one
			_currentPath.addEdge(_currentNode, backConnections[j], _weights.at<float>(backConnections[j], _currentNode));
			if (_cols.at<float>(0, backConnections[j]) == 0)
			{
				// If start node, insert into path
				_pathVector.push_back(_currentPath);
			}
			else
			{
				// Recursive
				iteration(_currentPath, backConnections[j], _pathVector, _weights, _cols);
			}

		}
		else
		{
			// Add a new Path
			Path path = currentPathCopy;	// use currentPath instead of _currentPath because it might have been modified in the previous iteration
			path.addEdge(_currentNode, backConnections[j], _weights.at<float>(backConnections[j], _currentNode));

			if (_cols.at<float>(0, backConnections[j]) == 0)
			{
				// If end node, insert into path
				_pathVector.push_back(path);
			}
			else
			{
				// Recursive
				iteration(path, backConnections[j], _pathVector, _weights, _cols);
			}
		}
	}
}
void Graph::Path::addNode(int id)
{
	vector<int>::iterator it = find(nodes.begin(), nodes.end(), id);
	if (it == nodes.end())
	{
		// not found: add
		nodes.push_back(id);
	}
}
void Graph::Path::reversePath()
{
	std::reverse(nodes.begin(), nodes.end());
	std::reverse(weights.begin(), weights.end());
}
void Graph::Path::addEdge(int id1, int id2, float w12)
{
	addNode(id1);
	addNode(id2);
	addWeight(w12);
}
float Graph::Path::getWeight() const
{
	float sum = 0;
	for (size_t i = 0; i<weights.size(); ++i)
		sum += weights[i];
	if (weights.size() > 0)
		sum /= weights.size();

	return sum;
}
void Graph::Path::printf() const
{
	cout << " - Nodes: ";
	for (size_t i = 0; i<nodes.size(); ++i)
	{
		cout << nodes[i] << " ";
	}

	cout << "; Weight: " << getWeight() << endl;
}