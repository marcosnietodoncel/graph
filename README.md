# Graph
Graph solving algorithm

Graph is a simple library for solving a Path Finding problem on non-cyclic directed Graphs,
being the output a list of Paths connecting Nodes.
The algorithm, unlike other Shortest Path algorithms, doesn't compute the sum of costs
between nodes of the Path, but computes the average. As a result, this algorithm is more
suited for problems where interconnected nodes represent the same object or entity, and where
the edge between two nodes represents the likelihood these two nodes correspond to the same entity.

The algorithm, called ENBP (End-Node Back Propagation) is documented and to be published in paper
form. This note will be uptated should the paper be accepted for publication.

# License
GNU General Publick License v3.0 (https://www.gnu.org/licenses/gpl-3.0.html)

As Graph depends on other libraries, the user must adhere to and keep in place any
licencing terms of those libraries:

* OpenCV v3.0.0 (or any other version) (http://opencv.org)

## License Agreement for OpenCV
BSD 2-Clause License (http://opensource.org/licenses/bsd-license.php)
The dependence of the "Non-free" module of OpenCV is excluded from Graph.
