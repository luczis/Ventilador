//
//	O codigo deste arquivo foi originalmente desenvolvido pelo Dr. Vinicius Dario Bacon, pequenas modificações foram realizadas para adaptacao ao codigo ja implementado.
//	Todos os creditos, direitos e meritos do codigo presente neste arquivo devem ser redirecionados aos desenvolvedores do codigo base.
//	
//	Codigo base:
//		- Dr. Vinicius Dario Bacon
//	
//	Alteracoes:
//		- Lucas Zischler
//

#include "tabelavazao.h"

//Parte para calculos e atuacoes do respirador
TaskHandle_t RespiradorTask;

unsigned int tmpIns_counter = (int)(tmpIns*1000.0f), tmpCmp_counter = (int)(tmpCmp*1000.0f);	
unsigned int respirador_counter = 0;
unsigned short adc_sample_count = 0;
unsigned int tmp_adc0val = 0, tmp_adc1val = 0, tmp_adc2val = 0;
bool changeCycle = false;
void RespiradorTaskFunction(void* parameters) {
	while(1) {
		vTaskSuspend(NULL);
	
		// Incrementa o contador do respirador e reseta no final do ciclo
		respirador_counter++;
    if (respirador_counter>(int)(tmpCmp*1000.0f))
   //if (respirador_counter>1000)
			respirador_counter = 0;

 		// Alterna o estado das valvulas
		if(respiradorOn) {
			if (respirador_counter<(int)(tmpIns*1000.0f)){
      //if(respirador_counter<500){
				digitalWrite(valve0pin,HIGH);
				digitalWrite(valve1pin,HIGH);
				digitalWrite(valve2pin,HIGH);
				//digitalWrite(buzzer,HIGH);
			}
			else{
				digitalWrite(valve0pin,LOW);
				digitalWrite(valve1pin,LOW);
        digitalWrite(valve2pin,LOW);
				//digitalWrite(buzzer,LOW);
			}
		}

    // Verifica os alarmes
    if( (adc0val>=alrm_pressao) ||
        (adc1val>=alrm_vazamento) ||
        (adc2val>=alrm_queda_rede))
      ledcWrite(buzzer_channel, 512);
    else
      ledcWrite(buzzer_channel, 0);

		// Realiza media dos ADCs
		tmp_adc0val += analogRead(adc0pin);
		tmp_adc1val += analogRead(adc1pin);
		tmp_adc2val += analogRead(adc2pin);

		adc_sample_count++;
		if(adc_sample_count >= ADC_SAMPLES) {
			adc0val = tmp_adc0val/ADC_SAMPLES;
			adc1val = tmp_adc1val/ADC_SAMPLES;
			adc2val = tmp_adc2val/ADC_SAMPLES;
			tmp_adc0val = 0;
			tmp_adc1val = 0;
			tmp_adc2val = 0;
			adc_sample_count = 0;
		}
		
	}
}

//	#### Funcoes para quando os valores forem alterados ####
//
//	Lembrando: os valores trabalhados aqui sao todos temporarios
// 			   e so entram em efeito na confirmacao da configuracao
//	Tem muito "tmp" nessa parte...
//	O padrao seguido:
//	"tmp_*"			Significa:	"temporario_*"
//	"tmp*"			Significa:	"tempo*"
//	"tmp_tmpIns"	Significa:	"temporario_tempoInspiratorio"

//	Alteracao do tempo inspiratorio
void alterarTmpIns() {
	// Limita o tempo inspiratorio
	if(tmp_tmpIns < 0.1f)
		tmp_tmpIns = 0.1f;
	
	// Altera outros valores com base no tempo inspiratorio
	tmp_tmpExp = tmp_tmpCmp - tmp_tmpIns;
	if(tmp_tmpIns > 5*tmp_tmpExp){
		tmp_tmpCmp = tmp_tmpIns*1.2f;
		tmp_freRes = 60.0f/tmp_tmpCmp;
	}
	else if(tmp_tmpIns < 0.0101f*tmp_tmpExp){
		tmp_tmpCmp = 100.0f*tmp_tmpIns;
		tmp_freRes = 60.0f/tmp_tmpCmp;
	}

	tmp_I_E = tmp_tmpIns/tmp_tmpExp;
}

//	Alteracao da frequencia inspiratoria
void alterarFreRes() {
	// Limita a frequencia respiratoria
	if(tmp_freRes > 180.0f)
		tmp_freRes = 180.0f;
	else if(tmp_freRes < 1.0f)
		tmp_freRes = 1.0f;

	// Altera outros valores com base na frequencia respiratoria
	tmp_tmpCmp = 60.0f/tmp_freRes;
	tmp_tmpExp = tmp_tmpCmp - tmp_tmpIns;
	if(tmp_tmpIns > 5*tmp_tmpExp)
		tmp_tmpIns = (5.0f/6.0f) * tmp_tmpCmp;
	else if(tmp_tmpIns < 0.0101f*tmp_tmpExp)
		tmp_tmpIns = 0.01f * tmp_tmpCmp;

	tmp_I_E = tmp_tmpIns/tmp_tmpExp;
}
