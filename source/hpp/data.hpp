#pragma once
#include <fstream>
#include <face.hpp>
#include<boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

namespace cg
{

	class data
	{
	public:
		data();
		~data();
		std::vector<vertex*>& GetVertexList(); 
		std::vector<face*>& GetFaceList();
		std::list<std::set<edge*>>& GetBoundaryList();
		bool Read(const std::string&);
		bool Write(const std::string&);
		void CalculateBoundaryInfo();
		std::vector<face*> Distance(const size_t&, const size_t&);
	private:
		bool isDataRead, isBoundaryCalculated;
		std::vector<vertex*> vertexList;
		std::vector<face*> faceList;
		std::list<std::set<edge*>> boundaryList;


	};
}