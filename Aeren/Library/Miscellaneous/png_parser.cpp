#include <bits/stdc++.h>

using namespace std;


#include "lodepng.h"
struct png_parser{
	static tuple<vector<unsigned char>, unsigned int, unsigned int>
	decode(const char *file){
		vector<unsigned char> image;
		unsigned int width, height;
		if(auto error = lodepng::decode(image, width, height, file)){
			cerr << "Decoder Error " << error << ": " << lodepng_error_text(error) << endl;
			exit(1);
		}
		return {image, width, height};
	}
	static void encode(
		const vector<unsigned char> &image,
		unsigned int width,
		unsigned int height,
		const char *file
	){
		if(auto error = lodepng::encode(file, image, width, height)){
			cerr << "Encoder Error " << error << ": "<< lodepng_error_text(error) << endl;
			exit(2);
		}
	}
};
