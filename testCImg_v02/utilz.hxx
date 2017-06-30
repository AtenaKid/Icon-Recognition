#pragma once
#ifndef _utilz__
#define _utilz__
#include "CImg.h"
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace cimg_library;


/**
* Thresholding the image at a given threshold
*/
CImg<unsigned char> getBinary(CImg<unsigned char> gray_img, unsigned char threshold)
{
	// initialize a binary image version of img
	int width = gray_img._width, height = gray_img._height;
	CImg<unsigned char> binary_img(width, height, 1, 1, 0);

	// for every (x, y) pixel in gray_img
	cimg_forXY(gray_img, x, y)
	{
		int gray_value = gray_img(x, y, 0, 0);

		// COMPARE gray_value with threshold
		int binary_value;

		// gray_value > threshold: 255 (white)
		if (gray_value > threshold) binary_value = 255;
		// gray_value < threshold: 0 (black)
		else binary_value = 0;

		// assign binary_value to each of binary_img's pixels
		binary_img(x, y, 0, 0) = binary_value;
	}
	return binary_img;
}


/*
* Croping the icon (without considering the circle
*/
CImg<unsigned char> cropImg(CImg<unsigned char> img, CImg<unsigned char> binary_img)
{
	int width = img._width;
	int height = img._height;

	double curValue = 0;
	int iTop = 0, iBottom = height - 1;
	for (int i = 0; i < height - 1; i++)
	{
		curValue = binary_img.get_column(i).sum();
		//printf("i = %d, cur = %6.0f \n", i, curValue);

		// find the top value 
		if ((curValue > 0) && (iTop == 0))
		{
			iTop = i;
		}
		if (i > 0)
		{
			unsigned int old_value = (unsigned int)(binary_img.get_column(i - 1).sum());
			if ((old_value > 0) && (curValue == 0) && (iBottom == (height - 1)))
			{
				iBottom = i - 1;
				break;
			}
		}
	}
	//printf("The Top idx: %d, and Bottom idx: %d \n", iTop, iBottom);

	// Find the left and right 
	//double curValue = 0; 
	int iLeft = 0, iRight = width - 1;
	for (int i = 0; i < width - 1; i++)
	{
		curValue = binary_img.get_row(i).sum();
		//printf("i = %d, cur = %6.0f \n", i, curValue);

		// find the top value 
		if ((curValue > 0) && (iLeft == 0))
		{
			iLeft = i;
		}
		if (i > 0)
		{
			unsigned int old_value = unsigned int(binary_img.get_row(i - 1).sum());
			if ((old_value > 0) && (curValue == 0) && (iRight == (width - 1)))
			{
				iRight = i - 1;
				break;
			}
		}
	}
	//printf("The Left idx: %d, and Right idx: %d", iLeft, iRight);
	CImg<unsigned char> gray_crop(width, height, 1, 1, 0);
	gray_crop = img.get_crop(iTop, iLeft, 0, 0, iBottom, iRight, 0, 0);
	return gray_crop;
}


/**
* Function to generate histogram of image
*/
int *generate_histogram(cimg_library::CImg<unsigned char> img)
{
	int histogram[256];

	// initialize default values for histogram
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = 0;
	}

	// increment intensity for histogram
	for (int i = 0; i < img.height(); i++)
	{
		for (int j = 0; j < img.width(); j++)
		{
			int gray_value = img(j, i, 0, 0);
			histogram[gray_value]++;
		}
	}

	return histogram;
}


// convert gray CImg image to 1d array 
vector<unsigned int> CImgGray2Vect(cimg_library::CImg<> angles) {
	vector<unsigned int> tmp_angles(angles._width * angles._height, 0);
	for (unsigned int i = 0; i < angles._width; i++)
	{
		for (unsigned int j = 0; j < angles._height; j++)
		{
			int idx = i *angles._width + j;
			tmp_angles[idx] = unsigned int(angles(i, j, 0, 0));

			//cout << "Idx: " << idx << ", cur angles: " << tmp_angles[idx];
			//cout << ", cur magit = " << tmp_magnit[idx] << '\n';
		}
	}
	return tmp_angles;
}

// helper function that adds pixels to the border around the image
cimg_library::CImg<> add_padding_to_image(cimg_library::CImg<> image, int pixels) {
	cimg_library::CImg<int> padded_image(image.width() + 2 * pixels, image.height() + 2 * pixels, 1, image.spectrum());
	for (int channel = 0; channel < image.spectrum(); channel++) {
		for (int r = 0; r < image.height(); r++) {
			for (int c = 0; c < image.width(); c++) {
				padded_image(r + pixels, c + pixels, 0, channel) = (int)image(r, c, 0, channel);
			}
		}
	}
	return padded_image;
}


/**
* Find threshold value from the grayscale image's histogram
*/
int otsu_threshold(CImg<unsigned char> img)
{
	int * histogram = generate_histogram(img); // image histogram

	int total = img.width() * img.height(); // total pixels

	double sum = 0;

	int i;
	for (i = 0; i < 256; i++)
	{
		sum += i * histogram[i];
	}

	double sumB = 0;
	int wB = 0;
	int wF = 0;

	double var_max = 0;
	int threshold = 0;

	for (i = 0; i < 256; i++)
	{
		wB += histogram[i];
		if (wB == 0) continue;

		wF = total - wB;
		if (wF == 0) continue;

		sumB += (double)(i * histogram[i]);

		double mB = sumB / wB;
		double mF = (sum - sumB) / wF;

		double var_between = (double)wB * (double)wF * (mB - mF) * (mB - mF);

		if (var_between > var_max)
		{
			var_max = var_between;
			threshold = i;
		}
	}

	return threshold;
}


//applies the given filter to the image (1st param) inside the padding and produces an output image
cimg_library::CImg<> apply_sobel_filter(cimg_library::CImg<> image, float filter[3][3], int padding) {

	cimg_library::CImg<int> output(image.width(), image.height(), 1, image.spectrum());

	for (int channel = 0; channel < image.spectrum(); channel++) {

		//loop through each pixel INSIDE the padding
		for (int r = padding; r < image.height() - padding; r++) {
			for (int c = padding; c < image.width() - padding; c++) {

				float value =
					filter[0][0] * (float)image(r - 1, c - 1, 0, channel) +
					filter[0][1] * (float)image(r - 1, c, 0, channel) +
					filter[0][2] * (float)image(r - 1, c + 1, 0, channel) +
					filter[1][0] * (float)image(r, c - 1, 0, channel) +
					filter[1][1] * (float)image(r, c, 0, channel) +
					filter[1][2] * (float)image(r, c + 1, 0, channel) +
					filter[2][0] * (float)image(r + 1, c - 1, 0, channel) +
					filter[2][1] * (float)image(r + 1, c, 0, channel) +
					filter[2][2] * (float)image(r + 1, c + 1, 0, channel);

				//scale the value to [0, 255]; why it should be scaled? for display?

				//value = (value + 255) / 510 * 255;

				output(r, c, 0, channel) = (int)value;
			}
		}
	}
	return output;
}


// Given the x & y gradients, compute the magnitude
float get_magnitude(float ix, float iy) {
	double value = sqrt(pow(ix, 2) + pow(iy, 2));

	//scale the value from [0, 360.6] to [0, 255]
	//return float(255 * (value / 360.6));
	return value; 
}


// Computes the magnitude with magnitude = sqrt(Ix^2 + Iy^2). For color images, each channel is
// computed individually so the resulting image can be displayed as RGB

cimg_library::CImg<> compute_magnitude(cimg_library::CImg<> Ix, cimg_library::CImg<> Iy) {
	cimg_library::CImg<float> output(Ix.width(), Ix.height(), 1, Ix.spectrum());
	for (int channel = 0; channel < Ix.spectrum(); channel++) {
		for (int r = 0; r < Ix.height(); r++) {
			for (int c = 0; c < Ix.width(); c++) {
				output(r, c, channel) =  get_magnitude(Ix(r, c, channel), Iy(r, c, channel));
			}
		}
	}
	return output;
}


// color LUT using HSV to RGB conversion where H = theta (the angle from (b) properly transformed), S = 1.0, V= 1.0
cimg_library::CImg<> compute_orientation(cimg_library::CImg<> input_image, cimg_library::CImg<> Ix, cimg_library::CImg<> Iy) {
	//create an RGB image for displaying the orientation
	cimg_library::CImg<> output(input_image);



	int channel = 0;
	for (int r = 0; r < Ix.height(); r += 10) {
		for (int c = 0; c < Ix.width(); c += 10) {
			
			float Iy_value = Iy(r, c, channel);
			float Ix_value = Ix(r, c, channel);

			if (Iy_value == 0 && Ix_value == 0) {

			}
			else {
				//angle in radians
				float angle = float(atan2(Iy_value, Ix_value)); // * 180 / cimg_library::cimg::PI;

														 //ensure value is always positive
														 // if (angle < 0) angle += 360;

				unsigned char randomColor[3];
				randomColor[0] = rand() % 256;
				randomColor[1] = rand() % 256;
				randomColor[2] = rand() % 256;


				float magnitude = float(get_magnitude(Ix_value, Iy_value) / 255.0 * 10.0);

				int y2 = int(magnitude * sin(angle));
				int x2 = int( magnitude * cos(angle));

				//compute the arrow x2 y2 coordinates

				//maximum length of arrow is 10px

				//draw an arrow
				output.draw_arrow(r, c, r - y2, x2 + c, randomColor);
			}
		}
	}
	return output;
}

void detect_edges(cimg_library::CImg<> input_image) {
	/** add padding to input image **/

	int padding = 2;
	cimg_library::CImg<> padded_image = add_padding_to_image(input_image, padding);

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


	// /** Apply sobel filters to image **/
	cimg_library::CImg<> Ix = apply_sobel_filter(padded_image, G_x, padding);
	cimg_library::CImg<> Iy = apply_sobel_filter(padded_image, G_y, padding);

	Ix.display("Ix");
	Iy.display("Iy");


	// Calculate the edge magnitude
	cimg_library::CImg<> magnitude = compute_magnitude(Ix, Iy);
	magnitude.display("magnitude");

	//For grayscale images, compute the orientation
	if (input_image.spectrum() == 1) {
		cimg_library::CImg<> color_orientation_plot = compute_orientation(padded_image, Ix, Iy);
		color_orientation_plot.display("Orientation");
	}


}

/*
* Calculate Gradient image magnitude 
*/



/*
* HOG Features
*/
// Calculate HOG feature of the current image 
// C++ implementation  https://kr.mathworks.com/matlabcentral/fileexchange/28689-hog-descriptor-for-matlab

vector<float> HOG_Feature_Gen(CImg<float> im, int nwin_x = 3, int nwin_y = 3, int B = 9)
{
	//int nwin_x = 3, nwin_y = 3;	// number of HOG window per image
	//int B = 9;				// the number of histogram bins 
	int L = im._width, C = im._height;
	vector<float> H(nwin_x*nwin_y*B, 0);
	double m = sqrt(L / 2);
	unsigned int step_x = unsigned int(floor(C / (nwin_x + 1)));
	unsigned int step_y = unsigned int(floor(L / (nwin_y + 1)));
	int cont = 0;

	// get gradient image 
	CImgList<float> grad = im.get_gradient("xy", 3);

	// Construct gradient magnitude image 
	//vector<float> angles(L * C, 0);
	//vector<float> magnit(L * C, 0); 

	CImg <float> angles(L, C, 1, 1, 0);
	CImg <float> magnit(L, C, 1, 1, 0); 

	for (int i = 0; i < L; i++)
	{
		for (int j = 0; j < C; j++)
		{
			int idx = i * L + j;
			magnit(i, j, 0) = sqrt(grad[0](i, j) * grad[0](i, j) + grad[1](i, j) * grad[1](i, j));
			angles(i, j, 0) = abs(atan2(grad[0](i, j), grad[1](i, j))); // as unsigned perform the best
			//cout << magnit(i, j) << ',' << angles(i, j) << '\t';
		}
		//cout << endl;
	}
	
	// Convert them to matrix

	// Check the angle n 
	/*
	int cdfMin = 100; int cdfMax = -100;
	for (unsigned int i = 0; i < angles.size(); i++)
	{
		if (angles[i] < cdfMin)
			cdfMin = angles[i];
		if (angles[i] > cdfMax)
			cdfMax = angles[i];
	}
	cout << "Range of angles: [" << cdfMin << ", " << cdfMax << "]" << '\n';

	// Check the range of magniture 
	cdfMin = 100; cdfMax = -100;
	for (unsigned int i = 0; i < magnit.size(); i++)
	{
		if (magnit[i] < cdfMin)
			cdfMin = magnit[i];
		if (magnit[i] > cdfMax)
			cdfMax = magnit[i];
	}
	cout << "Range of Magnitude: [" << cdfMin << ", " << cdfMax << "]" << '\n';
	*/

	CImg<> angles2;			// temporal parameters
	CImg<> magnit2;
	for (int n = 0; n < nwin_x; n++)
	{
		for (int m = 0; m < nwin_y; m++)
		{
			cont++;
			angles2 = angles.get_crop(n*step_y , m*step_x , 0, 0, (n + 2) * step_y, (m + 2) * step_x, 0, 0);
			magnit2 = magnit.get_crop(n*step_y , m*step_x , 0, 0, (n + 2) * step_y, (m + 2) * step_x, 0, 0);

			int K = angles2._width * angles2._height;

			//assembling the histogram with 9 bins(range of 20 degrees per bin)
			int bin = -1;
			const double pi = 3.1412;
			vector<float> H2(B, 0);
			fill(H2.begin(), H2.end(), 0);

			for (float ang_lim = 0; ang_lim <= 3.1412/2; ang_lim += 3.1412 / (2 *(B - 1)))
			{
				bin = bin + 1;
				//cout << "Bin: " << bin << ", Angle Lim: " << ang_lim << endl;

				unsigned int idx = 0;
				for (unsigned int i = 0; i < angles2._height; i++)
				{
					for (unsigned int j = 0; j < angles2._width; j++)
					{
						idx = i*angles2._width + j;

						double cur_angle = angles2(i, j, 0, 0);

						//cout << "v_angles[" << idx << "] = " << cur_angle << endl;
						//cout << ", ang_lim = " << ang_lim << '\n';
						if (cur_angle < ang_lim)
						{
							angles2(i, j, 0, 0) = 1000;
							H2[bin] = H2[bin] + magnit2[idx];
							//cout << ",		H2[" << bin << "] = " << H2[bin] << ",		Magnit2[idx] = " << magnit2[idx] << endl;
							}
					}
				}
				//cout << ",		H2[" << bin << "] = " << H2[bin] << ",		Magnit2[idx] = " << magnit2[idx] << endl; 
			}

			// l2-norm
			double accum = 0.;
			for (int i = 0; i < bin; ++i) {
				accum += H2[i] * H2[i];
				//cout << H2[i] << '\t';
			}
			//cout << endl; 
			double normH2 = sqrt(accum);
			//cout << "Norm H2: " << normH2 << endl;

			// normalized H2
			for (int i = 0; i < bin; ++i)
			{
				H2[i] = float(H2[i] / (normH2 + 0.01)); 
				//cout << H2[i] << '\t';
				H[(cont - 1)*B + i] = H2[i];
			}
		}
	}

	//cout << "Finish HOG Features Extraction !!!" << endl;
	return H;
}



#endif