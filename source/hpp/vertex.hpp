#pragma once
#include<essential>
namespace cg
{
	class edge;
	class vertex
	{
	public:
		static vertex* New(const double& = 0.0, const double& = 0.0, const double& = 0.0, const size_t & = 0);
		virtual ~vertex();
		vertex(vertex const&) = delete;
		vertex& operator=(vertex const&) = delete;

		void Reassign(const double& = 0.0, const double& = 0.0, const double& = 0.0);
		size_t GetId() const;
		double GetXCoord() const;
		double GetYCoord() const;
		double GetZCoord() const;
		std::set<edge*>& GetAssociatedEdge();
		void Print() const;
        void SetPrevious(vertex*);
        vertex* GetPrevious();
        void SetDistance(const size_t&);
        size_t GetDistance();
	private:
		vertex(const double& = 0.0, const double& = 0.0, const double& = 0.0, const size_t & = 0);

		static size_t count;
		size_t id;
		double xCoord, yCoord, zCoord;
		std::set<edge*> AssociatedEdge;
        vertex* previous;
        size_t distance;
	};

	double CrossProductByVertex2D(vertex*, vertex*, vertex*);
}
