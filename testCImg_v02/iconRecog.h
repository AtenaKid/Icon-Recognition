#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "CImg.h"
#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
using namespace cimg_library;
using namespace std;

//struct svm_parameter param;     // set by parse_command_line
//struct svm_problem prob;        // set by read_problem

class iconRecog {

private:
	int classifyNum = 17;

	// HOG descriptor parameter
	int WIN = 32;
	int BLOCK = 16;
	int STRIDE = 8;
	int CELL = 8;
	int BIN = 9;
	
	int trainPosDataNum = 55;
	int totalPosDataNum = 60; 
	
	int background = 0;

	int trainNegDataNum = 310;
	int totalNegDataNum = 318; //318
	
	vector<string> hogFileName = {
		"PositiveBack.xml",
		"PositiveClose.xml",
		"PositiveDelete.xml",
		"PositiveDownload.xml",
		"PositiveEdit.xml",
		"PositiveHome.xml",
		"PositiveInfo.xml",
		"PositiveLove.xml",
		"PositiveMenu.xml",
		"PositiveMinus.xml",
		"PositivePlus.xml",
		"PositiveProfile.xml",
		"PositiveSearch.xml",
		"PositiveSettings.xml",
		"PositiveShare.xml",
		"PositiveShopBag.xml",
		"PositiveShopping.xml",
		"Negative.xml"
	};
	
	vector<string> iconClass = {
		"back",
		"close",
		"delete",
		"download",
		"edit",
		"home",
		"info",
		"love",
		"menu",
		"minus",
		"plus",
		"profile",
		"search",
		"settings",
		"share",
		"shopbag",
		"shopping",
		"negative"
	};

	vector<string> cFileName = {
		"icon_image/back/back/back",
		"icon_image/close/close/close",
		"icon_image/delete/delete/delete",
		"icon_image/download/download/download",
		"icon_image/edit/edit/edit",
		"icon_image/home/home/home",
		"icon_image/info/info/info",
		"icon_image/love/love/love",
		"icon_image/menu/menu/menu",
		"icon_image/minus/minus/minus",
		"icon_image/plus/plus/plus",
		"icon_image/profile/profile/profile",
		"icon_image/search/search/search",
		"icon_image/settings/settings/settings",
		"icon_image/share/share/share",
		"icon_image/shopBag/shopBag/shopBag",
		"icon_image/shopping/shopping/shopping",
		"icon_image/negative/negative/negative"
	};

	vector<string> testFileName = {
		"icon_image/REAL_DATA/aliexpress_back.bmp",
		"icon_image/REAL_DATA/aliexpress_close.bmp",
		"icon_image/REAL_DATA/aliexpress_close2.bmp",
		"icon_image/REAL_DATA/aliexpress_delete.bmp",
		"icon_image/REAL_DATA/aliexpress_delete2.bmp",
		"icon_image/REAL_DATA/aliexpress_love.bmp",
		"icon_image/REAL_DATA/aliexpress_menu.bmp",
		"icon_image/REAL_DATA/aliexpress_menu2.bmp",
		"icon_image/REAL_DATA/aliexpress_menu3.bmp",
		"icon_image/REAL_DATA/aliexpress_menu4.bmp",
		"icon_image/REAL_DATA/aliexpress_minus.bmp",
		"icon_image/REAL_DATA/aliexpress_plus.bmp",
		"icon_image/REAL_DATA/aliexpress_plus2.bmp",
		"icon_image/REAL_DATA/aliexpress_search.bmp",
		"icon_image/REAL_DATA/aliexpress_share.bmp",
		"icon_image/REAL_DATA/aliexpress_shopping.bmp",
		"icon_image/REAL_DATA/aliexpress_shopping2.bmp",
		"icon_image/REAL_DATA/aliexpress_shopping3.bmp",
		"icon_image/REAL_DATA/kakao_profile.bmp",
		"icon_image/REAL_DATA/netflix_menu.bmp",
		"icon_image/REAL_DATA/netflix_search.bmp",
		"icon_image/REAL_DATA/outlook_menu.bmp",
		"icon_image/REAL_DATA/youtube_home.bmp",
		"icon_image/REAL_DATA/youtube_search.bmp",
	};
	vector <int> testFileLabel = { 0, 1, 1, 2, 2, 7, 8, 8, 8, 8, 9, 10, 10, 12, 14, 16, 16, 16, 11, 8, 12, 8, 5, 12};
public:	
	double testWithRealData(int cropSize);
	svm_problem prepareTrainning(int cropSize); 
	svm_node* prepareData(string file_name, int cropSize);

	CImg<float> preProcessingImg(const char cImgPath, int crop_size);
	svm_model* trainWithLibSVM(int cropSize); 
	double testSVMTrainedData(int cropSize); 
	void evalParamSVM(); 
};