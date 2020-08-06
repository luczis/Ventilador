// IHM-Respirador
// Projeto para uma interface Homem-Maquina desenvolvida a fim de ser utilizada para um respirador artificial,
// voltado para um ESP32. Desenvolvido em nome da UTFPR-Campus Apucarana.

#include "include/ESP32Lib.h"
#include "include/reset.h"
#include "include/Fonts/Font8x8.h"
#include "include/Fonts/Font6x8.h"

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 25;
const int vsyncPin = 26;
const int adc0pin = 33;
const int adc1pin = 34;
const int adc2pin = 35;
const int int0pin = 23;
const int encbut0pin = 15;
const int button0pin = 16;
const int button1pin = 17;
const int button2pin = 18;
const int encApin = 4;
const int encBpin = 5;
const int ledadv0=32;
const int ledadv1=21;
const int ledadv2=22;
const int buzzer = 13;
float valmax=0;
float val=0;
float valbuz=0;
int a=1;
int contreset=0;

unsigned long time0;
#define MIN_MIL 10  //O tempo minimo, em miliseg para atualizar a tela

VGA3BitI vga;
#include "include/Itens.h"

Graph grafico1, grafico2, grafico3;
Box caixa1, caixa2, caixa3, caixa4, caixa5, caixa6, caixa7, caixa8, caixa9, caixa10;
Button botao1, botao2, botao3, botao4, botao5;

TaskHandle_t DrawingTask;
TaskHandle_t EncoderTask;
TaskHandle_t SerialTask;

void DrawingTaskFunction(void* parameters);
void EncoderTaskFunction(void* parameters);
void SerialTaskFunction(void* parameters);
void ExternalInterrupt();
void EncoderInterrupt();

void setup()
{
  //Configura serial
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  
  //Utilizado para debug, mostra o que causou o ultimo reset do sistema
  Serial.println("CPU0 reset reason:");
  print_reset_reason(rtc_get_reset_reason(0));
  verbose_print_reset_reason(rtc_get_reset_reason(0));
  Serial.println("CPU1 reset reason:");
  print_reset_reason(rtc_get_reset_reason(1));
  verbose_print_reset_reason(rtc_get_reset_reason(1));
  
  //Configuracao de seed aleatoria
  randomSeed(analogRead(0));

  //Configuracao de pinos
  pinMode(encbut0pin, INPUT);
  pinMode(button0pin, INPUT);
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);
  pinMode(int0pin ,INPUT_PULLUP);
  pinMode(encApin ,INPUT_PULLDOWN);
  pinMode(encBpin ,INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(int0pin), ExternalInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(encApin), EncoderInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encBpin), EncoderInterrupt, CHANGE);

  //Configuracoes VGA
  vga.init(vga.MODE800x600, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
  vga.setFont(Font6x8);
  vga.setCursor(10, 10);
  vga.setTextColor(0x8);
  vga.setFrameBufferCount(1);

  //Desenho de itens fixos
  DrawBackground(800, 600, 0xc, 0xc);
  DrawLogo(690, 10);

  //Configuracoes dos graficos
  grafico1.ydataoff = 50;
  grafico2.ydataoff = 25;
  grafico3.ydataoff = 75;
  grafico3.ydatasize = 150;
  SetupGraph(&grafico1, 20, 25, 450, 150, "Sine Wave", 2);
  SetupGraph(&grafico2, 20, 225, 450, 150, "Sine Wave 2", 2);
  SetupGraph(&grafico3, 20, 425, 450, 150, "Foo Bar Baz Random", 2);

  //eixo x
  SetupLegenda(8, 25,"UM",0xf,0x0,1,1);
  SetupLegenda(8, 225,"UM",0xf,0x0,1,1);
  SetupLegenda(8, 425,"UM",0xf,0x0,1,1);
  //eixo y
  SetupLegenda(450, 180,"UM",0xf,0x0,1,0);
  SetupLegenda(450, 380,"UM",0xf,0x0,1,0);
  SetupLegenda(450, 580,"UM",0xf,0x0,1,0);

  //Configuracoes de caixas
  SetupBox(&caixa1, 490, 100, 95, 80, "FooBarBaz", 123, "Km/h");
  SetupBox(&caixa2, 490, 200, 95, 80, "FooBarBaz", 123, "Km/h");
  SetupBox(&caixa3, 490, 300, 95, 80, "FooBarBaz", 123, "Km/h");
  SetupBox(&caixa4, 490, 400, 95, 80, "FooBarBaz", 123, "Km/h");
  SetupBox(&caixa5, 490, 500, 95, 80, "FooBarBaz", 123, "Km/h");
  SetupBox(&caixa6, 595, 100, 95, 80, "ABCDE", -345, "mm3");
  SetupBox(&caixa7, 595, 200, 95, 80, "ABCDE", -345, "mm3");
  SetupBox(&caixa8, 595, 300, 95, 80, "ABCDE", -345, "mm3");
  SetupBox(&caixa9, 595, 400, 95, 80, "ABCDE", -345, "mm3");
  SetupBox(&caixa10, 595, 500, 95, 80, "ABCDE", -345, "mm3");

  //Configuracoes de botoes
  SetupButton(&botao1, 700, 100, 95, 80, "Sine Wave Banana", 123, "%");
  SetupButton(&botao2, 700, 200, 95, 80, "Sine Wave Banana", 123, "%");
  SetupButton(&botao3, 700, 300, 95, 80, "Sine Wave Banana", 123, "%");
  SetupButton(&botao4, 700, 400, 95, 80, "Sine Wave Banana", 123, "%");
  SetupButton(&botao5, 700, 500, 95, 80, "Sine Wave Banana", 123, "%");

  vga.setCursor(25, 580);
  //vga.setTextColor(0xb);
  vga.print("BG-Mister e LZ-Mister", 0xf, 0x0, 2);
  vga.setCursor(500, 25);
  vga.print("A gente constroi", 0xf, 0x0, 2);
  vga.setCursor(530, 45);
  vga.print("Deus leva", 0xf, 0x0, 2);

  //Definicao de tarefas
  xTaskCreatePinnedToCore(DrawingTaskFunction, "DrawingTask", 20000, NULL, PRIORITY_1, &DrawingTask, CORE_0);
  xTaskCreate(EncoderTaskFunction, "EncoderTask", 1000, NULL, PRIORITY_2, &EncoderTask);
  xTaskCreatePinnedToCore(SerialTaskFunction, "SerialTask", 1000, NULL, PRIORITY_1, &SerialTask, CORE_1);
}

unsigned int counter1 = 0;
float t = 0.0f;
unsigned int value1 = 0;
unsigned int adc0val = 0, adc1val = 0, adc2val = 0;
unsigned short decoder = 0, decoderPast = 0;

//Inicia tarefas quando ocorre interrupcoes
void ExternalInterrupt() {
  //vTaskResume(SerialTask);
}

//Tarefa utilizada para desenhar atualizacoes
void DrawingTaskFunction(void* parameters) {
  while (1) {
    time0 = millis();
    RedrawGraph(&grafico1, counter1, 50.0 * sin((float)counter1 * 6.28 / 450 + t));
    RedrawGraph(&grafico2, counter1, 70.0 * sin(((float)counter1 + 150.0) * 6.28 / 450 + t));
    RedrawGraph(&grafico3, counter1, (short)random(-50, 50));

    t > 6.28 ? t = 0 : t += 0.01;
    counter1 >= 450 ? counter1 = 0 : counter1++;

    if (counter1 % 10 == 0) {
      value1++;
      adc0val = analogRead(adc0pin);
      adc1val = analogRead(adc1pin);
      if(decoder!=decoderPast){
        decoderPast=decoder;
            botao1.is_selected = false;
            botao2.is_selected = false;
            botao3.is_selected = false;
            botao4.is_selected = false;
            botao5.is_selected = false;
        switch(decoder%5){
          case 0:
            botao1.is_selected = true;
            break;
          case 1:
            botao2.is_selected = true;
            break;
          case 2:
            botao3.is_selected = true;
            break;
          case 3:
            botao4.is_selected = true;
            break;
          case 4:
            botao5.is_selected = true;
            break;
        }
      }
      RedrawButton(&botao1, 0.000f);
      RedrawButton(&botao2, value1);
      RedrawButton(&botao3, value1);
      RedrawButton(&botao4, value1);
      RedrawButton(&botao5, value1);
      RedrawBox(&caixa1, counter1);
      RedrawBox(&caixa2, adc0val);
      RedrawBox(&caixa3, adc0val);
      RedrawBox(&caixa4, adc0val);
      RedrawBox(&caixa5, adc0val);
      RedrawBox(&caixa6, t);
      RedrawBox(&caixa7, adc1val);
      RedrawBox(&caixa8, adc1val);
      RedrawBox(&caixa9, t);
      RedrawBox(&caixa10, decoder);
    }
    showFPS();
    while (millis() - time0 < MIN_MIL)
      vTaskDelay(1/portTICK_PERIOD_MS);
  }
}


//Detecta interrupcoes do encoder
void EncoderInterrupt() {
  vTaskResume(EncoderTask);
}

uint8_t portVal = 0, mp = 0, i = 0, pastI = 0;
//Tarefa para decodificar o encoder incremental
void EncoderTaskFunction(void* parameters) {
  while(1) {
    portVal=(uint8_t)(*portInputRegister(digitalPinToPort(encApin)));
    i = portVal&0x30;
    if(i!=pastI)
    {
      pastI=i;
      switch(i){
        case 0x10:
          if(mp==0)
            mp=0b01;
          break;
        case 0x20:
          if(mp==0)
            mp=0b10;
          break;
        case 0x00:
          if(mp==1)
            decoder++;
          else if(mp==2)
            decoder--;
          mp=0;
          break;
      }
    }
    vTaskSuspend(NULL);
  }
}

//Tarefa somente de teste por enquanto
void SerialTaskFunction(void* parameters) {
  while(1) {
    //Serial.println((int)(*portInputRegister(digitalPinToPort(encApin))));
    //Serial.print('\t');
    //gpiomap++;
    //if(gpiomap>=(uint8_t*)0x3FF44FFF)
    //  vTaskSuspend(NULL);
    vTaskDelay(100/portTICK_PERIOD_MS);
    //vTaskSuspend(NULL);
  }
}

void loop() {
  //led
  LoopLed(button0pin,ledadv0);
  LoopLed(button1pin,ledadv1);
  LoopLed(button2pin,ledadv2);

  // buzzer
   digitalWrite(buzzer, LOW);
   Serial.println(button0pin);
    if(valmax<=val)
    {
      digitalWrite(buzzer, HIGH);
      delay(10);
    }
    if(valmax==0){
    digitalWrite(buzzer, HIGH);
    delay(1);
    }
    while(digitalRead(16)==HIGH && digitalRead(17)==HIGH){
    contreset++;
    if(contreset>=1500000){
    ESP.restart();
    }
    }
    contreset=0;
}
