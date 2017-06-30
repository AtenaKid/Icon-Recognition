/*
create by rosie  2017.03.30
edited by Atena Nguyen 2017.05.31
*/
#include "CImg.h"
#include "iconRecog.h"
#include "utilz.hxx"
#include "debugUtils.h"
#include <iomanip>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace cimg_library;
using namespace std; 

// Struct from svm 
struct svm_parameter param;     // set by parse_command_line
struct svm_problem prob;        // set by read_problem
struct svm_model *model;
struct svm_node *x_space;

int nwin_x = 4, nwin_y = 4;			// number of HOG window per image
int B = 7;

/*
Pre-processing image 
*/
CImg<float> iconRecog::preProcessingImg(const char cImgPath, int iImgSize)
{
	CImg<unsigned char> image;
	//image.load( (testFileName[i]).c_str());
	image.load("abc.bmp");

	// test gradient 

	int width = image._width, height = image._height;

	// ======== convert RGB to gray_scale image ===========
	CImg<unsigned char> gray_img = image;
	gray_img.RGBtoYCbCr().channel(0);

	// ============== Convert to Binary image ===============
	// find threshold of grayscale image
	int threshold = otsu_threshold(gray_img);
	CImg<unsigned char> binary_img(width, height, 1, 1, 0);
	binary_img = getBinary(gray_img, threshold);

	// ============= Cropping the image ====================
	// Find the minimum and maximum in horizontal 	
	CImg<unsigned char> gray_crop = cropImg(gray_img, binary_img);

	// ============= Making Square Image ===================
	//int maxSize = max(width, height);
	int maxSize = 16;
	CImg<float> gray_crop_resize = gray_crop.resize(maxSize, maxSize, 1, 1, 5);


	return gray_crop_resize;
}

/*
* Prepare data 
*/
svm_node* iconRecog::prepareData(string cFileName, int maxSize)
{
	// ===================== Prepare Data =======================================	
	//char cFullFileName[100];	// Total 100 files 
								// possitive data
								//struct svm_problem prob;        // set by read_problem
	//sprintf_s(cFullFileName, "%s%d.bmp", cFileName[i].c_str(), j);
	//cout << cFullFileName << '\n';
	// load image
	CImg<unsigned char> image;
	image.load(cFileName.c_str());
	//image.load("abc.bmp");
	int width = image._width, height = image._height;

	// ======== convert RGB to gray_scale image ===========
	CImg<unsigned char> gray_img = image;
	gray_img.RGBtoYCbCr().channel(0);

	// ============== Convert to Binary image ===============
	// find threshold of grayscale image
	int threshold = otsu_threshold(gray_img);
	CImg<unsigned char> binary_img(width, height, 1, 1, 0);
	binary_img = getBinary(gray_img, threshold);

	// ============= Cropping the image ====================
	// Find the minimum and maximum in horizontal 	
	CImg<unsigned char> gray_crop = cropImg(gray_img, binary_img);

	// ============= Making Square Image ===================
	CImg<float> gray_crop_resize = gray_crop.resize(maxSize, maxSize, 1, 1, 5);

	// ============= Calculate HOG Features =====================
	//int nwin_x = 5, nwin_y = 5;			// number of HOG window per image
	//int B = 9;							// the number of histogram bins
	int feature_size = nwin_x*nwin_y*B;
	vector<float> H(feature_size, 0);
	H = HOG_Feature_Gen(gray_crop_resize, nwin_x, nwin_y, B);

	// The size should be 1 more larger than the Features size 
	svm_node* x_space = Malloc(svm_node, feature_size);
	for (int col = 0; col < feature_size; col++)
	{
		x_space[col].index = col;
		x_space[col].value = H[col];
	}
	x_space[feature_size].index = -1; 
	return x_space;
}


/*
* Prepare the training data 
*	- maxSize: the size of cropped data; 
*/
svm_problem iconRecog::prepareTrainning(int maxSize)
{
	// ===================== Prepare Data =======================================
	char cFullFileName[100];	// Total 100 files 
								// possitive data
								//struct svm_problem prob;        // set by read_problem
	svm_problem tmp_prob; 
	tmp_prob.l = classifyNum * trainPosDataNum;	// this is number of training sample 
	svm_node** x = Malloc(svm_node*, tmp_prob.l);
	tmp_prob.y = Malloc(double, tmp_prob.l);

	int idx = 0;	// index for each training data  
	for (int i = 0; i < classifyNum; i++)
	{
		// For each class  
		// vector< vector <float> > vHogFeatures; 

		for (int j = 0; j < trainPosDataNum; j++)
		{
			sprintf_s(cFullFileName, "%s%d.bmp", cFileName[i].c_str(), j);
			//cout << cFullFileName << '\n';
			// load image
			CImg<unsigned char> image;
			image.load(cFullFileName);
			//image.load("abc.bmp");
			int width = image._width, height = image._height;

			// ======== convert RGB to gray_scale image ===========
			CImg<unsigned char> gray_img = image;
			gray_img.RGBtoYCbCr().channel(0);

			// ============== Convert to Binary image ===============
			// find threshold of grayscale image
			int threshold = otsu_threshold(gray_img);
			CImg<unsigned char> binary_img(width, height, 1, 1, 0);
			binary_img = getBinary(gray_img, threshold);

			// ============= Cropping the image ====================
			// Find the minimum and maximum in horizontal 	
			CImg<unsigned char> gray_crop = cropImg(gray_img, binary_img);

			// ============= Making Square Image ===================
			//int maxSize = max(width, height);
			//int maxSize = 16;
			CImg<float> gray_crop_resize = gray_crop.resize(maxSize, maxSize, 1, 1, 5);

			// ============= Calculate HOG Features =====================
			//int nwin_x = 3, nwin_y = 3;			// number of HOG window per image
			//int B = 9;							// the number of histogram bins
			int feature_size = nwin_x*nwin_y*B;
			vector<float> H(feature_size, 0);
			H = HOG_Feature_Gen(gray_crop_resize, nwin_x, nwin_y, B);
			// Now making sample 
			tmp_prob.y[idx] = i;
		
			// Now making the SVM training data
			// The size should be 1 more larger than the Features size 
			svm_node* x_space = Malloc(svm_node, feature_size + 1);
			for (int col = 0; col < feature_size; col++)
			{
				x_space[col].index = col;
				x_space[col].value = H[col];
			}
			x_space[feature_size].index = -1;
			// Copy it to the field x 
			x[idx] = x_space;
			idx = idx + 1;
		}

	}

	// coppy the data to problem 
	tmp_prob.x = x;
	
	cout << "Finish prepare training data" << endl; 

	return tmp_prob; 

}

/*
* Extract HoG Features form test image and put into proper SVM training format 
*/
svm_model* iconRecog::trainWithLibSVM(int cropSize)
{

	// ===================== Prepare Data =======================================
	char cFullFileName[100];	// Total 100 files 
	// possitive data
	//struct svm_problem prob;        // set by read_problem
	prob = prepareTrainning(cropSize);

	// Define parameter for SVM 
	//struct svm_parameter param;     // set by parse_command_line
	param.svm_type = C_SVC;		// support type: C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR
	param.kernel_type = RBF;    // suport: LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED
	param.degree = 3;			// for poly 
	param.gamma = 0.5;			// for poly/rbf/sigmoid
	param.coef0 = 0;			// for poly/sigmoid
	param.nu = 0.5;				/* for NU_SVC, ONE_CLASS, and NU_SVR */
	param.cache_size = 100;		/* in MB */
	param.C = 1;				/* for C_SVC, EPSILON_SVR and NU_SVR */
	param.eps = 1e-3;			/* stopping criteria */
	param.p = 0.1;				/* for EPSILON_SVR */
	param.shrinking = 1;		/* use the shrinking heuristics */
	param.probability = 0;		 /* do probability estimates */
	param.nr_weight = 0;		/* for C_SVC */
	param.weight_label = NULL;	/* for C_SVC */
	param.weight = NULL;		/* for C_SVC */

	//Train model---------------------------------------------------------------------
	svm_model *model = svm_train(&prob, &param);

	// save the trained model 
	string file_name = "icon_Size" + to_string(cropSize) + "HOG_Size" +
						to_string(nwin_x) + "_B" + to_string(B)  + ".txt";
	svm_save_model(file_name.c_str(), model);	

	return model; 
}

/* 
* Test trainded svm with the training data 
*/

double iconRecog::testSVMTrainedData(int maxSize)
{
	int possitive = 0, falseResult = 0;
	// loading model 
	string file_name = "icon_Size" + to_string(maxSize) + "HOG_Size" +
						to_string(nwin_x) + "_B" + to_string(B) + ".txt";
	struct svm_model *trained_model = svm_load_model(file_name.c_str());
	//cout << "Loaded train model!" << endl;
	//string test_name; 

	// Testing
	struct svm_node* test_node;
	char FullFileName[100];
	for (int i = 0; i < classifyNum; i++)
	{

		for (int j = 0; j < trainPosDataNum; j++)
		{
			sprintf_s(FullFileName, "%s%d.bmp", cFileName[i].c_str(), j);
			test_node = prepareData(FullFileName, maxSize);
			
			int prediction = svm_predict(trained_model, test_node);
			//cout << "Prediction: " << i << ", " << j << " is: " << prediction << endl;
			possitive = possitive + ( (prediction == i) ? 1 : 0);
		}
	}
	possitive = possitive * 100.0 / (classifyNum * trainPosDataNum);
	//cout << "Possitive of for training data at crop size of " << maxSize << " is " << possitive * 100.0 / (classifyNum * trainPosDataNum) << "% ! "<< endl;
	//cout << "End test trained data!" << endl;
	return possitive; 
}

/*
* Test with real data 
*/
double iconRecog::testWithRealData(int maxSize)
{
	int possitive = 0, falseResult = 0;
	// loading model 
	string file_name = "icon_Size" + to_string(maxSize) + "HOG_Size" +
						to_string(nwin_x) + "_B" + to_string(B) + ".txt";
	struct svm_model *trained_model = svm_load_model(file_name.c_str());
	//cout << "Loaded train model!" << endl;

	struct svm_node* test_node;
	//testFileLabel
	for (unsigned int i = 0; i < testFileName.size(); i ++ )
	{
		test_node = prepareData(testFileName[i], maxSize);
		int prediction = svm_predict(trained_model, test_node);
		//cout << i << ", " << testFileName[i] << " is predicted as " << iconClass[prediction].c_str() << endl;
		possitive = possitive + ((prediction == testFileLabel[i]) ? 1 : 0);
	}
	possitive = possitive * 100.0 / (testFileName.size()); 
	//cout << "Possitive of for test data at crop size of " << maxSize << " is " << possitive << "% ! " << endl;
	//cout << "End test data!" << endl;
	return possitive; 
}

/*
* This function to evaluate parameter of SVM 
*/

void iconRecog::evalParamSVM()
{
	// varying the size and results 
	
	int iter = 0; 
	//FILE* pfile; 
	//pfile = fopen("evalSVM.txt", "a"); 
	for (int i = 16; i <= 128; i = i + 16)
	{		
		int crop_size = i;
		// train model 
		svm_model* model = trainWithLibSVM(crop_size);
	}
	string file_name = "icon_SVMPar_eval_Size" + to_string(nwin_x) + "_B" + to_string(B) + ".log"; 
	freopen(file_name.c_str(), "w", stdout);
	iter = iter + 1;
	for(int i = 16; i <= 128; i = i + 16)
	{
		int crop_size = i; 
		// test with train data 
		float train_pos = testSVMTrainedData(crop_size);
		float test_pos = testWithRealData(crop_size);
		cout << iter << ", Crop size " << crop_size << ", nWinx: "
			<< nwin_x << ", nWiny: " << nwin_y << ", B: " << B <<
			"Train Pos: " << train_pos << ", Test Pos: " << test_pos << endl;
		//fprintf(pfile, "%d, Crop size %d, winx: %d, winy: %d, B:%d, Train Pos: %6.3f, Test Pos: %6.3f \n", 
		//				iter, crop_size, nwin_x, nwin_y, B, train_pos, test_pos); 
	}
	//fclose(pfile);
	fclose(stdout);
}