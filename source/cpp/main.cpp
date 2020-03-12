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

	
	//writting the data in .obj format // working with Open3Mod viewer
	std::ofstream file("../output/openedge.obj");
	auto vertexList = triMesh.GetVertexList();
	auto faceList = triMesh.GetFaceList();
	for (auto it = vertexList.begin(); it != vertexList.end(); ++it)
	{
		file << "v " << (*it)->GetXCoord() << " " << (*it)->GetYCoord() << " " << (*it)->GetZCoord() << std::endl;

	}
	for (auto it_be = boundaryEdge.begin(); it_be != boundaryEdge.end(); ++it_be)
	{
		for (auto it_be_s = (*it_be).begin(); it_be_s != (*it_be).end(); ++it_be_s)
		{
			file << "l " << (*it_be_s)->GetStart()->GetId() << " " << (*it_be_s)->GetEnd()->GetId() << std::endl;
		}

	}
	
	
	
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
