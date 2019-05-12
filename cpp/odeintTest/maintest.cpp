#include "element.hpp"
#include <stdio.h>
#include <algorithm>
using namespace std;
int main(int argc, char *argv[])
{
    vector<Element> vElement;

    Element tmp1("U_235", 235, 92);
    vElement.push_back(tmp1);
	Element tmp3("U_235", 235, 92);
	vElement.push_back(tmp3);
    Element tmp2("U_238", 238, 92);
    vElement.push_back(tmp2);

    cout << "before sort:" << endl;
    for (unsigned int i = 0; i < vElement.size(); i++)
        vElement[i].show();

    sort(vElement.begin(), vElement.end(), sort_element);

    vector<Element>::iterator unque_it = std::unique(vElement.begin(), vElement.end());
    vElement.erase(unque_it, vElement.end());

    cout << endl << "after unique" << endl;
    for (unsigned int i = 0; i < vElement.size(); i++)
        vElement[i].show();
	getchar();
    return 0;
}