#pragma once
#include <vertex.hpp>
#include <essential>
namespace cg
{
	class vector
	{
	public:
		static vector* New(const double& = 0.0, const double& = 0.0, const double& = 0.0);
		static vector* New(vertex*, vertex*);

		vector(vertex*, vertex*);
		vector(const double& = 0.0, const double& = 0.0, const double& = 0.0);
		virtual ~vector();

		vector operator+(const vector&);
		vector operator-(const vector&);
		double operator*(const vector&); // DOT PRODUCT
		double operator^(const vector&); // CROSS PRODUCT
		double Abs();

		double GetDx() const;
		double GetDy() const;
		double GetDz() const;

		void Reassign(vertex*, vertex*);
		void Reassign(const double&, const double&, const double& = 0.0);

		void Print() const;

	private:
		double dx, dy, dz;
	};
}