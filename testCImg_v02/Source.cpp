#include "CImg.h"
#include "iconRecog.h"

using namespace cimg_library;

int main() {
	iconRecog icon;
	//	icon.HOGfeature2XML(); // 특징 추출

	//	icon.trainingBySVM(); // 트레이닝

	//	icon.testSVMTrainedData(); // 성능평가

	icon.testWithRealData(); // 실제 app 캡쳐화면으로 평가

	//CImg<unsigned char> image("icon_image/back/back/back0.bmp");// , visu(500, 400, 1, 3, 0);
	//const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
	//CImgDisplay main_disp(image, "Original image ");// draw_disp(visu, "Intensity profile");
	
	return 0;
}
