/*
create by rosie  2017.03.30
edited by Atena Nguyen 2017.05.31
*/
#include "CImg.h"
#include "iconRecog.h"
#include "utilz.hxx"
#include "debugUtils.h"
#include <iomanip>

using namespace cimg_library;
using namespace std; 

/*
* Test with real data 
*/
void iconRecog::testWithRealData()
{
	//for (unsigned int i = 0; i < testFileName.size(); i++)
	for (unsigned int i = 0; i < 1; i ++ )
	{
		CImg<unsigned char> image;
		//image.load( (testFileName[i]).c_str());
		image.load("back0.bmp");

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
		CImg<unsigned char> gray_crop = cropImg( gray_img, binary_img);

		// ============= Making Square Image ===================
		//int maxSize = max(width, height);
		int maxSize = 16; 
		CImg<float> gray_crop_resize = gray_crop.resize(maxSize, maxSize, 1, 1, 5);

		// ============= Display the image ===================== 
		// CImgDisplay src_disp(image, "Source image");
		// CImgDisplay gray_disp(gray_img, "Grayscale image");
		// CImgDisplay binary_disp(binary_img, "Binary image");
		// CImgDisplay gray_crop_disp(gray_crop, "Binary image");
		// CImgDisplay gray_resize_disp(gray_crop_resize, "Resized image");

		/*
		while (!src_disp.is_closed())
		{
			src_disp.wait();
		}
		*/

		// ============= Calculate HOG Features =====================
		int nwin_x = 3, nwin_y = 3;	// number of HOG window per image
		int B = 9;				// the number of histogram bins 
		vector<float> H(nwin_x*nwin_y*B, 0);
		H = HOG_Feature_Gen(gray_crop_resize, nwin_x, nwin_y, B);

		cout << "Calculated HoG features: " << endl; 
		int counter = 0;
		for (int i = 0; i < nwin_x * nwin_y * B; i++)
		{
			//cout.setf(ios::fixed); 
			cout << fixed << setprecision(3) << H[i] << '\t';
			counter = counter + 1; 
			if (counter == B)
			{
				counter = 0;
				cout << endl; 
			}
				 
		}
		cout << endl; 
		cout << "End!" << endl; 
	}
	//return 0;
}