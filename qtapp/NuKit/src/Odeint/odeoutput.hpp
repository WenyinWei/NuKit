#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <map>
#include <fstream>
#include <boost/phoenix/core.hpp>
#include "element.hpp"

#include <QString>
#include <QtScript/QScriptEngine> // For input the neutron flux

typedef boost::numeric::ublas::vector<double> vector_type;

struct write_on_terminal
{
	size_t m_every, m_count;

	write_on_terminal(size_t every = 10)
		: m_every(every), m_count(0) {}

	void operator()(const vector_type &x, double t)
	{
		if ((m_count % m_every) == 0)
		{
			std::clog << t << std::endl;
			std::cout << "sp '-'" << std::endl;
			for (size_t i = 0; i < x.size(); ++i)
				std::cout << "i: " << i << "\tx(i):" << x(i) << std::endl;
			std::cout << "e" << std::endl
					  << std::endl;
		}
		++m_count;
	}
};

// write_snapshots snapshots;
// snapshots.snapshots().insert(make_pair(size_t(0), string("lat_0000.dat")));
// snapshots.snapshots().insert(make_pair(size_t(10), string("lat_0010.dat")));
// snapshots.snapshots().insert(make_pair(size_t(20), string("lat_0020.dat")));
// obs.observers().push_back(snapshots);
class write_snapshots
{
public:
	typedef std::map<size_t, std::string> map_type;

	write_snapshots(void) : m_count(0) {}

	void operator()(const vector_type &x, double t)
	{
		std::map<size_t, std::string>::const_iterator it = m_snapshots.find(m_count);
		if (it != m_snapshots.end())
		{
			std::ofstream fout(it->second.c_str());
			for (size_t i = 0; i < x.size(); ++i)
				fout << "i: " << i << "\tx(i):" << x(i) << std::endl;
			fout << "\n";
		}
		++m_count;
	}

	map_type &snapshots(void) { return m_snapshots; }
	const map_type &snapshots(void) const { return m_snapshots; }

private:
	size_t m_count;
	map_type m_snapshots;
};

struct write_csv
{
	size_t m_every, m_count;
	std::vector<Element> m_vElement;
	std::string m_odeFile = "../data/oderun.csv";
	std::string m_neutronFile = "../data/t_neutron.csv";
	double iNeutronN;
	QString iNeutronText;

	write_csv(std::vector<Element> vElement, QString NeutronText, size_t every = 1)
		: m_every(every), m_count(0), m_vElement(vElement), iNeutronText(NeutronText)
	{
		std::ofstream odefout(m_odeFile.c_str(), std::ofstream::out);
		std::ofstream nfout(m_neutronFile.c_str(), std::ofstream::out);
		odefout << "iter_index,time";
		for (size_t i = 0; i < m_vElement.size(); ++i)
			odefout << "," << m_vElement[i].outname();
		odefout << std::endl;
		nfout << "time,n_1" << std::endl;
	}

	void operator()(const vector_type &x, double t)
	{
		if ((m_count % m_every) == 0)
		{
			std::ofstream odefout(m_odeFile.c_str(), std::ofstream::app);
			std::ofstream nfout(m_neutronFile.c_str(), std::ofstream::app);
			odefout << m_count << "," << t;
			for (size_t i = 0; i < x.size(); ++i)
				odefout << "," << x(i);
			odefout << std::endl;

			QScriptEngine expression;
			iNeutronN = expression.evaluate(iNeutronText.repeated(1).replace("T", QString::number(t))).toNumber();
			nfout << t << "," << iNeutronN << std::endl;
		}
		++m_count;
	}
};
