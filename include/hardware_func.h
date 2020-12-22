//Funcoes relacionadas aos botoes
bool shut_but_flag = false;
byte buttonFlag = 0;
uint32_t port32val = 0;
TaskHandle_t ButtonTask;
TaskHandle_t SwitchTask;

//Detecta interrupcoes do botao
unsigned long lastButInt = 0;
void ButtonInterrupt() {
	if(millis()-lastButInt>100) {
		lastButInt=millis();
		vTaskResume(ButtonTask);
	}
}

//Tarefa para botoes
void ButtonTaskFunction(void* parameters) {
	while(1) {
		port32val=(uint32_t)(*portInputRegister(digitalPinToPort(button0pin)));
		buttonFlag = ((port32val&0x50000)>>16) | 0b010;
		vTaskSuspend(NULL);
	}
}

//Detecta interrupcoes do switch
unsigned long lastSwiInt = 0;
void SwitchInterrupt() {
	if(millis()-lastSwiInt>100) {
		lastButInt=millis();
		vTaskResume(SwitchTask);
	}
}

//Tarefa para switch
void SwitchTaskFunction(void* parameters) {
	while(1) {
		buttonFlag = 0b101;
		vTaskSuspend(NULL);
	}
}

//Funcoes relacionadas ao timer
hw_timer_t * timerInt = NULL;
hw_timer_t * timerRes = NULL;

bool drawingFlag = false;
unsigned int timercount = 0;
int shutTimerCount = 0;
void int0_01s() {
	if(timercount%10==0 && !shutDownFlag) {
		port32val=(uint32_t)(*portInputRegister(digitalPinToPort(button0pin)));
		if(!(0x11000&port32val))
			shut_but_flag = true;
		else
			shut_but_flag = false;

		shut_but_flag ? shutTimerCount++ : shutTimerCount--;
		if(shutTimerCount > SHUT_TIME) {
			shutTimerCount = 0;
			shutDownFlag = true;
		}
		else if(shutTimerCount<=0)
			shutTimerCount = 0;
	}

	if(timercount >= 10000)
		timercount = 0;
	if(timercount%MIN_MIL == 0)
		drawingFlag = true;
	timercount++;
}

unsigned int respirador_timercounter = 0;
void int0_001s() {
	if(mainScreen) {
		vTaskResume(RespiradorTask);
	}

	if(respirador_timercounter >= 10000)
		respirador_timercounter = 0;
	respirador_timercounter++;
}

//Funcoes relacionadas ao encoder
unsigned short decoder = 0, decoderPast = 0;
TaskHandle_t EncoderTask;

//Detecta interrupcoes do encoder
unsigned long lastEncInt = 0;
void EncoderInterrupt() {
	if(micros()-lastEncInt>1000) {
		lastEncInt=micros();
		vTaskResume(EncoderTask);
	}
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
