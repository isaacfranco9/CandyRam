//console commands used to test camera and picture functions through imagemajick
#include <iostream>
#include <string>
using namespace std;		
		
		
int main(){
			
		string Now = "tset";
		system(("touch " + Now + ".jpg").c_str());
		system("libcamera-jpeg -n -o temp.jpg");
		system(("mv temp.jpg "+ Now + ".jpg").c_str());
		
		system(("convert " + Now + ".jpg -distort SRT 180 " + Now + ".jpg").c_str()); //rotates 180
		
		system(("convert " + Now + ".jpg -brightness-contrast 50% " + Now + ".jpg").c_str()); // brightness +50%
		
		system(("convert " + Now + ".jpg -colorspace LinearGray " + Now + ".jpg").c_str()); //Grayscale
		
		system()
		//place timestamp after all filters
		system(("/usr/bin/convert /home/ramsandrockets/testing/" + Now + ".jpg -pointsize 72 -fill black -annotate +50+1900 "+ Now + " /home/ramsandrockets/testing/" + Now + ".jpg").c_str());

		return 0;
	}
