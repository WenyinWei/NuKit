#pragma once
#include <stdio.h>
#include <QString>
#include <boost/phoenix/core.hpp>
#include <QtScript/QScriptEngine> // For input the neutron flux
typedef boost::numeric::ublas::vector<double> vector_type;
typedef boost::numeric::ublas::matrix<double> matrix_type;

struct stiff_system
{
    matrix_type iDecayJ;
    matrix_type iNeutronJ;
    double iNeutronN;
    QString iNeutronText;
    void operator()(const vector_type &x, vector_type &dxdt, double  t )
	{
        QScriptEngine expression;
        iNeutronN = expression.evaluate(iNeutronText.repeated(1).replace("T", QString::number(t))).toNumber()*3600/pow(10,11);

		for (size_t i = 0; i < dxdt.size(); i++)
		{
            dxdt[i] = 0;
            for (size_t j = 0; j < dxdt.size(); j++)
                    dxdt[i] += iDecayJ(i, j) * x[j] + iNeutronJ(i, j)*iNeutronN * x[j];
		}
	}
};

struct stiff_system_jacobi
{
    matrix_type iDecayJ;
    matrix_type iNeutronJ;
    double iNeutronN;
    QString iNeutronText;
    void operator()(const vector_type & /* x */, matrix_type &J, const double & t , vector_type &dfdt)
    {
        QScriptEngine expression;
        iNeutronN = expression.evaluate(iNeutronText.repeated(1).replace("T", QString::number(t))).toNumber()*3600/pow(10,11);
        J = iDecayJ + iNeutronJ * iNeutronN;
		for (size_t i = 0; i < dfdt.size(); i++)
		{
			dfdt[i] = 0.0;
		}
	}
};
