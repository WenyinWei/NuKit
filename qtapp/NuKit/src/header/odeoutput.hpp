#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <map>
#include <fstream>
#include <boost/phoenix/core.hpp>
#include "element.hpp"
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
	std::string m_filename;
	std::vector<Element> m_vElement;
	write_csv(std::vector<Element> vElement, size_t every = 1, std::string filename = "oderun.csv")
		: m_every(every), m_count(0), m_filename(filename), m_vElement(vElement)
	{
		std::ofstream fout(m_filename.c_str(), std::ofstream::out);
		fout << "iter_index,time";
		for (size_t i = 0; i < m_vElement.size(); ++i)
			fout << "," << m_vElement[i].outname();
		fout << std::endl;
	}

	void operator()(const vector_type &x, double t)
	{
		if ((m_count % m_every) == 0)
		{
			std::ofstream fout(m_filename.c_str(), std::ofstream::app);
			fout << m_count << "," << t;
			for (size_t i = 0; i < x.size(); ++i)
				fout << "," << x(i);
			fout << std::endl;
		}
		++m_count;
	}
};

void write_element_list(std::vector<Element> vElement, std::string filename = "vElement.csv")
{
	std::ofstream fout(filename.c_str(), std::ofstream::out);
	for (size_t i = 0; i < vElement.size()-1; ++i)
		fout << vElement[i].getname()<<",";
	fout << vElement[vElement.size() - 1].getname() << std::endl;
	for (size_t i = 0; i < vElement.size() - 1; ++i)
		fout << vElement[i].getAnum() << ",";
	fout << vElement[vElement.size() - 1].getAnum() << std::endl;
	for (size_t i = 0; i < vElement.size() - 1; ++i)
		fout << vElement[i].getZnum() << ",";
	fout << vElement[vElement.size() - 1].getZnum() << std::endl;
	for (size_t i = 0; i < vElement.size() - 1; ++i)
		fout << vElement[i].getNnum() << ",";
	fout << vElement[vElement.size() - 1].getNnum() << std::endl;
};