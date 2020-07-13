// IHM-Respirador
// Projeto para uma interface Homem-Maquina desenvolvida a fim de ser utilizada para um respirador artificial,
// voltado para um ESP32. Desenvolvido em nome da UTFPR-Campus Apucarana.

#include "ESP32Lib.h"
#include "Ressources/Font8x8.h"

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

float t=0;
float u=0;

VGA3BitI vga;
#include "Itens.h"

Graph grafico1, grafico2, grafico3;

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

	print("Poe um menu",2,false);
	print("E tela toche",4,true);
	
	SetupGraph(&grafico1,25,25,450,150);
	SetupGraph(&grafico2,25,175,450,150);
	SetupGraph(&grafico3,25,225,450,150);

	DrawBackground(800,600,0xf,0xe);
	DrawLogo(690, 10);

}

unsigned int counter1 = 0, counter2 = 150, counter3 = 300;
float t=0.0f;

void loop()
{
	RedrawGraph(&grafico1,counter,sin((float)counter1/450+t);
	RedrawGraph(&grafico2,counter,sin((float)counter2/450+t);
	RedrawGraph(&grafico3,counter,sin((float)counter3/450+t);

	t=t%6.28f+0.01;
	counter1=counter1%450+1;
	counter2=counter2%450+1;
	counter3=counter3%450+1;
}
