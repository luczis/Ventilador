//
// IHM-Respirador
// Projeto para uma interface Homem-Maquina desenvolvida a fim de ser utilizada para um respirador artificial,
// voltado para um ESP32. Desenvolvido em nome da UTFPR-Campus Apucarana.
//
// Colaboradores:
// - Bruno Gabriel da Silva 
// - Lucas Zischler
//

#include "include/ESP32Lib.h"
#include "include/reset.h"
#include "include/Fonts/Font8x8.h"
#include "include/Fonts/Font6x8.h"

//pin configuration
#include "include/pinout.h"

float valmax=0;
float val=0;
float valbuz=0;
int a=1;
int contreset=0;
int flag=1;
int flag1=0;

unsigned long time0;

//Parametros de inicializacao
float altura_paciente = 1.50f;
float peso_paciente = 70;

//Parametros do respirador
float FiO2 = .21f;	//0.21	-	1.0 	%
float PEEP = 0;		//0		-	20 		cm H2O
float volIns = 50;	//50	-	700 	mL
float flxIns = 0;	//0		-	70 		L/min
float freRes = 8;	//8		-	40		RPM
float tmpIns = 0.3f;//0.3	-	2.0		s
float I_E = 1;		//

//Alarmes
int alrm_pressao = 100;
int alrm_vazamento = 100;
int alrm_queda_rede = 100;

//Valores de incremento
const float inc_freRes = 0.1f;
const float inc_tmpIns = 0.01f;
const int inc_alrm_pressao = 1;
const int inc_alrm_vazamento = 1;
const int inc_alrm_queda_rede = 1;

//Valores temporarios dos botoes
float tmp_freRes = 8;
float tmp_tmpIns = 0.3f;
int tmp_alrm_pressao = 100;
int tmp_alrm_vazamento = 100;
int tmp_alrm_queda_rede = 100;

//Parametros globais
bool config_mode = 0;
bool config_mode_past = 1;
bool changing_mode = 0;
bool confirm_mode = 0;
bool discard_mode = 0;
bool button0press = 0;
bool button1press = 0;
bool button2press = 0;
bool booted = false;
bool startPhase = true;
bool shutDownFlag = false;
bool resetFlag = false;
bool sairFlag = false;
bool continuarFlag = false;

//configuracoes
#define _DEBUG					//Comente para desativar funcoes de debug
#define MIN_MIL 2  				//O tempo minimo, em centiseg para atualizar a tela
#define SHUT_TIME 20			//O tempo, em deciseg, para entrar na tela de shutdown
#define INFO_FRAMES_UPDATE 10	//Em quantos quadros os parametros são atualizados
#define GRAPH_SIZE 500			//Em quantos quadros os parametros são atualizados
#define LOGO_INICIAL			//Comente para desativar a logo inicial
#define TEMPO_LOGO 5			//Tempo que a logo fica ativa no inicio
#define _CLOCK					//Ativa o relogio

VGA3BitI vga;
#include "include/Itens.h"

Button startBotao1, startBotao2, startBotao3;
Button startBotao4, startBotao5;
Button startBotao6, startBotao7, startBotao8;
Button startBotao9, startBotao10;

Button resetBotao1, resetBotao2, resetBotao3;

Graph grafico1, grafico2, grafico3;
Box caixa1, caixa2, caixa3, caixa4, caixa5;
Button botao1, botao2, botao3, botao4, botao5;

TaskHandle_t DrawingTask;
TaskHandle_t StartTask;
TaskHandle_t SetupDrawTask;
TaskHandle_t EndTask;
TaskHandle_t BuzzerTask;
TaskHandle_t ResetTask;

void DrawingTaskFunction(void* parameters);
void StartTaskFunction(void* parameters);
void SetupDrawTaskFunction(void* parameters);
void EndTaskFunction(void* parameters);
void BuzzerTaskFunction(void* parameters);
void ResetTaskFunction(void* parameters);

void ExternalInterrupt();
void ButtonInterrupt();

#include "include/hardware_func.h"

void setup()
{
	//Configura serial
	Serial.begin(115200);
  
	//Configuracao de seed aleatoria
	randomSeed(analogRead(0));

	//Configuração do timer para interrupcoes
	timerInt = timerBegin(TIMER_0, 80, true);
	timerAttachInterrupt(timerInt, &int001s, true);
	timerAlarmWrite(timerInt, 10000, true);
	timerAlarmEnable(timerInt);

	//Configuração do timer para imagem
	timerFPS = timerBegin(TIMER_1, 80, true);

	//Utilizado para debug, mostra o que causou o ultimo reset do sistema
	Serial.println("CPU0 reset reason:");
	print_reset_reason(rtc_get_reset_reason(0));
	verbose_print_reset_reason(rtc_get_reset_reason(0));
	Serial.println("CPU1 reset reason:");
	print_reset_reason(rtc_get_reset_reason(1));
	verbose_print_reset_reason(rtc_get_reset_reason(1));

	//Configuracao de pinos
	pinMode(encbut0pin, INPUT);
	pinMode(button0pin, INPUT_PULLUP);
	pinMode(button1pin, INPUT_PULLUP);
	pinMode(button2pin, INPUT_PULLUP);
	pinMode(int0pin ,INPUT_PULLUP);
	pinMode(encApin ,INPUT_PULLDOWN);
	pinMode(encBpin ,INPUT_PULLDOWN);
	pinMode(buzzer, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(int0pin), ExternalInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(button0pin), ButtonInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(button1pin), ButtonInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(button2pin), ButtonInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(encApin), EncoderInterrupt, CHANGE);
	attachInterrupt(digitalPinToInterrupt(encBpin), EncoderInterrupt, CHANGE);

	//Configuracoes VGA
	vga.init(vga.MODE800x600, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	vga.setFont(Font6x8);
	vga.setCursor(10, 10);
	vga.setTextColor(0x8);
	vga.setFrameBufferCount(1);
	
	//Definicao de tarefas de setup
	xTaskCreate(EncoderTaskFunction, "EncoderTask", 1000, NULL, PRIORITY_2, &EncoderTask);
	xTaskCreate(ButtonTaskFunction, "ButtonTask", 1000, NULL, PRIORITY_2, &ButtonTask);
//	xTaskCreate(StartTaskFunction, "StartTask", 1000, NULL, PRIORITY_0, &StartTask);
	xTaskCreatePinnedToCore(StartTaskFunction, "StartTask", 1000, NULL, PRIORITY_0, &StartTask, CORE_0);
	xTaskCreate(BuzzerTaskFunction, "BuzzerTask", 1000, NULL, PRIORITY_0, &BuzzerTask);
//	xTaskCreatePinnedToCore(ResetTaskFunction, "ResetTask", 1000, NULL, PRIORITY_2, &ResetTask, CORE_0);
}
void(* resetFunc) (void) = 0;

unsigned int counter1 = 0;
float t = 0.0f;
unsigned int value1 = 0;
unsigned int adc0val = 0, adc1val = 0, adc2val = 0;

//Inicia tarefas quando ocorre interrupcoes
void ExternalInterrupt() {
}

//Tarefa de inicializacao do sistema
void StartTaskFunction(void* parameters) {
	booted = false;
	DrawBackground(800, 600, 0xc, 0xc);
#ifdef LOGO_INICIAL
	DrawLogo(0, -20, 8);
	while(timercount < TEMPO_LOGO*100)
			vTaskDelay(10/portTICK_PERIOD_MS);
	DrawBackground(800, 600, 0xc, 0xc);
#endif
	char startTextSize = 2;
	vga.setCursor(0, 80-vga.font->charHeight*startTextSize);
	vga.printCenter("Paciente", 200, 600, 0xf, 0x0, startTextSize);
	vga.setCursor(0, 200-vga.font->charHeight*startTextSize);
	vga.printCenter("Medidas do Paciente", 200, 600, 0xf, 0x0, startTextSize);
	vga.setCursor(0, 320-vga.font->charHeight*startTextSize);
	vga.printCenter("Modo de Operacao", 200, 600, 0xf, 0x0, startTextSize);

	startBotao1.m_text_size=2;
	startBotao2.m_text_size=2;
	startBotao3.m_text_size=2;
	SetupButton(&startBotao1, 200, 80, 100, 100, "", "Neonatal", "");
	SetupButton(&startBotao2, 330, 80, 140, 100, "", "Pediatrico", "");
	SetupButton(&startBotao3, 500, 80, 100, 100, "", "Adulto", "");
	SetupButton(&startBotao4, 200, 200, 175, 100, "Peso", 0, "Kg");
	SetupButton(&startBotao5, 425, 200, 175, 100, "Altura", 0, "m");
	SetupButton(&startBotao7, 350, 320, 100, 100, "Pressao Controlada", "PCV", "");
	SetupButton(&startBotao6, 200, 320, 100, 100, "Ventilacao Nao Invasiva", "NIV", "");
	SetupButton(&startBotao8, 500, 320, 100, 100, "Volume Controlado", "VCV", "");
	SetupButton(&startBotao9, 320, 460, 60, 60, "Cancela", "X", "");
	SetupButton(&startBotao10, 420, 460, 60, 60, "Confirma", "OK", "");

	startBotao1.config_on = false;
	startBotao2.config_on = false;
	startBotao3.config_on = false;
	startBotao4.config_on = false;
	startBotao5.config_on = false;
	startBotao6.config_on = false;
	startBotao7.config_on = false;
	startBotao8.config_on = false;
	startBotao9.config_on = false;
	startBotao10.config_on = false;
	startBotao10.config_on = false;
	startBotao9.border_no_config_color = 0xe;
	startBotao9.border_default_color = 0x8;
	startBotao9.border_selected_color = 0x9;
	startBotao10.border_no_config_color = 0xe;
	startBotao10.border_default_color = 0x8;
	startBotao10.border_selected_color = 0xa;

	byte currentRow = 0;
	byte pastRow = 1;
	const byte settedColor = 0x8;
	const byte notSetColor = 0xf;
	changing_mode = false;
	config_mode = false;
	while(startPhase){
		time0 = millis();
		
    	counter1 >= 450 ? counter1 = 0 : counter1++;

    	if (counter1 % INFO_FRAMES_UPDATE == 0) {
			if(buttonFlag!=0b111) {
				switch(buttonFlag){
					case 0b110:
						currentRow++;
						break;
					case 0b011:
						changing_mode ^= true;
						break;
				}
				if((currentRow%4)==3 && buttonFlag==0b101)
					config_mode = true;
				else
					config_mode = false;
				buttonFlag=0b111;
			}

			if(currentRow!=pastRow){
				pastRow=currentRow;
				startBotao1.is_selected ? startBotao1.border_no_config_color = settedColor : startBotao1.border_no_config_color = notSetColor;	
				startBotao2.is_selected ? startBotao2.border_no_config_color = settedColor : startBotao2.border_no_config_color = notSetColor;	
				startBotao3.is_selected ? startBotao3.border_no_config_color = settedColor : startBotao3.border_no_config_color = notSetColor;	
				startBotao4.is_selected ? startBotao4.border_no_config_color = settedColor : startBotao4.border_no_config_color = notSetColor;	
				startBotao5.is_selected ? startBotao5.border_no_config_color = settedColor : startBotao5.border_no_config_color = notSetColor;	
				startBotao6.is_selected ? startBotao6.border_no_config_color = settedColor : startBotao6.border_no_config_color = notSetColor;	
				startBotao7.is_selected ? startBotao7.border_no_config_color = settedColor : startBotao7.border_no_config_color = notSetColor;	
				startBotao8.is_selected ? startBotao8.border_no_config_color = settedColor : startBotao8.border_no_config_color = notSetColor;
				startBotao1.config_on = false;
				startBotao2.config_on = false;
				startBotao3.config_on = false;
				startBotao4.config_on = false;
				startBotao5.config_on = false;
				startBotao6.config_on = false;
				startBotao7.config_on = false;
				startBotao8.config_on = false;
				startBotao9.config_on = false;
				startBotao10.config_on = false;
				RedrawButton(&startBotao1, "");
				RedrawButton(&startBotao2, "");
				RedrawButton(&startBotao3, "");
				RedrawButton(&startBotao4, peso_paciente);
				RedrawButton(&startBotao5, altura_paciente);
				RedrawButton(&startBotao6, "");
				RedrawButton(&startBotao7, "");
				RedrawButton(&startBotao8, "");
				RedrawButton(&startBotao9, "");
				RedrawButton(&startBotao10, "");
			}
			switch(currentRow%4){
				case 0:
					startBotao1.config_on = true;
					startBotao2.config_on = true;
					startBotao3.config_on = true;
					switch(decoder%3){
						case 0:
							startBotao1.is_selected = true;
							startBotao2.is_selected = false;
							startBotao3.is_selected = false;
							break;
						case 1:
							startBotao1.is_selected = false;
							startBotao2.is_selected = true;
							startBotao3.is_selected = false;
							break;
						case 2:
							startBotao1.is_selected = false;
							startBotao2.is_selected = false;
							startBotao3.is_selected = true;
							break;
					}
					RedrawButton(&startBotao1, "");
					RedrawButton(&startBotao2, "");
					RedrawButton(&startBotao3, "");
					break;
				case 1:
					startBotao4.config_on = true;
					startBotao5.config_on = true;
					if(!changing_mode) {
						startBotao4.is_changing = false;
						startBotao5.is_changing = false;
						switch(decoder%2) {
							case 0:
								startBotao4.is_selected = true;
								startBotao5.is_selected = false;
								break;
							case 1:
								startBotao4.is_selected = false;
								startBotao5.is_selected = true;
								break;
						}
					}
					else {
						if(startBotao4.is_selected) {
							startBotao4.is_changing = true;
							startBotao5.is_changing = false;
						}
						else if(startBotao5.is_selected) {
							startBotao4.is_changing = false;
							startBotao5.is_changing = true;
						}
						if(decoder!=decoderPast) {
							if(startBotao4.is_selected)
								decoder>decoderPast? peso_paciente += 0.5 : peso_paciente -= 0.5;
							else if(startBotao5.is_selected)
								decoder>decoderPast? altura_paciente += 0.01 : altura_paciente -= 0.01;
							decoderPast=decoder;
						}
					}
					RedrawButton(&startBotao4, peso_paciente);
					RedrawButton(&startBotao5, altura_paciente);
					break;
				case 2:
					startBotao6.config_on = true;
					startBotao7.config_on = true;
					startBotao8.config_on = true;
					switch(decoder%3){
						case 0:
							startBotao6.is_selected = true;
							startBotao7.is_selected = false;
							startBotao8.is_selected = false;
							break;
						case 1:
							startBotao6.is_selected = false;
							startBotao7.is_selected = true;
							startBotao8.is_selected = false;
							break;
						case 2:
							startBotao6.is_selected = false;
							startBotao7.is_selected = false;
							startBotao8.is_selected = true;
							break;
					}
					RedrawButton(&startBotao6, "");
					RedrawButton(&startBotao7, "");
					RedrawButton(&startBotao8, "");
					break;
				case 3:
					startBotao9.config_on = true;
					startBotao10.config_on = true;
					switch(decoder%2){
						case 0:
							startBotao9.is_selected = true;
							startBotao10.is_selected = false;
							if(config_mode) {
								if(esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0) == ESP_OK)
									if(rtc_gpio_pullup_en(GPIO_NUM_12) == ESP_OK) {
										esp_deep_sleep_start();
										resetFunc();
									}
							}
							break;
						case 1:
							startBotao9.is_selected = false;
							startBotao10.is_selected = true;
							if(config_mode)
								startPhase=false;
							break;
					}
					RedrawButton(&startBotao9, "");
					RedrawButton(&startBotao10, "");
					break;
			}
		}
 
#ifdef _DEBUG
		showFPS();
#endif
		while (!drawingFlag)
			vTaskDelay(2/portTICK_PERIOD_MS);
		drawingFlag = false;
	}
	xTaskCreatePinnedToCore(SetupDrawTaskFunction, "SetupDrawTask", 10000, NULL, PRIORITY_0, &SetupDrawTask, CORE_0);
	
	booted = true;
	vTaskDelete(NULL);
}

//Tarefa de setup para tela inicial
void SetupDrawTaskFunction(void* parameters) {
	//Desenho de itens fixos
	DrawBackground(800, 600, 0xc, 0xc);
	DrawLogo(675, 10);

	//Configuracoes dos graficos
	grafico1.ydataoff = 75;
	grafico2.ydataoff = -20;
	grafico3.ydataoff = 75;
	grafico1.ydatasize = 150;
	grafico2.ydatasize = 150;
	grafico3.ydatasize = 150;
	grafico1.xdatasize = 100/MIN_MIL;
	grafico2.xdatasize = 100/MIN_MIL;
	grafico3.xdatasize = 100/MIN_MIL;
	SetupGraph(&grafico1, 25, 25, GRAPH_SIZE, 150, "Pressao", 2, true);
	SetupGraph(&grafico2, 25, 225, GRAPH_SIZE, 150, "Fluxo", 2, true);
	SetupGraph(&grafico3, 25, 425, GRAPH_SIZE, 150, "Volume", 2, true);

	//Configuracoes de caixas
	SetupBox(&caixa1, 550, 100, 100, 80, "FiO2", "", "%");
	SetupBox(&caixa2, 550, 200, 100, 80, "PEEP", "", "H2O");
	SetupBox(&caixa3, 550, 300, 100, 80, "Volume Inspirado", "", "mL");
	SetupBox(&caixa4, 550, 400, 100, 80, "Fluxo Inspiratorio", "", "L/min");
	SetupBox(&caixa5, 550, 500, 100, 80, "Relacao I:E", "", "");
	
	//Configuracoes de botoes
	SetupButton(&botao1, 675, 100, 100, 80, "Tempo Inspiratorio", "", "s");
	SetupButton(&botao2, 675, 200, 100, 80, "Frequencia Respiratoria", "", "RPM");
	SetupButton(&botao3, 675, 300, 100, 80, "Alarme\31Pressao Maxima\31em\31Vias", "", "");
	SetupButton(&botao4, 675, 400, 100, 80, "Alarme Vazamento", "", "");
	SetupButton(&botao5, 675, 500, 100, 80, "Alarme Queda de Rede de Gases", "", "");

	if(!continuarFlag)
		xTaskCreatePinnedToCore(DrawingTaskFunction, "DrawingTask", 10000, NULL, PRIORITY_1, &DrawingTask, CORE_0);
	else
		vTaskResume(DrawingTask);
	vTaskDelete(NULL);
}

//Tarefa utilizada para desenhar atualizacoes
void DrawingTaskFunction(void* parameters) {
	static unsigned short graphCnt = 0;
while(1){
	config_mode = false;
	changing_mode = false;
	while (!shutDownFlag){
		time0 = millis();

		//Atualizacao de graficos
		graphCnt >= GRAPH_SIZE ? graphCnt = 0 : graphCnt++;
		RedrawGraph(&grafico1, graphCnt, 50.0 * sin((float)graphCnt * 6.28 / 450 + t));
		RedrawGraph(&grafico2, graphCnt, 70.0 * sin(((float)graphCnt + 150.0) * 6.28 / 450 + t));
		RedrawGraph(&grafico3, graphCnt, (short)random(-50, 50));

		t > 6.28 ? t = 0 : t += 0.01;

		//Atualizacao da visualizacao dos parametros
		if (graphCnt % INFO_FRAMES_UPDATE == 0) {
			value1++;
			adc0val = analogRead(adc0pin);
			adc1val = analogRead(adc1pin);

			if(buttonFlag!=0b111) {
				confirm_mode = false;
				discard_mode = false;
				switch(buttonFlag){
					case 0b110:
						if(config_mode)
							confirm_mode = true;
						break;
					case 0b011:
						if(!config_mode)
							config_mode = true;
						else
							discard_mode = true;
						break;
					case 0b101:
						if(config_mode)
							changing_mode ^= true;
						else
							changing_mode = false;
						break;
				}
				buttonFlag=0b111;
			}

			//Atualizacao de botoes
			if(!config_mode) {
				if(config_mode!=config_mode_past) {
					config_mode_past = config_mode;
					botao1.config_on = config_mode;
					botao2.config_on = config_mode;
					botao3.config_on = config_mode;
					botao4.config_on = config_mode;
					botao5.config_on = config_mode;
				}
				RedrawButton(&botao1, tmpIns);
				RedrawButton(&botao2, freRes);
				RedrawButton(&botao3, alrm_pressao);
				RedrawButton(&botao4, alrm_vazamento);
				RedrawButton(&botao5, alrm_queda_rede);
				tmp_tmpIns = tmpIns;
				tmp_freRes = freRes;
				tmp_alrm_pressao = alrm_pressao;
				tmp_alrm_vazamento = alrm_vazamento;
				tmp_alrm_queda_rede = alrm_queda_rede;
			}
			else {
				if(!changing_mode) {
					if(confirm_mode) {
						tmpIns = tmp_tmpIns;
						freRes = tmp_freRes;
						alrm_pressao = tmp_alrm_pressao;
						alrm_vazamento = tmp_alrm_vazamento;
						alrm_queda_rede = tmp_alrm_queda_rede;
						confirm_mode = false;
						config_mode = false;
					}
					else if(discard_mode) {
						discard_mode = false;
						config_mode = false;
					}
				}
				static byte changing_button;
				if(config_mode!=config_mode_past) {
					config_mode_past = config_mode;
					botao1.config_on = config_mode;
					botao2.config_on = config_mode;
					botao3.config_on = config_mode;
					botao4.config_on = config_mode;
					botao5.config_on = config_mode;
				}
				if(changing_mode) {
					botao1.is_changing = false;
					botao2.is_changing = false;
					botao3.is_changing = false;
					botao4.is_changing = false;
					botao5.is_changing = false;
					switch(changing_button){
						case 0:
							botao1.is_changing = true;
							break;
						case 1:
							botao2.is_changing = true;
							break;
						case 2:
							botao3.is_changing = true;
							break;
						case 3:
							botao4.is_changing = true;
							break;
						case 4:
							botao5.is_changing = true;
							break;
					}
				}
				else {
					botao1.is_changing = false;
					botao2.is_changing = false;
					botao3.is_changing = false;
					botao4.is_changing = false;
					botao5.is_changing = false;
				}
				RedrawButton(&botao1, tmp_tmpIns);
				RedrawButton(&botao2, tmp_freRes);
				RedrawButton(&botao3, tmp_alrm_pressao);
				RedrawButton(&botao4, tmp_alrm_vazamento);
				RedrawButton(&botao5, tmp_alrm_queda_rede);

				if(decoder!=decoderPast) {
					if(!changing_mode){
						botao1.is_selected = false;
						botao2.is_selected = false;
						botao3.is_selected = false;
						botao4.is_selected = false;
						botao5.is_selected = false;
						changing_button = decoder%5;
						switch(changing_button){
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
					else {
						switch(changing_button) {
							case 0:
								decoder>decoderPast? tmp_tmpIns += inc_tmpIns : tmp_tmpIns -= inc_tmpIns;
								break;
							case 1:
								decoder>decoderPast? tmp_freRes += inc_freRes : tmp_freRes -= inc_freRes;
								break;
							case 2:
								decoder>decoderPast? tmp_alrm_pressao += inc_alrm_pressao : tmp_alrm_pressao -= inc_alrm_pressao;
								break;
							case 3:
								decoder>decoderPast? tmp_alrm_vazamento += inc_alrm_vazamento : tmp_alrm_vazamento -= inc_alrm_vazamento;
								break;
							case 4:
								decoder>decoderPast? tmp_alrm_queda_rede += inc_alrm_queda_rede : tmp_alrm_queda_rede -= inc_alrm_queda_rede;
								break;
						}
					}
					decoderPast=decoder;
					decoderPast=decoder;
				}
				if(confirm_mode) {
					changing_mode = false;
					confirm_mode = false;
				}
				else if(discard_mode) {
					changing_mode = false;
					discard_mode = false;
					switch(changing_button) {
						case 0:
							tmp_tmpIns = tmpIns;
							break;
						case 1:
							tmp_freRes = freRes;
							break;
						case 2:
							tmp_alrm_pressao = alrm_pressao;
							break;
						case 3:
							tmp_alrm_vazamento = alrm_vazamento;
							break;
						case 4:
							tmp_alrm_queda_rede = alrm_queda_rede;
							break;
					}
				}
			}

			FiO2=adc0val;
			PEEP=adc0val;
			volIns=adc0val;
			flxIns=adc0val;
			I_E=adc0val;

			//Atualizacao de caixas
			RedrawBox(&caixa1, shutTimerCount);
			RedrawBox(&caixa2, PEEP);
			RedrawBox(&caixa3, volIns);
			RedrawBox(&caixa4, flxIns);
			RedrawBox(&caixa5, I_E);
		}

#ifdef _DEBUG
		showFPS();
#endif
		while (!drawingFlag)
			vTaskDelay(2/portTICK_PERIOD_MS);
		drawingFlag = false;
	}
	shutDownFlag = false;
	booted = false;
	xTaskCreatePinnedToCore(ResetTaskFunction, "ResetTask", 1000, NULL, PRIORITY_0, &ResetTask, CORE_0);
	vTaskSuspend(NULL);
}
}

void BuzzerTaskFunction(void* parameters){
	digitalWrite(buzzer, LOW);
	if(valmax<=val) {
		digitalWrite(buzzer, HIGH);
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
	else if(valmax==0) {
		digitalWrite(buzzer, HIGH);
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

void ResetTaskFunction(void* parameters){
	bool shouldRedrawFlag = false;
	resetFlag = false;
	sairFlag = false;
	continuarFlag = false;
	resetBotao1.border_size = 10;
	resetBotao2.border_size = 10;
	resetBotao3.border_size = 10;
	resetBotao1.border_default_color = 0xa;
	resetBotao2.border_default_color = 0x9;
	resetBotao3.border_default_color = 0xc;
	while(!continuarFlag) {
		DrawBackground(800, 600, 0x0, 0x0);
		SetupButton(&resetBotao1, 50, 200, 200, 200, "", "VOLTAR", "");
		SetupButton(&resetBotao2, 300, 200, 200, 200, "", "DESLIGAR", "");
		SetupButton(&resetBotao3, 550, 200, 200, 200, "", "REINICIAR", "");
		vga.setCursor(0,100);
		vga.printCenter("Sistema de Desligamento:", 0, 800, 0xf, 0x0, 3);
		vTaskDelay(1000/portTICK_PERIOD_MS);
		buttonFlag = 0b111;

		while(!shouldRedrawFlag) {
			if(buttonFlag!=0b111) {
				switch(buttonFlag){
					case 0b011:
						shouldRedrawFlag = true;
						continuarFlag = true;
						break;
					case 0b110:
						shouldRedrawFlag = true;
						sairFlag = true;
						break;
					case 0b101:
						shouldRedrawFlag = true;
						resetFlag = true;
						break;
				}
				buttonFlag=0b111;
			}
			while (!drawingFlag)
				vTaskDelay(2/portTICK_PERIOD_MS);
			drawingFlag = false;
		}
		if(sairFlag) {
			DrawBackground(800, 600, 0x0, 0x0);
			SetupButton(&resetBotao1, 50, 200, 200, 200, "", "SIM", "");
			SetupButton(&resetBotao2, 550, 200, 200, 200, "", "NAO", "");
			vga.setCursor(0,100);
			vga.printCenter("TEM CERTEZA QUE DESEJA DESLIGAR?", 0, 800, 0x9, 0x0, 3);
			
			shutDownFlag = false;
			while(sairFlag) {
				if(buttonFlag!=0b111) {
					switch(buttonFlag){
						case 0b011:
							shutDownFlag = true;
							sairFlag = false;
							shouldRedrawFlag = true;
							break;
						case 0b110:
							sairFlag = false;
							shouldRedrawFlag = true;
							break;
					}
				buttonFlag=0b111;
				}
				while (!drawingFlag)
					vTaskDelay(2/portTICK_PERIOD_MS);
				drawingFlag = false;
			}
			continuarFlag = true;
			if(shutDownFlag){
				vTaskDelay(1000/portTICK_PERIOD_MS);
				if(rtc_gpio_pullup_en(GPIO_NUM_12) == ESP_OK) {
					if(esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0) == ESP_OK) {
						esp_deep_sleep_start();
						resetFunc();
					}
				}
			}
		}
		else if(resetFlag) {
			DrawBackground(800, 600, 0x0, 0x0);
			SetupButton(&resetBotao1, 50, 200, 200, 200, "", "SIM", "");
			SetupButton(&resetBotao2, 550, 200, 200, 200, "", "NAO", "");
			vga.setCursor(0,100);
			vga.printCenter("TEM CERTEZA QUE DESEJA REINICIAR?", 0, 800, 0xb, 0x0, 3);
			
			shutDownFlag = false;
			while(resetFlag) {
				if(buttonFlag!=0b111) {
					switch(buttonFlag){
						case 0b011:
							shutDownFlag = true;
							resetFlag = false;
							shouldRedrawFlag = true;
							break;
						case 0b110:
							resetFlag = false;
							shouldRedrawFlag = true;
							break;
					}
				buttonFlag=0b111;
				}
				while (!drawingFlag)
					vTaskDelay(2/portTICK_PERIOD_MS);
				drawingFlag = false;
			}
			continuarFlag = true;
			if(shutDownFlag){
				resetFunc();
			}
		}
	}
	xTaskCreatePinnedToCore(SetupDrawTaskFunction, "SetupDrawTask", 10000, NULL, PRIORITY_0, &SetupDrawTask, CORE_0);
	
	vTaskDelete(NULL);
}

void loop() {
  //led
  /*LoopLed(button0pin,ledadv0);
  LoopLed(button1pin,ledadv1);
  LoopLed(button2pin,ledadv2);

  // buzzer
   digitalWrite(buzzer, LOW);
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
    contreset=0;*/
}
