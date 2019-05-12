#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

class Element;

struct NuReaction
{
	std::string Retype;  // Nuclear reaction name
	Element *ReDaughter; // Nuclear reaction daughter(the product)
	Element *ReMother;   // Nuclear reaction mother(the source)
	double ReConst;		 // Decay reaction constant, in [h^-1]
};

class Element
{
private: // constants for a kind of specific element
	std::string name;
	int Anum;
	int Nnum;
	int Znum;

public: // decay infomation
	std::vector<NuReaction> ReactionAsM;
	std::vector<NuReaction> ReactionAsD;
	int Eindex;

public: // forced initialization format
	Element(std::string iname, int iAnum, int iZnum, int iNnum)
	{
		name = iname;
		Anum = iAnum;
		Nnum = iNnum;
		Znum = iZnum;
	}

	Element(std::string iname, int iAnum, int iZnum)
	{
		name = iname;
		Anum = iAnum;
		Nnum = iAnum - iZnum;
		Znum = iZnum;
	}

public: // overload comparative operator
	friend bool operator<(const Element &a, const Element &b)
	{
		if (a.Znum != b.Znum)
			return a.Znum < b.Znum;
		else
			return a.Nnum < b.Nnum;
	};
	friend bool operator>(const Element &a, const Element &b)
	{
		if (a.Znum != b.Znum)
			return a.Znum > b.Znum;
		else
			return a.Nnum > b.Nnum;
	};
	friend bool operator==(const Element &a, const Element &b)
	{
		return (a.Znum == b.Znum) && (a.Nnum == b.Nnum);
	};
	void show(void)
	{
		std::cout << (*this).name << ' ' << (*this).Anum << ' ' << (*this).Znum << std::endl;
	};
	std::string outname(void)
	{
		return (*this).name + "_" + std::to_string((*this).Anum);
	};
	std::string getname(void)
	{
		return (*this).name;
	};
	int getAnum(void)
	{
		return (*this).Anum;
	};
	int getZnum(void)
	{
		return (*this).Znum;
	};
	int getNnum(void)
	{
		return (*this).Nnum;
	};
};

bool sort_element(const Element &s1, const Element &s2)
{
	return s1 < s2;
}
