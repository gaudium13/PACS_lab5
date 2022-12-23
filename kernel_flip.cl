__kernel void flip_img(
	__global unsigned char *red,
  	__global unsigned char *green,
  	__global unsigned char *blue,
	unsigned int width,
	unsigned int height){

	for (unsigned int i = 0; i < height; i++){
		for (unsigned int j = 0; j < width/2; j++){
			unsigned char temp_red = red[i*width+j];
			unsigned char temp_green = green[i*width+j];
			unsigned char temp_blue = blue[i*width+j];

			red[i*width+j] = red[(i+1)*width-(j+1)];
			red[(i+1)*width-(j+1)] = temp_red;

			green[i*width+j] = green[(i+1)*width-(j+1)];
			green[(i+1)*width-(j+1)] = temp_green;

			blue[i*width+j] = blue[(i+1)*width-(j+1)];
			blue[(i+1)*width-(j+1)] = temp_blue;

			//blue[i*width+j] = 255;
			//blue[(i+1)*width-(j+1)] = 0;
		}
	}
	
}
