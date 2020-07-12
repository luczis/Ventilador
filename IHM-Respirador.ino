// IHM-Respirador
// Projeto para uma interface Homem-Maquina desenvolvida a fim de ser utilizada para um respirador artificial,
// voltado para um ESP32. Desenvolvido em nome da UTFPR-Campus Apucarana.

#include "include/ESP32Lib.h"
#include "include/Itens.h"
#include "include/Ressources/Font8x8.h"

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

float t=0;
float u=0;

void setup()
{
	Serial.begin(9600);
	
	vga.init(vga.MODE800x600, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	//setting the font
	vga.setFont(Font8x8);
	//clearing with white background
	vga.clear(0xf);
	//text position
	vga.setCursor(10, 10);
	//black text color no background color
	vga.setTextColor(0x8);
	//show the remaining memory
	//vga.print("AJUSTES");
	vga.setFrameBufferCount(1);
	/*
	vga.setTextColor(0x9);
	vga.setCursor(725, 500);
	vga.print("t");
	vga.setCursor(175, 400);
	vga.printV("P");
	vga.show();
	vga.setCursor(725, 200);
	vga.print("t");
	vga.setCursor(175, 85);
	vga.printV("Vi");
	vga.setCursor(40, 20);
	*/

	DrawBackground(800,600,0xf,0xe);
	DrawLogo(690, 10);

	//eixos
	/*for(short i =0; i < 801; i++){
		for(short j =0; j < 250; j++){
			vga.dot(j,i,0x0);
		}
	}

	for(short i =0; i < 801; i++){
		vga.dot(250,i,0xf);
	}
	for(short i =0; i < 250; i++){
		vga.dot(i,150,0xf);
		vga.dot(i,300,0xf);
		vga.dot(i,450,0xf);
	}*/
}

void loop()
{
	/*float A=100; //amplitude
	float B=100; //frequencia
	short xmin=300;
	short xmax=750;
	short ymin=315;
	short ymax=575;
	short yoffset=130;
	short xoffset=130;
	float a=100; //amplitude
	float b=100; //frequencia
	short Xmin=300;
	short Xmax=750;
	short Ymin=25;
	short Ymax=285;
	short Yoffset=130;
	short Xoffset=130;

	for(short i = ymin; i < ymax; i++){
		vga.dot(xmin - 1,i,0xd);
	}
	for(short i = xmin; i < xmax; i++){
		vga.dot(i,ymin+yoffset,0xd);
	}

	for(short i = Ymin; i < Ymax; i++){
		vga.dot(Xmin - 1,i,0xd);
	}

	for(short i = Xmin; i < Xmax; i++){
		vga.dot(i,Ymin+Yoffset,0xd);
	}

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
		if(k!=yoffset+ymin)
		vga.dot(xmin+i,k,0x8);

		if(i>0)
		for(int k = Ymin; k< Ymax; k++)
		if(k!=Yoffset+Ymin)
		vga.dot(Xmin+i,k,0x8);

		vga.dot(xmin+i,yres,0x8+(int)(0x7*t/6.2830));
		vga.dot(Xmin+i,Yres,0x8+(int)(0x7*u/6.2830));
		}

		t+=0.1;
		u+=0.2;

		if(t>2*3.1415){
		t=0;}
		if(u>2*3.1415){
		u=0;
	}*/
}
