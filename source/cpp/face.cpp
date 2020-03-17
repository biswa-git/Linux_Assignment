#include <face.hpp>

size_t cg::face::count = 0;

cg::face::face() :area(0.0), isOrphanedEdgeRemoveFlag{ true }, previous(nullptr),distance(SIZE_MAX),id(0) //NOT SURE ABOUT GCC INF
{
}

cg::face::~face()
{
}

void cg::face::SetOrphanedEdgeRemoveFlag(bool status)
{
	isOrphanedEdgeRemoveFlag = status;
}

void cg::face::SetPrevious(cg::face* P)
{
	previous = P;
}

cg::face* cg::face::GetPrevious()
{
	return previous;
}

void cg::face::SetDistance(const size_t& D)
{
	distance = D;
}

size_t cg::face::GetDistance()
{
	return distance;
}

cg::tri_face* cg::tri_face::New(vertex* A, vertex* B, vertex* C, const size_t& id_)
{
    return new tri_face(A,B,C,id_);
}

cg::tri_face::~tri_face()
{
	for (size_t i = 0; i < 3; i++)
	{	
		auto parentEdge = halfEdge[i]->GetParentEdge();
		halfEdge[i]->SetNext(nullptr);
		halfEdge[i]->SetFace(nullptr);
		if (isOrphanedEdgeRemoveFlag && parentEdge->GetHalfEdge(0)->GetFace() == nullptr && parentEdge->GetHalfEdge(1)->GetFace() == nullptr)
		{	
			//DELETE THE EDGE IF IT IS ORPHANED. i.e. NO ASSOCIATED WITH ANY FACE/FACES.
			delete parentEdge;
		}
	}
}

double cg::tri_face::GetArea()
{
	return area;
}

cg::tri_face::tri_face(vertex* A, vertex* B, vertex* C, const size_t& id_ = 0)
{
    id = id_;
	halfEdge.resize(3, nullptr);
	//IF IT IS COLINEAR THEN IT IS NOT A VALID SURFACE. WE NEED TO STOP THAT.
	auto lhs = (A->GetYCoord() - B->GetYCoord()) / (A->GetXCoord() - B->GetXCoord());
	auto rhs = (A->GetYCoord() - C->GetYCoord()) / (A->GetXCoord() - C->GetXCoord());
	if (fabs(lhs - rhs) < 1e-12)
	{
		std::cout << "Warning:The points of the triangle wiht id="<< id <<"are colinear!" << std::endl;
	}

	//	FIRST WE CREATE THREE EDGE / CHECK EXISTANCE

	vertex* tempVertexArray[3]{ A,B,C };
	edge* triEdge[3]{ nullptr,nullptr,nullptr };

	//CHECKING WHETHER EDGE EXIST BETWEEN VERTEX
	for (size_t i = 0; i < 3; i++)
	{
		auto edgeList = tempVertexArray[i]->GetAssociatedEdge();
		for (auto it = edgeList.begin(); it != edgeList.end(); it++)
		{
			auto associatedEdge = *it;
			if (associatedEdge->GetEnd() == tempVertexArray[(i + 1) % 3] || associatedEdge->GetStart() == tempVertexArray[(i + 1) % 3])
			{
				triEdge[i] = associatedEdge;
				break;
			}
		}
		if (triEdge[i] == nullptr) triEdge[i] = edge::New(tempVertexArray[i], tempVertexArray[(i + 1) % 3]);
	}

	for (size_t i = 0; i < 3; i++)
	{
		vector directionVec(triEdge[i]->GetHalfEdge(0)->GetEnd(), tempVertexArray[(i+2)%3]);

		if ((triEdge[i]->GetHalfEdge(0)->GetEdgeVector() ^ directionVec) > 0.0)
		{
			halfEdge[i] = triEdge[i]->GetHalfEdge(0);
		}
		else
		{
			halfEdge[i] = triEdge[i]->GetHalfEdge(1);
		}
		halfEdge[i]->SetFace(this);
	}

	if (halfEdge[0]->GetEnd() == halfEdge[1]->GetStart())
	{
		halfEdge[0]->SetNext(halfEdge[1]);
		halfEdge[1]->SetNext(halfEdge[2]);
		halfEdge[2]->SetNext(halfEdge[0]);
	}
	else
	{
		halfEdge[0]->SetNext(halfEdge[2]);
		halfEdge[2]->SetNext(halfEdge[1]);
		halfEdge[1]->SetNext(halfEdge[0]);
	}
	//AREA BEING A BASIC PROPERTY OF FACE, BEING CALCULATED IMMIDIATELY
	CalculateArea();
}

void cg::tri_face::CalculateArea()
{
	vector firstEdgeVector(halfEdge[0]->GetStart(), halfEdge[0]->GetEnd());
	vector secondEdgeVector(halfEdge[1]->GetStart(), halfEdge[1]->GetEnd());
    area = 0.5*fabs(firstEdgeVector ^ secondEdgeVector);
}

std::vector<cg::half_edge*>& cg::tri_face::GetHalfEdgeVector()
{
	return halfEdge;
}

bool cg::tri_face::IsInside(vertex* V)
{
	for (size_t i = 0; i < 3; i++)
	{
		double crossProduct = cg::CrossProductByVertex2D(halfEdge[i]->GetStart(), halfEdge[i]->GetEnd(), V);
		if (crossProduct <= 0.0) return false;
	}
	return true;
}
