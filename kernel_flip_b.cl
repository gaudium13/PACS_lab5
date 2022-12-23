__kernel void flip_img(
	__global CImg img){
	
	// Load image file "image.jpg" at object img
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
}
