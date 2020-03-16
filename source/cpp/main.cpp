#include <data.hpp>

int main()
{
	cg::data triMesh;

    if (!triMesh.Read("../input/test_input3.obj"))
	{
		std::cout << "could not read the file" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!triMesh.Write("../output/test_output3.obj"))
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

	
    //writting the boundary/hole edges in .obj format // working with Open3Mod viewer
    std::ofstream file("../output/boundary_hole_edge.obj");
    std::vector<cg::vertex*>& vertexList = triMesh.GetVertexList();
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
    //connecting vertex is a vector containing all the vertex pointer between A and B
    //assuming the vertex index starts from 1
    auto connectingVertex = triMesh.Distance(1,3);

	std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "    Distance between 1st and 3rd vertex " << std::endl
        << "    (in terms of graph edge) is " << connectingVertex.size() << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;

	return 0;
}
