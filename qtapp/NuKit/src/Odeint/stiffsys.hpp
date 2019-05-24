#pragma once
#include <stdio.h>
#include <boost/phoenix/core.hpp>
typedef boost::numeric::ublas::vector<double> vector_type;
typedef boost::numeric::ublas::matrix<double> matrix_type;

struct stiff_system
{
	matrix_type iJ;
	void operator()(const vector_type &x, vector_type &dxdt, double /* t */)
	{
		for (size_t i = 0; i < dxdt.size(); i++)
		{
			dxdt[i] = 0;
			for (size_t j = 0; j < dxdt.size(); j++)
				if (iJ(i, j) != 0)
					dxdt[i] += iJ(i, j) * x[j];
		}
	}
};

struct stiff_system_jacobi
{
	matrix_type iJ;
	void operator()(const vector_type & /* x */, matrix_type &J, const double & /* t */, vector_type &dfdt)
	{
		J = iJ;
		for (size_t i = 0; i < dfdt.size(); i++)
		{
			dfdt[i] = 0.0;
		}
	}
};