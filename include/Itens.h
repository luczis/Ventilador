#include "ESP32Lib.h";
#include "Logo.h";

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
};

void DrawBackground(short xsize, short ysize, char color1, char color2){
	short i=0, j=0;
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
						vga.dot(xpos+i+k,ypos+j+l,0xb);
					if(Logo[i+100*j]==0xb)
						vga.dot(xpos+i+k,ypos+j+l,0x8);
				}
			}
		}
	}
}

void SetupGraph(Graph* grafico, short xpos, short ypos, short xsize, short ysize){
	short i=0,j=0;
	grafico->xpos=xpos;
	grafico->ypos=ypos;
	grafico->xsize=xsize;
	grafico->ysize=ysize;
	for(i=0;i<grafico->xsize;i++)
		for(j=0;j<grafico->ysize;j++)
			vga.dot(grafico->xpos+i,grafico->ypos+j,grafico->back_color);
	for(i=0;i<grafico->xsize;i++)
		vga.dot(grafico->xpos+i,grafico->ypos+1,grafico->axis_color);
	for(i=0;i<grafico->ysize;i++)
		vga.dot(grafico->xpos-1,grafico->ypos+i,grafico->axis_color);
	if(grafico->ydataoff > 0){
		short xaxis=grafico->ysize-(short)((float)grafico->ysize/grafico->ydatasize*(float)grafico->ydataoff)+grafico->ypos;
		for(i=0;i<grafico->xsize;i++)
			vga.dot(grafico->xpos,xaxis,grafico->axis_color);
	}
}

void RedrawGraph(Graph* grafico, short xdatanum, float newy){
	short i=0, yres=0, ymax, ymin;
	xdatanum = xdatanum%grafico->xdatasize;
	ymax = grafico->ypos;
	ymin = grafico->ypos-grafico->ysize;
	yres=grafico->ysize-(short)(newy*grafico->ysize/grafico->ydatasize)+grafico->ypos;
	for(i=0; i< grafico->ysize;i++){
		vga.dot(grafico->xpos+xdatanum,grafico->ypos+i,grafico->back_color);
	}
	vga.dot(grafico->xpos+xdatanum,yres<ymin ? ymin : yres>ymax ? ymax : yres, grafico->data_color);
	if(grafico->ydataoff > 0){
		short xaxis=grafico->ysize-(short)((float)grafico->ysize/grafico->ydatasize*(float)grafico->ydataoff)+grafico->ypos;
		vga.dot(grafico->xpos,xaxis,grafico->axis_color);
	}
}

void RedrawGraphic(Graph* grafico, void* xdata, void* ydata){

}
