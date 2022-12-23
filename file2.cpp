#include "CImg.h"
#include <iostream>
using namespace cimg_library;
int main(){

  CImg<unsigned char> img("image.jpg");  // Load image file "image.jpg" at object img

  std::cout << "Image width: " << img.width() << "Image height: " << img.height() << "Number of slices: " << img.depth() << "Number of channels: " << img.spectrum() << std::endl;
  //dump some characteristics of the loaded image

  for(int i = 0; i < img.width(); i++)
  {
    img(i,img.height()/2,0,0)=0;
    img(i,img.height()/2,0,1)=0;
    img(i,img.height()/2,0,2)=255;
  }


  int i = 10;
  int j = 10;
  std::cout << std::hex << (int) img(i, j, 0, 0) << std::endl;  //print pixel value for channel 0 (red) 
  std::cout << std::hex << (int) img(i, j, 0, 1) << std::endl;  //print pixel value for channel 1 (green) 
  std::cout << std::hex << (int) img(i, j, 0, 2) << std::endl;  //print pixel value for channel 2 (blue) 
  
//  img.display("My first CImg code");             // Display the image in a display windowç
  CImg<unsigned char> img2("image.jpg");  // Load image file "image.jpg" at object img
  for (int i = 0; i < img.height(); i++){
	for (int j = 0; j < img.width()/2; j++){
		int red_t = img(j, i, 0, 0);
		int blue_t = img(j, i, 0, 2);
		int green_t = img(j, i, 0, 1);

		img(j, i, 0,0) = img(img.width() - j, i, 0,0);
		img(j, i, 0,2) = img(img.width() - j, i, 0,2);
		img(j, i, 0,1) = img(img.width() - j, i, 0,1);
		
		img(img.width() - j, i, 0,0) = red_t;
		img(img.width() - j, i, 0,2) = blue_t;
		img(img.width() - j, i, 0,1) = green_t;

	}
  }
	img.display("My first CImg code");
std::cout << "Done";
  return 0;

}
