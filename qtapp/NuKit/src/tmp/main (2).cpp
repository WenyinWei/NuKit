/*
 * rosenbrock4.cpp
 *
 * Copyright 2010-2012 Mario Mulansky
 * Copyright 2011-2012 Karsten Ahnert
 * Copyright 2012 Andreas Angelopoulos
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <map> /* map */

#include <boost/numeric/odeint.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

#include "element.hpp"
#include "odeoutput.hpp"
#include "stiffsys.hpp"
using namespace std;
using namespace boost::numeric::odeint;
namespace phoenix = boost::phoenix;

//[ stiff_system_definition
typedef boost::numeric::ublas::vector<double> vector_type;
typedef boost::numeric::ublas::matrix<double> matrix_type;

int main(int argc, char **argv)
{
    //    typedef rosenbrock4< double > stepper_type;
    //    typedef rosenbrock4_controller< stepper_type > controlled_stepper_type;
    //    typedef rosenbrock4_dense_output< controlled_stepper_type > dense_output_type;
    //[ integrate_stiff_system

    ifstream inFile("Element.csv", ios::in);
    string lineStr;
    vector<vector<string>> strArray;
    while (getline(inFile, lineStr))
    {
        // store the data as 2D table 存成二维表结构
        stringstream ss(lineStr);
        string str;
        vector<string> lineArray;
        // extracted cell data seperated by the ',' 按照逗号分隔
        while (getline(ss, str, ','))
            lineArray.push_back(str);
        strArray.push_back(lineArray);
    }

    vector<Element> vElement;
    for (size_t i = 1; i < strArray.size(); i++)
    {
        Element ele1(strArray[i][3], stoi(strArray[i][1]), stoi(strArray[i][2]));
        Element ele2(strArray[i][13], stoi(strArray[i][11]), stoi(strArray[i][12]));
        vElement.push_back(ele1);
        vElement.push_back(ele2);
    }
    // Now we clear the duplicated elements and sort the vector
    sort(vElement.begin(), vElement.end(), sort_element);
    vector<Element>::iterator unque_it = std::unique(vElement.begin(), vElement.end());
    vElement.erase(unque_it, vElement.end());

    for (size_t i = 0; i < vElement.size(); i++)
        vElement[i].Eindex = i;

    for (size_t i = 1; i < strArray.size(); i++)
    {
        Element ele1(strArray[i][3], stoi(strArray[i][1]), stoi(strArray[i][2]));
        Element ele2(strArray[i][13], stoi(strArray[i][11]), stoi(strArray[i][12]));
        int index1, index2;
        for (size_t j = 0; j < vElement.size(); j++)
            if (ele1 == vElement[j])
            {
                index1 = j;
                break;
            };
        for (size_t j = 0; j < vElement.size(); j++)
            if (ele2 == vElement[j])
            {
                index2 = j;
                break;
            };
        NuReaction tmpNuReaction;
        double d = atof(strArray[i][16].c_str());
        tmpNuReaction.ReConst = d;
        tmpNuReaction.Retype = strArray[i][8];
        tmpNuReaction.ReDaughter = &vElement[index2];
        tmpNuReaction.ReMother = &vElement[index1];
        vElement[index1].ReactionAsM.push_back(tmpNuReaction);
        vElement[index2].ReactionAsD.push_back(tmpNuReaction);
    }

    vector_type x(vElement.size());
    for (size_t i = 0; i < vElement.size(); i++)
        x(i) = 0;
	x(7) = 2301;
	x(8) = 3012;
    x(9) = 2123;
	write_element_list(vElement);
    matrix_type J(vElement.size(), vElement.size());
    for (size_t i = 0; i < vElement.size(); i++)
        for (size_t j = 0; j < vElement.size(); j++)
            J(i, j) = 0;
    for (size_t i = 0; i < vElement.size(); i++)
    {
        for (size_t j = 0; j < vElement[i].ReactionAsD.size(); j++)
            J(i, vElement[i].ReactionAsD[j].ReMother->Eindex) += vElement[i].ReactionAsD[j].ReConst;
        for (size_t j = 0; j < vElement[i].ReactionAsM.size(); j++)
            J(i, i) -= vElement[i].ReactionAsM[j].ReConst;
    }

    stiff_system stiff;
    stiff.iJ = J;
    stiff_system_jacobi stiffj;
    stiffj.iJ = J;

    observer_collection<vector_type, double> obs;
    obs.observers().push_back(write_on_terminal(1));
    obs.observers().push_back(write_csv(vElement, 10,"oderun.csv"));

    size_t num_of_steps = integrate_const(make_dense_output<rosenbrock4<double>>(1.0e-6, 1.0e-6),
                                          make_pair(stiff, stiffj),
                                          x, 0.0, 100.0, 0.001, boost::ref(obs));
    //]

    clog << num_of_steps << endl;

    getchar();
	return 0;
}