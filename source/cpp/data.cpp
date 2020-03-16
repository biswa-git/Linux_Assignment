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
    std::ifstream fileIn(fileName.c_str());
    if (!fileIn.is_open()) return isDataRead;

    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
    std::string line;

    double coord[3]{ 0,0,0 };
    size_t vertexId[3]{ 0,0,0 };
    size_t vertNum = 0, faceNum = 0;

    while (getline(fileIn, line))
    {
        Separator sep{ " /" };
        Tokenizer tok(line, sep);
        std::vector<double> rowData;
        auto it = tok.begin();
        auto identifier = *it;

        if (identifier == "v")
        {
            ++it;
            std::vector<std::string> tempStorage;
            for (; it != tok.end(); ++it)
            {
                tempStorage.emplace_back(*it);
            }

            for (size_t i = 0; i < 3; ++i)
            {
                coord[i] = std::stod(tempStorage[i]);
            }

            ++vertNum;
            vertexList.push_back(vertex::New(coord[0], coord[1], coord[2], vertNum));
        }
        else if (identifier == "f")
        {
            ++it;
            std::vector<std::string> tempStorage;
            for (; it != tok.end(); ++it)
            {
                tempStorage.emplace_back(*it);
            }
            if (tempStorage.size() == 9)
            {
                vertexId[0] = static_cast<size_t>(std::stoi(tempStorage[0]));
                vertexId[1] = static_cast<size_t>(std::stoi(tempStorage[3]));
                vertexId[2] = static_cast<size_t>(std::stoi(tempStorage[6]));
            }
            else if (tempStorage.size() == 6)
            {
                vertexId[0] = static_cast<size_t>(std::stoi(tempStorage[0]));
                vertexId[1] = static_cast<size_t>(std::stoi(tempStorage[2]));
                vertexId[2] = static_cast<size_t>(std::stoi(tempStorage[4]));
            }
            else if (tempStorage.size() == 3)
            {
                vertexId[0] = static_cast<size_t>(std::stoi(tempStorage[0]));
                vertexId[1] = static_cast<size_t>(std::stoi(tempStorage[1]));
                vertexId[2] = static_cast<size_t>(std::stoi(tempStorage[2]));
            }
            ++faceNum;
            auto newFace = tri_face::New(vertexList[vertexId[0]-1], vertexList[vertexId[1]-1], vertexList[vertexId[2]-1], faceNum);
            faceList.push_back(newFace);
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
std::vector<cg::vertex*> cg::data::Distance(const size_t& a, const size_t& b)
{
    vertex* A = vertexList[a - 1];
    vertex* B = vertexList[b - 1];
    std::vector<cg::vertex*> distanceVector; //stores and return inbetween connecting faces

    A->SetDistance(static_cast<size_t>(0));
    std::queue<vertex*> crawlQueue;
	crawlQueue.push(A);

    auto crawlFlag = true;
    if(A==B) crawlFlag = false;
	while (crawlFlag)
	{
		auto front = crawlQueue.front();
		crawlQueue.pop();
        auto edgeSet = front->GetAssociatedEdge();
        for (auto it_e = edgeSet.begin(); it_e != edgeSet.end(); ++it_e)
		{

            vertex* neighbourVertex = nullptr;
            auto start = (*it_e)->GetStart();
            auto end = (*it_e)->GetEnd();

            if (front==start)
			{
                neighbourVertex = end;
			}
            else
			{
                neighbourVertex = start;
            }
            if (neighbourVertex != nullptr && ((front->GetDistance() + 1) < neighbourVertex->GetDistance()))
			{
                neighbourVertex->SetDistance(front->GetDistance() + 1);
                neighbourVertex->SetPrevious(front);
                if (neighbourVertex == B)
				{
					crawlFlag = false;
				}
				else
				{
                    crawlQueue.push(neighbourVertex);
				}
			}
		}
	}
	
    auto presentVertex = B;
    while (presentVertex!=A)
	{
        distanceVector.push_back(presentVertex);
        presentVertex = presentVertex->GetPrevious();
	}
	//RESETING DISTANCE
    for (auto it = vertexList.begin(); it != vertexList.end(); ++it)
	{
        (*it)->SetDistance(SIZE_MAX);
	}
	return distanceVector;
}
