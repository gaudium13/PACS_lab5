__kernel void rotate_img(
	__global unsigned char *red_in,
  	__global unsigned char *green_in,
  	__global unsigned char *blue_in,
	__global unsigned char *red_out,
  	__global unsigned char *green_out,
  	__global unsigned char *blue_out,
	unsigned int width,
	unsigned int height,
	unsigned int angle){

	for (unsigned int x_1 = 0; x_1 < height; x_1++){
		for (unsigned int y_1 = 0; y_1 < width; y_1++){

			unsigned int x_2 = (unsigned int)((float)cosd(angle)*(x_1-height/2) - (float)sind(angle)*(y_1-width/2) + height/2);
			unsigned int y_2 = (unsigned int)((float)sind(angle)*(x_1-height/2) + (float)cosd(angle)*(y_1-width/2) + width/2);

			if x_2 >= 0 & y_2 >= 0 & x_2 < height & y_2 < width
			{
				red_out[x_2*width+y_2] = red_in[x_1*width+y_1];
			
				green_out[x_2*width+y_2] = green_in[x_1*width+y_1];
			
				blue_out[x_2*width+y_2] = blue_in[x_1*width+y_1];
			}
		}
	}
	
}
