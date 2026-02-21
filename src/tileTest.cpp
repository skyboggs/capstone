#include <iostream>

#include "tile.h"

using namespace std;

int main()
{
	tile testTile;

	cout << endl;
	cout << "printTile():" << endl;
	cout << "==-->" << endl;
	testTile.printTile();
	cout << "<--==" << endl;
	cout << endl;

	cout << endl;
	cout << "operator<<:" << endl;
	cout << "==-->" << endl;
	cout << testTile;
	cout << "<--==" << endl;
	cout << endl;

	cout << endl;
	cout << "operator<<:" << endl;
	cout << "==-->" << endl;
	cout << tile(4,4);
	cout << "<--==" << endl;
	cout << endl;

	cout << endl;
	cout << "operator<<:" << endl;
	cout << "==-->" << endl;
	cout << tile(4,4,2,2);
	cout << "<--==" << endl;
	cout << endl;

	cout << endl;
	cout << "setDimensions(3,3):" << endl;
	cout << "==-->" << endl;
	testTile.setDimensions(3,3);
	cout << testTile;
	cout << "<--==" << endl;
	cout << endl;

	cout << endl;
	cout << "setCoords(2,2):" << endl;
	cout << "==-->" << endl;
	testTile.setCoords(2,2);
	cout << testTile;
	cout << "<--==" << endl;
	cout << endl;

	return 0;
}
