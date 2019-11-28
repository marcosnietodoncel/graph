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

#ifndef _GRAPH_H_
#define _GRAPH_H_

// -- STL -- //
#include <set>
#include <cstring>
#ifdef __linux__
  #include <cxxabi.h>
#endif
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <cassert>
#include <limits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>

// -- OpenCV -- //
#include <opencv2/opencv.hpp>

/* This class creates a Graph from a Matrix containing the weights between nodes and obtains the best paths maximizing the weights.
*/
class Graph
{
public:
    /* This class represents the Path as a vector of Nodes with associated weights between pairs of Nodes
    */
    class Path
    {
    public:
        /* This function can be used through addEdge, or directly, to create a Path with a single Node
        */
        void addNode(int id);
        /* Reverses the order of the nodes inside the vectors.
        */
        void reversePath();
        /* Adds an edge (i.e. a pair of nodes and the weight between them)
        */
        void addEdge(int id1, int id2, float w12);

        /* Computes the weight as the average weight of the path.
        */
        float getWeight() const;

        std::vector<int> getNodes() const { return nodes; }
        std::vector<float> getWeights() const { return weights; }
        int getNumNodes() const { return static_cast<int>(nodes.size()); }

        void printf() const;

    protected:
        void addWeight(float w) { weights.push_back(w); }

    private:
        std::vector<int> nodes;
        std::vector<float> weights;
    };

    Graph(const cv::Mat& _weights, bool _verbose=false);

    std::vector<Path> getBestPaths() const { return bestPaths; }

private:
    int numNodes;
    std::vector<Path> paths;
    std::vector<Path> bestPaths;
	bool verbose;

    void computePaths(const cv::Mat& _weights, std::vector<Path>& _paths);
    void iteration(Path _currentPath, int _currentNode, std::vector<Path>& _pathVector, const cv::Mat& _weights, const cv::Mat& _cols);

};

#endif // _GRAPH_H_