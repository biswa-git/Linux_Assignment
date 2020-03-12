#include "data.hpp"

cg::data::data() :isDataRead(false), isBoundaryCalculated(false)
{
}

cg::data::~data()
{
	/*
	if (!faceList.empty())
	{
		for (auto it = faceList.begin(); it != faceList.end(); ++it)
		{
			FREE_OBJ_MACRO(*it);
		}
	}
	if (!vertexList.empty())
	{
		for (auto it = vertexList.begin(); it != vertexList.end(); ++it)
		{
			FREE_OBJ_MACRO(*it);
		}
	}
	*/

}

std::vector<cg::vertex*>& cg::data::GetVertexList()
{
	return vertexList;
}

std::vector<cg::face*>& cg::data::GetFaceList()
{
	return faceList;
}

std::list<std::set<cg::edge*>>& cg::data::GetBoundaryList()
{
	return boundaryList;
}

bool cg::data::Read(const std::string& fileName)
{
	vertex* vertexData = NULL;
	face* faceData = NULL;

	std::ifstream fileIn(fileName.c_str());
	if (!fileIn.is_open()) return isDataRead;

	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
	std::string line;

	double coord[3];
	size_t vertexId[3];
	size_t vertNum = 0, faceNum = 0;

	while (getline(fileIn, line))
	{
		Separator sep{ " " };
		Tokenizer tok(line, sep);
		std::vector<double> rowData;
		auto it = tok.begin();
		auto identifier = *it;

		if (identifier == "v")
		{
			for (auto i = 0; i < 3; ++i)
			{
				coord[i] = std::stod(*(++it));
			}
			++vertNum;
			vertexData = vertex::New(coord[0], coord[1], coord[2], vertNum);
			vertexList.push_back(vertexData); vertexData = NULL;
		}
		else if (identifier == "f")
		{
			for (auto i = 0; i < 3; ++i)
			{
				vertexId[i] = std::stoi(*(++it)) - size_t(1);
			}
			++faceNum;
			faceData = tri_face::New(vertexList[vertexId[0]], vertexList[vertexId[1]], vertexList[vertexId[2]], faceNum);
			faceList.push_back(faceData); faceData = NULL;
		}
	}
	return true;
}

bool cg::data::Write(const std::string& fileName)
{
	//writting the data in .obj format // working with Open3Mod viewer
	std::ofstream file(fileName);
	for (auto it = vertexList.begin(); it != vertexList.end(); ++it)
	{
		file << "v " << (*it)->GetXCoord() << " " << (*it)->GetYCoord() << " " << (*it)->GetZCoord() << std::endl;

	}
	for (auto it = faceList.begin(); it != faceList.end(); ++it)
	{
		auto halfEdgeVector = (*it)->GetHalfEdgeVector();
		file << "f";
		for (auto it_hev = halfEdgeVector.begin(); it_hev != halfEdgeVector.end(); ++it_hev)
		{
			file << " " << (*it_hev)->GetStart()->GetId();
		}
		file << std::endl;
	}
	return true;
}

void cg::data::CalculateBoundaryInfo()
{
	if (isBoundaryCalculated) return;

	//BOUNDAR/LOOP DETECTION CODE
	std::set<edge*> boundaryEdgeSet;
	for (auto it_fl = faceList.begin(); it_fl != faceList.end(); it_fl++)
	{
		auto halfEdgeVector = (*it_fl)->GetHalfEdgeVector();
		for (auto it_hev = halfEdgeVector.begin(); it_hev != halfEdgeVector.end(); it_hev++)
		{
			auto parentEdge = (*it_hev)->GetParentEdge();
			if (parentEdge->GetHalfEdge(0)->GetFace() == nullptr || parentEdge->GetHalfEdge(1)->GetFace() == nullptr)
			{
				boundaryEdgeSet.emplace(parentEdge);
			}
		}
	}

	while (!boundaryEdgeSet.empty())
	{
		std::set<edge*> edgeSet;
		std::stack<edge*> edgeStack;	//to replace recursive function
		edgeStack.push(*boundaryEdgeSet.begin());
		//detecting the loop
		while (!edgeStack.empty())
		{
			auto top = edgeStack.top();
			auto start = top->GetStart();
			auto end = top->GetEnd();
			auto iterator = boundaryEdgeSet.find(top);
			if (iterator != boundaryEdgeSet.end())boundaryEdgeSet.erase(iterator);
			edgeSet.emplace(top);
			edgeStack.pop();

			for (auto it = start->GetAssociatedEdge().begin(); it != start->GetAssociatedEdge().end(); it++)
			{
				auto presentEdge = *it;
				if (((presentEdge->GetHalfEdge(0)->GetFace() == nullptr) || (presentEdge->GetHalfEdge(1)->GetFace() == nullptr)) && (edgeSet.find(presentEdge) == edgeSet.end()))
				{
					//means this is a new edge
					edgeStack.push(presentEdge);
				}
			}

			for (auto it = end->GetAssociatedEdge().begin(); it != end->GetAssociatedEdge().end(); it++)
			{
				auto presentEdge = *it;
				if (((presentEdge->GetHalfEdge(0)->GetFace() == nullptr) || (presentEdge->GetHalfEdge(1)->GetFace() == nullptr)) && (edgeSet.find(presentEdge) == edgeSet.end()))
				{
					//means this is a new edge
					edgeStack.push(presentEdge);
				}
			}
		}
		boundaryList.emplace_back(edgeSet);
	}
}

//IT IS BASED ON BFS ALGO
std::vector<cg::face*> cg::data::Distance(const size_t& a, const size_t& b)
{
	face* A = faceList[a - 1];
	face* B = faceList[b - 1];
	std::vector<cg::face*> distanceVector; //stores and return inbetween connecting faces

	A->SetDistance(size_t(0));
	std::queue<face*> crawlQueue;
	crawlQueue.push(A);

	auto crawlFlag = true;
	while (crawlFlag)
	{
		auto front = crawlQueue.front();
		crawlQueue.pop();
		auto halfEdgeVector = front->GetHalfEdgeVector();
		for (auto it_he = halfEdgeVector.begin(); it_he != halfEdgeVector.end(); ++it_he)
		{
			auto parentEdge = (*it_he)->GetParentEdge();
			face* neighbourFace = nullptr;
			if (parentEdge->GetHalfEdge(0) != (*it_he))
			{
				neighbourFace = parentEdge->GetHalfEdge(0)->GetFace();
			}
			else
			{
				neighbourFace = parentEdge->GetHalfEdge(1)->GetFace();
			}
			if (neighbourFace != nullptr && (front->GetDistance() + 1) < neighbourFace->GetDistance())
			{
				neighbourFace->SetDistance(front->GetDistance() + 1);
				neighbourFace->SetPrevious(front);
				if (neighbourFace == B)
				{
					crawlFlag = false;
				}
				else
				{
					crawlQueue.push(neighbourFace);
				}
			}
		}
	}
	
	auto presentFace = B;
	while (presentFace!=nullptr)
	{
		distanceVector.push_back(presentFace);
		presentFace = presentFace->GetPrevious();
	}
	//RESETING DISTANCE
	for (auto it = faceList.begin(); it != faceList.end(); ++it)
	{
		(*it)->SetDistance(99999999999);
	}

	return distanceVector;
}
