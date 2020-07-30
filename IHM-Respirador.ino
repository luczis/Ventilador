// IHM-Respirador
// Projeto para uma interface Homem-Maquina desenvolvida a fim de ser utilizada para um respirador artificial,
// voltado para um ESP32. Desenvolvido em nome da UTFPR-Campus Apucarana.

#include "include/ESP32Lib.h"
#include "include/Fonts/Font8x8.h"
#include "include/Fonts/Font6x8.h"

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

unsigned long time0;
#define MIN_MIL 10  //O tempo minimo, em miliseg para atualizar a tela

VGA3BitI vga;
#include "include/Itens.h"

Graph grafico1, grafico2, grafico3;
Box caixa1, caixa2, caixa3, caixa4, caixa5, caixa6, caixa7, caixa8, caixa9, caixa10;
Button botao1, botao2, botao3, botao4, botao5;

TaskHandle_t DrawingTask;
TaskHandle_t SerialTask;

void DrawingTaskFunction(void* parameters);
void SerialTaskFunction(void* parameters);

void setup()
{
  //Configuracoes Diversas
	// Serial.begin(115200);
	randomSeed(analogRead(0));

  //Configuracoes VGA
	vga.init(vga.MODE800x600, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	vga.setFont(Font6x8);
	vga.setTextColor(0x8);
	vga.setFrameBufferCount(1);

  //Desenho de itens fixos
	DrawBackground(800,600,0xc,0xc);
	DrawLogo(690, 10);

  //Configuracoes dos graficos
	grafico1.ydataoff=50;
	grafico2.ydataoff=25;
	grafico3.ydataoff=75;
  grafico3.ydatasize=150;
	SetupGraph(&grafico1,20,25,450,150, "Sine Wave", 2);
	SetupGraph(&grafico2,20,225,450,150, "Sine Wave 2", 2);
	SetupGraph(&grafico3,20,425,450,150, "Foo Bar Baz Random", 2);
  
  //eixo x
  SetupLegenda(8, 25,"UM",0xf,0x0,1,1);
  SetupLegenda(8, 225,"UM",0xf,0x0,1,1);
  SetupLegenda(8, 425,"UM",0xf,0x0,1,1);
  //eixo y
  SetupLegenda(450, 180,"UM",0xf,0x0,1,0);
  SetupLegenda(450, 380,"UM",0xf,0x0,1,0);
  SetupLegenda(450, 580,"UM",0xf,0x0,1,0);
  

  //Configuracoes de caixas
  SetupBox(&caixa1, 490, 100, 95, 80, "FooBarBaz",123,"Km/h");
  SetupBox(&caixa2, 490, 200, 95, 80, "FooBarBaz",123,"Km/h");
  SetupBox(&caixa3, 490, 300, 95, 80, "FooBarBaz",123,"Km/h");
  SetupBox(&caixa4, 490, 400, 95, 80, "FooBarBaz",123,"Km/h");
  SetupBox(&caixa5, 490, 500, 95, 80, "FooBarBaz",123,"Km/h");
  SetupBox(&caixa6, 595, 100, 95, 80, "ABCDE",-345,"mm3");
  SetupBox(&caixa7, 595, 200, 95, 80, "ABCDE",-345,"mm3");
  SetupBox(&caixa8, 595, 300, 95, 80, "ABCDE",-345,"mm3");
  SetupBox(&caixa9, 595, 400, 95, 80, "ABCDE",-345,"mm3");
  SetupBox(&caixa10, 595, 500, 95, 80, "ABCDE",-345,"mm3");
  
  //Configuracoes de botoes
  SetupButton(&botao1, 700, 100, 95, 80, "Sine Wave Banana",123,"%");
  SetupButton(&botao2, 700, 200, 95, 80, "Sine Wave Banana",123,"%");
  SetupButton(&botao3, 700, 300, 95, 80, "Sine Wave Banana",123,"%");
  SetupButton(&botao4, 700, 400, 95, 80, "Sine Wave Banana",123,"%");
  SetupButton(&botao5, 700, 500, 95, 80, "Sine Wave Banana",123,"%");

  vga.setCursor(25,580);
  //vga.setTextColor(0xb);
  vga.print("BG-Mister e LZ-Mister",0xf,0x0,2);
  vga.setCursor(500,25);
  vga.print("A gente constroi,",0xf,0x0,2);
  vga.setCursor(530,45);
  vga.print("Deus que leva",0xf,0x0,2);

  //Definicao de tarefas
  xTaskCreatePinnedToCore(DrawingTaskFunction, "DrawingTask", 20000, NULL, PRIORITY_1, &DrawingTask, CORE_0);
  //xTaskCreatePinnedToCore(SerialTaskFunction, "SerialTask", 10000, NULL, PRIORITY_0, &SerialTask, CORE_1);
}

unsigned int counter1 = 0;
float t=0.0f;
unsigned int value1 = 0;

//Tarefa utilizada para desenhar atualizacoes
void DrawingTaskFunction(void* parameters){
  while(1) {
    time0 = millis();
	  RedrawGraph(&grafico1,counter1,50.0*sin((float)counter1*6.28/450+t));
  	RedrawGraph(&grafico2,counter1,70.0*sin(((float)counter1+150.0)*6.28/450+t));
  	RedrawGraph(&grafico3,counter1,(short)random(-50,50));

	  t>6.28 ? t=0 : t+=0.01;
    counter1>=450 ? counter1=0 : counter1++;

    if(counter1%10==0){
        value1++;
        botao1.is_selected = random(2);
        botao2.is_selected = random(2);
        botao3.is_selected = random(2);
        botao4.is_selected = random(2);
        botao5.is_selected = random(2);
        RedrawButton(&botao1, 0.000f);
        RedrawButton(&botao2, value1);
        RedrawButton(&botao3, value1);
        RedrawButton(&botao4, value1);
        RedrawButton(&botao5, value1);
        RedrawBox(&caixa1, counter1);
        RedrawBox(&caixa2, t);
        RedrawBox(&caixa3, t);
        RedrawBox(&caixa4, t);
        RedrawBox(&caixa5, t);
        RedrawBox(&caixa6, t);
        RedrawBox(&caixa7, t);
        RedrawBox(&caixa8, t);
        RedrawBox(&caixa9, t);
        RedrawBox(&caixa10, t);
    }
    showFPS();
    while(millis()-time0 < MIN_MIL)
      delay(1);
  }
}

//Tarefa somente de teste por enquanto
void SerialTaskFunction(void* parameters){
  while(1) {
  delay(1000);
  
  //Serial.print(xPortGetCoreID());
  }
}

void loop(){}
