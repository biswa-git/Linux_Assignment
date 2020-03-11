#include <data.hpp>

int main()
{
	cg::data triMesh;

	if (!triMesh.Read("../input/test_input.obj"))
	{
		std::cout << "could not read the file" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!triMesh.Write("../output/test_output.obj"))
	{
		std::cout << "could not write the file" << std::endl;
		exit(EXIT_FAILURE);
	}

	triMesh.CalculateBoundaryInfo();

	//printing boundary/loop
	std::list<std::set<cg::edge*>>& boundaryEdge = triMesh.GetBoundaryList();
	std::cout << std::endl << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;
	std::cout << "    The mesh has " << boundaryEdge.size() << " boundary/hole" << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;
	//one can access the edges of a bundary/loop also

	//BFS ALGO FOR MINIMUM DISTANCE PATH
	auto connectingTrias = triMesh.Distance(1, 14);

	std::cout << "-------------------------------------------------" << std::endl;
	std::cout << "    Distance between 1st and 14th face " << std::endl
		<< "    (in terms of graph edge) is " << connectingTrias.size() - 1 << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;

	connectingTrias = triMesh.Distance(5,6);

	std::cout << "-------------------------------------------------" << std::endl;
	std::cout << "    Distance between 5th and 6th face " << std::endl
		<< "    (in terms of graph edge) is " << connectingTrias.size() - 1 << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;

	return 0;
}