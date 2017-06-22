/*
create by rosie  2017.03.30
edited by Atena Nguyen 2017.05.31
*/
#include "CImg.h"
#include "iconRecog.h"
#include "utilz.hxx"
#include "debugUtils.h"

using namespace cimg_library;
using namespace std; 



/*
* Test with real data 
*/
void iconRecog::testWithRealData()
{
	for (unsigned int i = 0; i < testFileName.size(); i++)
	{
		CImg<unsigned char> image;
		image.load( (testFileName[i]).c_str());
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
		int maxSize = max(width, height);
		CImg<unsigned char> gray_crop_resize = gray_crop.resize(maxSize, maxSize, 1, 1, 5);

		// ============= Display the image ===================== 
		CImgDisplay src_disp(image, "Source image");
		CImgDisplay gray_disp(gray_img, "Grayscale image");
		CImgDisplay binary_disp(binary_img, "Binary image");
		CImgDisplay gray_crop_disp(gray_crop, "Binary image");
		CImgDisplay gray_resize_disp(gray_crop_resize, "Resized image");

		/*
		while (!src_disp.is_closed())
		{
			src_disp.wait();
		}
		*/
		// Calculate HOG feature of the current image 
		// C++ implementation  https://kr.mathworks.com/matlabcentral/fileexchange/28689-hog-descriptor-for-matlab
		int nwin_x = 3, nwin_y = 3;	// number of HOG window per image
		int B = 9;				// the number of histogram bins 
		int L = gray_crop_resize._width, C = gray_crop_resize._height; 
		vector<float> H(nwin_x*nwin_y*B, 0);
		double m = sqrt(L / 2); 
		unsigned int step_x = unsigned int (floor(C / (nwin_x + 1))); 
		unsigned int step_y = unsigned int (floor(L / (nwin_y + 1)));
		int cont = 0; 

		// Sobel detection 
		int padding = 0;
		CImg<> padded_image = add_padding_to_image(gray_crop_resize, padding);
		/** define sobel filters **/
		float G_x[3][3] = {
			{ 0.25, 0, -0.25 },
			{ 0.50, 0, -0.50 },
			{ 0.25, 0, -0.25 }
		};
		float G_y[3][3] = {
			{ 0.25,  0.50,  0.25 },
			{ 0,     0,     0 },
			{ -0.25, -0.50, -0.25 }
		};
		CImg<> grad_xr = apply_sobel_filter(padded_image, G_x, padding);
		CImg<> grad_yu = apply_sobel_filter(padded_image, G_y, padding);
		CImg<> angles = compute_orientation(padded_image, grad_xr, grad_yu);
		CImg<> magnit = compute_magnitude(grad_xr, grad_xr);

		// Convert them to matrix 
		vector<unsigned int> tmp_angles = CImgGray2Vect(angles);
		vector<unsigned int> tmp_magnit = CImgGray2Vect(angles);			

		// Check the angle
		//
		int cdfMin = -100; int cdfMax = 0; 
		for (unsigned int i = 0; i < tmp_angles.size(); i++)
		{
			if (tmp_angles[i] < cdfMin)
				cdfMin = tmp_angles[i];
			if (tmp_angles[i] > cdfMax)
				cdfMax = tmp_angles[i];
		}			

		cout << "Range of angles: [" << cdfMin << ", " << cdfMax << "]"<<'\n';

		CImg<> angles2; 
		CImg<> magnit2; 
		for (int n = 0; n < nwin_x ; n++)
		{
			for (int m = 0; m < nwin_y; m++)
			{
				cont++; 
				angles2 = angles.get_crop(n*step_y + 1, m*step_x + 1, 0, 0, (n + 2) * step_y, (m + 2) * step_x, 0, 0);
				magnit2 = magnit.get_crop(n*step_y + 1, m*step_x + 1, 0, 0, (n + 2) * step_y, (m + 2) * step_x, 0, 0);
				
				int K = angles2._width * angles2._height; 
				
				//assembling the histogram with 9 bins(range of 20 degrees per bin)
				int bin = 0;
				const double pi = 3.1412; 
				vector<float> H2(B, 0);
				fill(H2.begin(), H2.end(), 0);

				for (unsigned int ang_lim = 0; ang_lim <= 360; ang_lim += 360/B)
				{
					++bin;
					unsigned int idx = 0;
					for (unsigned int i = 0; i < angles2._height; i++)
					{
						for (unsigned int j = 0; j < angles2._width; j++)
						{
							idx = (i - 1)*angles2._width + j;
							
							double cur_angle = angles2(i, j, 0, 0);
							
							//cout << "v_angles[" << idx << "] = " << cur_angle;
							//cout << ", ang_lim = " << ang_lim << '\n';
							if (cur_angle < ang_lim)
							{
								angles2(i, j, 0, 0) = 1000;
								H2[bin] = H2[bin] + magnit2[idx];
							}
						}
					}

				}

				// l2-norm
				double accum = 0.;
				for (int i = 0; i < bin; ++i) {
					accum += H2[i] * H2[i];
				}
				double normH2 = sqrt(accum);
				// normalized H2
				for (int i = 0; i < bin; ++i)
				{
					H2[i] = float(H2[i] / (normH2 + 0.001));
					H[(cont - 1)*B + i] = H2[i];
				}
			}
		}	

		cout << "Finish!!!";

	}
	//return 0;
}