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
	}
	friend bool operator>(const Element &a, const Element &b)
	{
		if (a.Znum != b.Znum)
			return a.Znum > b.Znum;
		else
			return a.Nnum > b.Nnum;
	}
	friend bool operator==(const Element &a, const Element &b)
	{
		return (a.Znum == b.Znum) && (a.Nnum == b.Nnum);
	}
	void show(void)
	{
		std::cout << (*this).name << ' ' << (*this).Anum << ' ' << (*this).Znum << std::endl;
	}
	std::string outname(void)
	{
		return (*this).name + "_" + std::to_string((*this).Anum);
	}
};

bool sort_element(const Element &s1, const Element &s2)
{
	return s1 < s2;
}

// #include <CkCsv.h>

// void ChilkatSample(void)
//     {
//     //  This example program loads a file (japanese.csv)
//     //  that contains this content:
//     //
//     //  year,color,country,food
//     //  2001,red,France,cheese
//     //  2005,blue,"United States",hamburger
//     //  2008,green,Italy,pasta
//     //  1998,orange,Japan,sushi
//     //  2010,silver,"太志","森田"
// //
//     //

//     CkCsv csv;

//     //  Prior to loading the CSV file, indicate that the 1st row
//     //  should be treated as column names:
//     csv.put_HasColumnNames(true);

//     //  Load the CSV records from the file.
//     //  Important: Make sure to call LoadFile2 so that the charset (utf-8)
//     //  of the CSV file can be indicated.
//     bool success = csv.LoadFile2("qa_data/csv/japanese.csv","utf-8");
//     if (success != true) {
//         std::cout << csv.lastErrorText() << "\r\n";
//         return;
//     }

//     //  Display the contents of the 3rd column
//     int row;
//     int n = csv.get_NumRows();
//     for (row = 0; row <= n - 1; row++) {
//         std::cout << csv.getCell(row,2) << "\r\n";
//     }
//     }