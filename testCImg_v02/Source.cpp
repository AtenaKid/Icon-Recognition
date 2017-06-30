#include "CImg.h"
#include "iconRecog.h"

using namespace cimg_library;
// Global parmater for HOG Features

int main() {
	
	iconRecog icon;
	int cropSize = 32; 
	bool is_train = false; 
	if (is_train) 
	{
		cout << " Train LibSVM at size " << cropSize << "..." << endl;
		icon.trainWithLibSVM(cropSize); // Training model 		
	}

	icon.evalParamSVM();

	//freopen("icon.log", "w", stdout);
	//cout << " Test trained SVM with training data ... "<< endl;
	//icon.testSVMTrainedData(cropSize); // Test results with training model 
	//cout << " Test trained SVM with real data ... " << endl; 
	//icon.testWithRealData(cropSize); // 실제 app 캡쳐화면으로 평가
	//fclose(stdout); 

	return 0;
}
