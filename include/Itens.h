#include "ESP32Lib.h"
#include "Logo.h"

struct Graph{
	unsigned short xpos = 0;
	unsigned short ypos = 0;
	unsigned short xsize = 100;
	unsigned short ysize = 100;
	float xdatasize = (float)xsize;
	float ydatasize = (float)ysize;
	float xdataoff = 0.0f;
	float ydataoff = 0.0f;
	short* xticks;
	short* yticks;
	short* xdata;
	short* ydata;
	char axis_color = 0xb;
	char back_color = 0x8;
	char data_color = 0xf;
	char text_color = 0xf;
};

struct Button{
	unsigned short xpos = 0;
	unsigned short ypos = 0;
	unsigned short xsize = 0;
	unsigned short ysize = 0;
	bool config_on = 0;
	bool is_selected = 0;
	bool is_changing = 0;
	char border_size = 2;
	char u_text_size = 1;
	char m_text_size = 3;
	char b_text_size = 2;
	char back_color = 0xf;
	char u_text_color = 0x8;
	char m_text_color = 0x8;
	char b_text_color = 0x8;
	char border_no_config_color = 0xa;
	char border_default_color = 0xb;
	char border_selected_color = 0x9;
	char border_changing_color = 0xe;
};

struct Box{
	unsigned short xpos = 0;
	unsigned short ypos = 0;
	unsigned short xsize = 0;
	unsigned short ysize = 0;
	char border_size = 2;
	char u_text_size = 1;
	char m_text_size = 3;
	char b_text_size = 2;
	char back_color = 0xf;
	char u_text_color = 0x8;
	char m_text_color = 0x8;
	char b_text_color = 0x8;
};

char* int2array(int num){
	char i;
	bool sig=0;
	if(num<0){
		num=-num; sig=1;
	}
	char n;
	num ? n=log10(num)+1+sig:n=1;
	static char* numArray;
	free(numArray);
	numArray=(char*)calloc(n+1,sizeof(char));
	if(sig)
		numArray[0]='-';
	for(i=1;i<=n-sig;++i,num/=10)
		numArray[n-i]=0x30+num%10;
	numArray[n]='\0';
	return numArray;
}

char* float2array(float num){
	char i;
	bool sig=0;
	if(num<0){
		num=-num; sig=1;
	}
	char decimals;
	num>0.1f ? decimals=4-sig-log10(num):decimals=4-sig;
	if(decimals<0)
		decimals=0;
	int num_i=(int)(pow(10,decimals)*num);
	char n;
	num>0.1f ? n=log10(num_i)+2+sig:n=decimals+1+sig;
	static char* numArray;
	free(numArray);
	numArray=(char*)calloc(n+1,sizeof(char));
	if(sig)
		numArray[0]='-';
	for(i=1;i<=decimals;++i,num_i/=10){
		numArray[n-i]=0x30+num_i%10;
	}
	if(decimals>0)
		numArray[n-decimals-1]='.';
	for(i=decimals+2;i<=n-sig;++i,num_i/=10){
		numArray[n-i]=0x30+num_i%10;
	}
	numArray[n]='\0';
	return numArray;
}

//DEBUG:: Utilizado para mostrar a quantidade de quadros por segundo, no canto superior direito. Tenha em mente que esta funcao ocupa processamento do dispositivo
void showFPS(){
	static bool has_started=false;
	if(!has_started)
	{
		vga.setCursor(750-8*vga.font->charWidth,0);
		vga.print("FPS[Hz]:");	
		has_started=true;
	}

	static unsigned char counter=0;
	static unsigned long int last_time;

	if(counter>=20){
		unsigned long time1 = millis()-last_time;
		last_time=millis();
		vga.setCursor(750,0);
		for(char i=0, j=0;i<3*vga.font->charWidth;i++)
			for(j=0;j<vga.font->charHeight;j++)
				vga.dot(i+vga.cursorX,j+vga.cursorY,vga.backColor);
		vga.print(20000/time1);
		counter=0;
	}
	counter++;
}

//xsize e ysize sao dados pelo tamanho da tela, color1 e color2 definem as duas cores que sao misturadas na razao 50/50
void DrawBackground(short xsize, short ysize, char color1, char color2){
	short i=0, j=0;
	vga.backColor=color1;
	if(color1==color2){
		for(i=0; i<xsize; i++)
			for(j=0; j<ysize; j++)
				vga.dot(i, j, color1);
		return;
	}
	for(i=0; i<xsize; i++)
		for(j=0; j<ysize; j++)
		{
			(i+j)%2 ? vga.dot(i, j, color1) : vga.dot(i, j, color2);
		}
}

//Xpos e Ypos sao a posicao inicial do canto superior-esquerdo. Size e um multiplo intero que escala a imagem
void DrawLogo(short xpos, short ypos, char size=1){
	char i=0,j=0,k=0,l=0;
	for(i=0;i<100;i++){
		for(j=0;j<80;j++){
			for(k=0;k<size;k++){
				for(l=0;l<size;l++){
					if(Logo[i+100*j]==0x8)
						vga.dot(xpos+size*i+k,ypos+size*j+l,0xb);
					if(Logo[i+100*j]==0xb)
						vga.dot(xpos+size*i+k,ypos+size*j+l,0x8);
				}
			}
		}
	}
}

//Cria um grafico, recebe um Graph como referencia
void SetupGraph(Graph* grafico, short xpos, short ypos, short xsize, short ysize, char* title = "", char title_size = 1, bool print_axis = false){
	short i=0,j=0;
	grafico->xpos=xpos;
	grafico->ypos=ypos;
	grafico->xsize=xsize;
	grafico->ysize=ysize;
	for(i=0;i<=grafico->xsize;i++)
		for(j=0;j<grafico->ysize;j++)
			vga.dot(grafico->xpos+i,grafico->ypos+j,grafico->back_color);
	for(i=0;i<=grafico->xsize;i++)
		vga.dot(grafico->xpos+i,grafico->ypos+grafico->ysize,grafico->axis_color);
	for(i=0;i<=grafico->ysize;i++)
		vga.dot(grafico->xpos-1,grafico->ypos+i,grafico->axis_color);
	if(grafico->ydataoff > 0){
		short xaxis=grafico->ysize-(short)((float)grafico->ysize/grafico->ydatasize*(float)grafico->ydataoff)+grafico->ypos;
		for(i=0;i<grafico->xsize;i++)
			vga.dot(grafico->xpos+i,xaxis,grafico->axis_color);
	}
	
	vga.setCursor(0, grafico->ypos-vga.font->charHeight*title_size);
	vga.printCenter(title, grafico->xpos, grafico->xpos+grafico->xsize, grafico->text_color, vga.backColor, title_size);

	if(print_axis) {
		short num = -(short)((float)grafico->ysize/grafico->ydatasize*(float)grafico->ydataoff);
		char n = 0;
		bool sig = 0;
	
		//ymin	
		num = (short)(grafico->ydatasize-grafico->ydataoff);
		sig=num<0;
		num ? n=log10(num<0?-num:num)+1+sig:n=1;
		
		vga.setCursor(grafico->xpos-vga.font->charWidth*n-(short)(vga.font->charWidth*0.5f),grafico->ypos);
		vga.print(int2array(num), grafico->text_color, 0, 1);
		
		//ymax	
		num = -(short)((float)grafico->ysize/grafico->ydatasize*(float)grafico->ydataoff);
		sig=num<0;
		num ? n=log10(num<0?-num:num)+1+sig:n=1;
		
		vga.setCursor(grafico->xpos-vga.font->charWidth*n-(short)(vga.font->charWidth*0.5f),grafico->ypos+grafico->ysize-vga.font->charHeight);
		vga.print(int2array(num), grafico->text_color, 0, 1);

		//xmin
		vga.setCursor(grafico->xpos, grafico->ypos+grafico->ysize+(short)(vga.font->charHeight*0.5f));
		vga.print("0", grafico->text_color, 0, 1);
		
		//xmax	
		num = (short)((float)grafico->xsize/grafico->xdatasize);
		sig=num<0;
		num ? n=log10(num<0?-num:num)+1+sig:n=1;
		
		vga.setCursor(grafico->xpos+grafico->xsize-vga.font->charWidth*n,grafico->ypos+grafico->ysize+(short)(vga.font->charHeight*0.5f));
		vga.print(int2array(num), grafico->text_color, 0, 1);

	}
}

//Utilizado para atualizar o grafico, xdatanum define o valor em x, e newy define o novo valor de y
void RedrawGraph(Graph* grafico, short xdatanum, float newy){
	short i=0, yres=0, ymax, ymin;
	//xdatanum = xdatanum%((short)grafico->xdatasize);
	ymax=grafico->ypos;
	ymin=grafico->ypos+grafico->ysize-1;
	yres=grafico->ysize-(short)((newy+grafico->ydataoff)*grafico->ysize/grafico->ydatasize)+grafico->ypos;
	for(i=0; i<grafico->ysize;i++){
		vga.dot(grafico->xpos+xdatanum,grafico->ypos+i,grafico->back_color);
	}
	vga.dot(grafico->xpos+xdatanum,yres>ymin ? ymin : yres<ymax ? ymax : yres, grafico->data_color);
	if(grafico->ydataoff > 0){
		short xaxis=grafico->ysize-(short)((float)grafico->ysize/grafico->ydatasize*(float)grafico->ydataoff)+grafico->ypos;
		vga.dot(grafico->xpos+xdatanum,xaxis,grafico->axis_color);
	}
}

//Cria um botao, recebe um Button como referencia
void SetupButton(Button* botao, short xpos, short ypos, short xsize, short ysize, char* up_text = "", char* middle_text = "", char* bottom_text = ""){
	short i=0,j=0;
	botao->xpos=xpos;
	botao->ypos=ypos;
	botao->xsize=xsize;
	botao->ysize=ysize;
	for(i=0;i<botao->xsize;i++)
		for(j=0;j<botao->ysize;j++)
			vga.dot(botao->xpos+i,botao->ypos+j,botao->back_color);
	vga.setCursor(0,botao->ypos+botao->border_size);
	if(*up_text != '\0')
		vga.printCenter(up_text, botao->xpos+botao->border_size, botao->xpos+botao->xsize-botao->border_size, botao->u_text_color, botao->back_color, botao->u_text_size);
	vga.setCursor(0,botao->ypos+(botao->ysize-vga.font->charHeight*botao->m_text_size)/2);
	if(*middle_text != '\0')
		vga.printCenter(middle_text, botao->xpos+botao->border_size, botao->xpos+botao->xsize-botao->border_size, botao->m_text_color, botao->back_color, botao->m_text_size);
	vga.setCursor(0,botao->ypos+botao->ysize-vga.font->charHeight*botao->b_text_size-botao->border_size);
	if(*bottom_text != '\0')
		vga.printCenter(bottom_text, botao->xpos+botao->border_size, botao->xpos+botao->xsize-botao->border_size, botao->b_text_color, botao->back_color, botao->b_text_size);
	for(i=0;i<botao->xsize;i++)
		for(j=0;j<botao->border_size;j++){
			vga.dot(botao->xpos+i,botao->ypos+j,botao->border_default_color);
			vga.dot(botao->xpos+i,botao->ypos+botao->ysize-botao->border_size+j,botao->border_default_color);
		}
	for(i=0;i<botao->ysize;i++)
		for(j=0;j<botao->border_size;j++){
			vga.dot(botao->xpos+j,botao->ypos+i,botao->border_default_color);
			vga.dot(botao->xpos+j+botao->xsize-botao->border_size,botao->ypos+i,botao->border_default_color);
		}
}
void SetupButton(Button* botao, short xpos, short ypos, short xsize, short ysize, char* up_text, int  middle_text, char* bottom_text = ""){
	SetupButton(botao, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupButton(Button* botao, short xpos, short ypos, short xsize, short ysize, char* up_text, unsigned int  middle_text, char* bottom_text = ""){
	SetupButton(botao, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupButton(Button* botao, short xpos, short ypos, short xsize, short ysize, char* up_text, short  middle_text, char* bottom_text = ""){
	SetupButton(botao, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupButton(Button* botao, short xpos, short ypos, short xsize, short ysize, char* up_text, unsigned short  middle_text, char* bottom_text = ""){
	SetupButton(botao, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupButton(Button* botao, short xpos, short ypos, short xsize, short ysize, char* up_text, float  middle_text, char* bottom_text = ""){
	SetupButton(botao, xpos, ypos, xsize, ysize, up_text, float2array(middle_text), bottom_text);}

//Utilizado para atualizar um botao, atualiza o texto central
void RedrawButton(Button* botao, char* value){
	short i=0, j=0;
	if(*value!='\0')
		for(i=botao->xpos;i<botao->xpos+botao->xsize;i++)
			for(j=botao->ypos+(botao->ysize-botao->m_text_size*vga.font->charHeight)/2;j<botao->ypos+(botao->ysize+botao->m_text_size*vga.font->charHeight)/2;j++)
				vga.dot(i,j,botao->back_color);
	vga.setCursor(0,botao->ypos+(botao->ysize-vga.font->charHeight*botao->m_text_size)/2);
	if(*value!='\0')
		vga.printCenter(value, botao->xpos+botao->border_size, botao->xpos+botao->xsize-botao->border_size, botao->m_text_color, botao->back_color, botao->m_text_size);
	for(i=0;i<botao->xsize;i++)
		if(botao->config_on) {
			for(j=0;j<botao->border_size;j++){
				vga.dot(botao->xpos+i,botao->ypos+j,botao->is_changing ? botao->border_changing_color : (botao->is_selected ? botao->border_selected_color : botao->border_default_color));
				vga.dot(botao->xpos+i,botao->ypos+botao->ysize-botao->border_size+j,botao->is_changing ? botao->border_changing_color : (botao->is_selected ? botao->border_selected_color : botao->border_default_color));
			}
		}
		else {
			for(j=0;j<botao->border_size;j++){
				vga.dot(botao->xpos+i,botao->ypos+j,botao->border_no_config_color);
				vga.dot(botao->xpos+i,botao->ypos+botao->ysize-botao->border_size+j,botao->border_no_config_color);
			}
		}
	for(i=0;i<botao->ysize;i++)
		if(botao->config_on) {
			for(j=0;j<botao->border_size;j++){
				vga.dot(botao->xpos+j,botao->ypos+i,botao->is_changing ? botao->border_changing_color : (botao->is_selected ? botao->border_selected_color : botao->border_default_color));
				vga.dot(botao->xpos+j+botao->xsize-botao->border_size,botao->ypos+i,botao->is_changing ? botao->border_changing_color : (botao->is_selected ? botao->border_selected_color : botao->border_default_color));
			}
		}
		else {
			for(j=0;j<botao->border_size;j++){
				vga.dot(botao->xpos+j,botao->ypos+i,botao->border_no_config_color);
				vga.dot(botao->xpos+j+botao->xsize-botao->border_size,botao->ypos+i,botao->border_no_config_color);
			}
		}
}
void RedrawButton(Button* botao, int value){
	RedrawButton(botao, int2array(value));}
void RedrawButton(Button* botao, unsigned int value){
	RedrawButton(botao, int2array(value));}
void RedrawButton(Button* botao, short value){
	RedrawButton(botao, int2array(value));}
void RedrawButton(Button* botao, unsigned short value){
	RedrawButton(botao, int2array(value));}
void RedrawButton(Button* botao, float value){
	RedrawButton(botao, float2array(value));}

//Cria uma caixa, recebe uma Box como referencia
void SetupBox(Box* caixa, short xpos, short ypos, short xsize, short ysize, char* up_text = "", char* middle_text = "", char* bottom_text = ""){
	short i=0,j=0;
	caixa->xpos=xpos;
	caixa->ypos=ypos;
	caixa->xsize=xsize;
	caixa->ysize=ysize;
	for(i=0;i<caixa->xsize;i++)
		for(j=0;j<caixa->ysize;j++)
			vga.dot(caixa->xpos+i,caixa->ypos+j,caixa->back_color);
	vga.setCursor(0,caixa->ypos+caixa->border_size);
	if(*up_text != '\0')
		vga.printCenter(up_text, caixa->xpos+caixa->border_size, caixa->xpos+caixa->xsize-caixa->border_size, caixa->u_text_color, caixa->back_color, caixa->u_text_size);
	vga.setCursor(0,caixa->ypos+(caixa->ysize-vga.font->charHeight*caixa->m_text_size)/2);
	if(*middle_text != '\0')
		vga.printCenter(middle_text, caixa->xpos+caixa->border_size, caixa->xpos+caixa->xsize-caixa->border_size, caixa->m_text_color, caixa->back_color, caixa->m_text_size);
	vga.setCursor(0,caixa->ypos+caixa->ysize-vga.font->charHeight*caixa->b_text_size-caixa->border_size);
	if(*bottom_text != '\0')
		vga.printCenter(bottom_text, caixa->xpos+caixa->border_size, caixa->xpos+caixa->xsize-caixa->border_size, caixa->b_text_color, caixa->back_color, caixa->b_text_size);
}
void SetupBox(Box* caixa, short xpos, short ypos, short xsize, short ysize, char* up_text, int  middle_text, char* bottom_text = ""){
	SetupBox(caixa, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupBox(Box* caixa, short xpos, short ypos, short xsize, short ysize, char* up_text, unsigned int  middle_text, char* bottom_text = ""){
	SetupBox(caixa, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupBox(Box* caixa, short xpos, short ypos, short xsize, short ysize, char* up_text, short  middle_text, char* bottom_text = ""){
	SetupBox(caixa, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupBox(Box* caixa, short xpos, short ypos, short xsize, short ysize, char* up_text, unsigned short  middle_text, char* bottom_text = ""){
	SetupBox(caixa, xpos, ypos, xsize, ysize, up_text, int2array(middle_text), bottom_text);}
void SetupBox(Box* caixa, short xpos, short ypos, short xsize, short ysize, char* up_text, float  middle_text, char* bottom_text = ""){
	SetupBox(caixa, xpos, ypos, xsize, ysize, up_text, float2array(middle_text), bottom_text);}

//Utilizado para atualizar uma caixa, atualiza o texto central
void RedrawBox(Box* caixa, char* value){
	short i=0, j=0;
	if(*value!='\0')
		for(i=caixa->xpos;i<caixa->xpos+caixa->xsize;i++)
			for(j=caixa->ypos+(caixa->ysize-caixa->m_text_size*vga.font->charHeight)/2;j<caixa->ypos+(caixa->ysize+caixa->m_text_size*vga.font->charHeight)/2;j++)
				vga.dot(i,j,caixa->back_color);
	vga.setCursor(0,caixa->ypos+(caixa->ysize-vga.font->charHeight*caixa->m_text_size)/2);
	if(*value != '\0')
		vga.printCenter(value, caixa->xpos+caixa->border_size, caixa->xpos+caixa->xsize-caixa->border_size, caixa->m_text_color, caixa->back_color, caixa->m_text_size);
}
void RedrawBox(Box* caixa, int value){
	RedrawBox(caixa, int2array(value));}
void RedrawBox(Box* caixa, unsigned int value){
	RedrawBox(caixa, int2array(value));}
void RedrawBox(Box* caixa, short value){
	RedrawBox(caixa, int2array(value));}
void RedrawBox(Box* caixa, unsigned short value){
	RedrawBox(caixa, int2array(value));}
void RedrawBox(Box* caixa, float value){
	RedrawBox(caixa, float2array(value));}

void LoopLed(const int button, const int led){
	if(digitalRead(button)==HIGH){
    		digitalWrite(led, HIGH);
  	}
  	else {
    	digitalWrite(led, LOW);
  	}
}
