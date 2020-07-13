#include "ESP32Lib.h";
#include "Logo.h";

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

void DrawGraphic1(short xpos, short ypos, short xsize, short ysize)
{
}

void DrawGraphic2(short xpos, short ypos, short xsize, short ysize)
{
}
