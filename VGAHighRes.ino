//This example shows a high VGA resolution 3Bit mode
//The VGA3BitI implementation uses the I²S interrupt to transcode a dense frame buffer to the needed
//8Bit/sample. Using the dense frame buffer allows to fit the big frame buffer in memory at the price of
//a lot cpu performance.
//You need to connect a VGA screen cable to the pins specified below.
//cc by-sa 4.0 license
//bitluni

//including the needed header
#include <ESP32Lib.h>
#include <Ressources/Font8x8.h>

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

//VGA Device using an interrupt to unpack the pixels from 4bit to 8bit for the I²S
//This takes some CPU time in the background but is able to fit a frame buffer in the memory
VGA3BitI vga;

float t=0;
f loat u=0;
void setup()
{
	Serial.begin(9600);
	//initializing the graphics mode
	vga.init(vga.MODE800x600, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	//setting the font
	vga.setFont(Font8x8);
	vga.clear(vga.RGB(0x000000));	//Fundo preto
	vga.setCursor(10, 10);
	vga.setTextColor(0xffffff);	//Texto branco
	//show the remaining memory
	vga.print("Trabalho brabo");
	vga.setFrameBufferCount(1);
}

void loop() {
	float A=100; //amplitude
	float B=100; //frequencia
	short xmin=200;
	short xmax=700;
	short ymin=400;
	short ymax=800;
	short yoffset=100;
	short xoffset=100;
	float a=100; //amplitude
	float b=100; //frequencia
	short Xmin=200;
	short Xmax=700;
	short Ymin=100;
	short Ymax=350;
	short Yoffset=100;
	short Xoffset=100;
  
	for(short i = 0; i < xmax-xmin && i < Xmax-Xmin ; i++){
		int yres=ymin+yoffset+(int)(-A*sin(t+(float)(i+xoffset)/B));
		int Yres=Ymin+Yoffset+(int)(-a*sin(u+(float)(i+Xoffset)/b));
		if(yres>ymax)
			yres=ymax;
		if(yres<ymin)
			yres=ymin;
		if(Yres>Ymax)
			Yres=Ymax;
		if(Yres<Ymin)
			Yres=Ymin;
		if(i>0)
			for(int k = ymin; k< ymax; k++)
				if(k!=yoffset-1)
					if(vga.get(xmin+i,k) == 0)
						vga.dot(xmin+i,k,0);
					vga.dot(xmin+i,yres,(int)(0xffffff*(t/(2*3.1415))));
		for(short i = ymin; i < ymax; i++){
  			vga.dot(xmin - 1,i,0xffffff);
		}
  		for(short i = xmin; i < xmax; i++){
  			vga.dot(i,ymin+yoffset,0xffffff);
		}
		t+=0.01;

		if(t>2*3.1415){
  			t=0;}
  		for(int k = Ymin; k< Ymax; k++)
		if(k!=Yoffset-1)
		if(vga.get(Ymin+i,k) == 0)
		vga.dot(Xmin+i,k,0);
		vga.dot(Xmin+i,Yres,(int)(0xffffff*(u/(2*3.1415))));
		for(short i = Ymin; i < Ymax; i++){
  			vga.dot(Xmin - 1,i,0xffffff);
		}
  		for(short i = Xmin; i < Xmax; i++){
  			vga.dot(i,Ymin+Yoffset,0xffffff);
		}
		u+=0.01;
		if(u>2*3.1415){
  			u=0;
		}
	}
 	vga.setCursor(725, 500);
  	vga.print("t");
  	vga.setCursor(175, 400);
  	vga.printV("P");
  	vga.show();
  	vga.setCursor(725, 200);
  	vga.print("t");
  	vga.setCursor(175, 85);
	vga.printV("Vi");

}
