#pragma once

#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include <vector>
using namespace std; 

// Check the angle
//
void getMinMaxArry(vector<unsigned int>& tmp_angles, unsigned int &maxVal, unsigned int &minVal, bool isDisplay) {
	//int minVal = -100; int maxVal = 0;
	for (unsigned int i = 0; i < tmp_angles.size(); i++)
	{
		if (tmp_angles[i] < minVal)
			minVal = tmp_angles[i];
		if (tmp_angles[i] > maxVal)
			maxVal = tmp_angles[i];
	}
	if (isDisplay == true)
	{
		cout << "Range of angles: [" << minVal << ", " << maxVal << "]" << '\n';
	}
		
}
