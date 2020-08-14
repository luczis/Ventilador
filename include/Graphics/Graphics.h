/*
	Author: bitluni 2019
	License: 
	Creative Commons Attribution ShareAlike 4.0
	https://creativecommons.org/licenses/by-sa/4.0/
	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net

	With modifications from Lucas Zischler :)
*/
#pragma once
#include <stdlib.h>
#include <math.h>
#include "Font.h"
#include "ImageDrawer.h"

template<typename Color>
class Graphics: public ImageDrawer
{
  public:
	int cursorX, cursorY, cursorBaseX;
	long frontColor, backColor;
	Font *font;
	int frameBufferCount;
	int currentFrameBuffer;
	Color **frameBuffers[3];
	Color **frontBuffer;
	Color **backBuffer;
	bool autoScroll;

	int xres;
	int yres;

	virtual void dotFast(int x, int y, Color color) = 0;
	virtual void dot(int x, int y, Color color) = 0;
	virtual void dotAdd(int x, int y, Color color) = 0;
	virtual void dotMix(int x, int y, Color color) = 0;
	virtual Color get(int x, int y) = 0;
	virtual Color** allocateFrameBuffer() = 0;
	virtual Color** allocateFrameBuffer(int xres, int yres, Color value)
	{
		Color** frame = (Color **)malloc(yres * sizeof(Color *));
		if(!frame)
			ERROR("Not enough memory for frame buffer");				
		for (int y = 0; y < yres; y++)
		{
			frame[y] = (Color *)malloc(xres * sizeof(Color));
			if(!frame[y])
				ERROR("Not enough memory for frame buffer");
			for (int x = 0; x < xres; x++)
				frame[y][x] = value;
		}
		return frame;
	}
	virtual Color RGBA(int r, int g, int b, int a = 255) const = 0;
	virtual int R(Color c) const = 0;
	virtual int G(Color c) const = 0;
	virtual int B(Color c) const = 0;
	virtual int A(Color c) const = 0;
	Color RGB(unsigned long rgb) const 
	{
		return RGBA(rgb & 255, (rgb >> 8) & 255, (rgb >> 16) & 255);
	}
	Color RGBA(unsigned long rgba) const 
	{
		return RGBA(rgba & 255, (rgba >> 8) & 255, (rgba >> 16) & 255, rgba >> 24);
	}
	Color RGB(int r, int g, int b) const 
	{
		return RGBA(r, g, b);
	}

	void setFrameBufferCount(unsigned char i)
	{
		frameBufferCount = i > 3 ? 3 : i;
	}

	virtual void show(bool vSync = false)
	{
		if(!frameBufferCount)
			return;
		currentFrameBuffer = (currentFrameBuffer + 1) % frameBufferCount;
		frontBuffer = frameBuffers[currentFrameBuffer];
		backBuffer = frameBuffers[(currentFrameBuffer + frameBufferCount - 1) % frameBufferCount];
	}

	Graphics(int xres = 0, int yres = 0)
	{
		this->xres = xres;
		this->yres = yres;
		font = 0;
		cursorX = cursorY = cursorBaseX = 0;
		frontColor = -1;
		backColor = 0;
		frameBufferCount = 1;
		for(int i = 0; i < 3; i++)
			frameBuffers[i] = 0;
		frontBuffer = 0;
		backBuffer = 0;
		autoScroll = true;
	}

	virtual bool allocateFrameBuffers()
	{
		if(yres <= 0 || xres <= 0)
			return false;
		for(int i = 0; i < frameBufferCount; i++)
			frameBuffers[i] = allocateFrameBuffer();
		currentFrameBuffer = 0;
		show();
		return true;
	}

	virtual void setResolution(int xres, int yres)
	{
		this->xres = xres;
		this->yres = yres;
		allocateFrameBuffers();
	}

	virtual float pixelAspect() const
	{
		return 1;
	}

	void setTextColor(long front, long back = 0)
	{
		frontColor = front;
		backColor = back;
	}

	void setFont(Font &font)
	{
		this->font = &font;
	}

	void setCursor(int x, int y)
	{
		cursorX = cursorBaseX = x;
		cursorY = y;
	}

	virtual void drawChar(int x, int y, int ch, char char_color=0xf, char back_color=0x0, char size = 1, bool is_vertical = false, short x_min_trim=0, short x_max_trim=0)
	{
		if (!font)
			return;
		if (!font->valid(ch))
			return;
		const unsigned char *pix = &font->pixels[font->charWidth * font->charHeight * (ch - font->firstChar)];
		int px = 0, py = 0;
		for (is_vertical? px = 0 : py = 0; is_vertical ? px < font->charHeight*size : py < font->charHeight*size; is_vertical ? px+=size : py+=size)
			for (is_vertical ? py = 0 : px = 0; is_vertical ? py < font->charWidth*size : px < font->charWidth*size; is_vertical ? py+=size : px+=size)
				if (*(pix++))
					for(char i = 0; i < size; i++)
					{
						if(x_max_trim && px+i+x>x_max_trim)
							break;
						if(x_min_trim && px+i+x<x_min_trim)
							break;
						for(char j = 0; j < size; j++)
						{
						    is_vertical ? dotMix(py + i + x, px + j + y, char_color) : dotMix(px + i + x, py + j + y, char_color);
                        			}
					}
				else
                			for(char i = 0; i < size; i++)
					{
						if(x_max_trim && px+i+x>x_max_trim)
							break;
						if(x_min_trim && px+i+x<x_min_trim)
							break;
						for(char j = 0; j < size; j++)
					    	{
						    is_vertical ? dotMix(py + i + x, px + j + y, back_color) : dotMix(px + i + x, py + j + y, back_color);
                        			}
					}
	}

	void print(const char ch, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0, short x_min_trim=0, short x_max_trim=0)
	{
		if (!font)
			return;
		if (font->valid(ch))
			drawChar(cursorX, cursorY, ch, char_color, back_color, size, is_vertical, x_min_trim, x_max_trim);
		else
			drawChar(cursorX, cursorY, ' ', char_color, back_color, size, is_vertical, x_min_trim, x_max_trim);		
		is_vertical ? cursorY += font->charHeight * size : cursorX += font->charWidth * size;
		if (is_vertical ? cursorY + font->charHeight * size > yres : cursorX + font->charWidth * size > xres)
		{
			if(is_vertical){
			cursorY = cursorBaseX;
			cursorX += font->charWidth * size;
			}
			else{
			cursorX = cursorBaseX;
			cursorY += font->charHeight * size;
			if(autoScroll && cursorY + font->charHeight * size > yres)
				scroll(cursorY + font->charHeight * size - yres, backColor);
			}
		}
	}

	void println(const char ch)
	{
		print(ch);
		print("\n");
	}

	void print(const char *str, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0, short x_min_trim=0, short x_max_trim=0)
	{
		if (!font)
			return;
		while (*str)
		{
			if(*str == '\n')
			{
				cursorX = cursorBaseX;
				cursorY += font->charHeight*size;
			}
			else
				print(*str, char_color, back_color, size, is_vertical, x_min_trim, x_max_trim);
			str++;
		}
	}

	void printCenter(char *str, short xmin, short xmax, char char_color=0xf, char back_color=0x0, char size=1){
		if((xmax < xmin)||(*str=='\0'))
			return;
		static char* new_str=(char*)calloc(100,sizeof(char));
		bool flag_newline=0;
		char* tmp_str=str;
		char spc_cnt=0, str_size=0, max_c=0;
		while(*tmp_str!='\0' && max_c < 100){
			if(*tmp_str=='\n')
				flag_newline=1;
			if(*tmp_str==' ')
				spc_cnt++;
			tmp_str++; str_size++; max_c++;
		}
		bool to_long=str_size*font->charWidth*size>xmax-xmin;
		char lines=1+(str_size*font->charWidth*size)/(xmax-xmin);
		char spc_per_line=spc_cnt/lines;

		// Neste momento e so possivel quebrar em no maximo 2 linhas
		// Desculpe nao sei o motivo
		// Se necessario atualizarei no futuro
		// - Lucas
		if(flag_newline||to_long){
			spc_cnt=0;
			tmp_str=str;
			char pastPos=0;
			for(char i=1; i<=lines; i++){
				for(char j=0; j<str_size; j++){
					if((str[j+pastPos]==' ')||(str[j+pastPos]=='\n')||(str[j+pastPos]=='\0')){
						if(spc_cnt>=spc_per_line){
							for(char k=0; k<j; k++)
								new_str[k]=str[k+pastPos];
							new_str[j]='\0';
							pastPos=j+1;
							printCenter(new_str, xmin, xmax, char_color, back_color, size);
							cursorY+=font->charHeight*size;
							spc_cnt=0;
							j=str_size;
						}
						else
							spc_cnt++;
					}
				}
			}
			return;
		}
		if(str_size*font->charWidth*size>xmax-xmin)
			return;
		cursorX=(xmax-xmin-font->charWidth*size*str_size)/2+xmin;
		print(str, char_color, back_color, size, 0, xmin, xmax);
	}

/*	void printCenter(char *str, short xmin, short xmax, char char_color=0xf, char back_color=0x0, char size=1){
		if(xmax < xmin)
			return;
		bool flag_newline=0;
		char* tmp_str=str;
		char str_size=0, max_c=0;
		while(*tmp_str!='\0' && max_c < 100){
			if(*tmp_str=='\n')
				flag_newline=1;
			tmp_str++; str_size++; max_c++;
		}
		bool to_long=str_size*font->charWidth*size>xmax-xmin;
		if(flag_newline||to_long){
			tmp_str=str;
			for(char i=0;i<str_size;i++, tmp_str++){
				if(*tmp_str=='\n'||(*tmp_str==' '&&to_long)){
					char* new_str=(char*)calloc(i+1,sizeof(char));
					for(char j=0;j<i;j++)
						new_str[j]=str[j];
					new_str[i]='\0';
					printCenter(new_str, xmin, xmax, char_color, back_color, size);
					delete[] new_str;
					cursorY+=font->charHeight*size;
					char* new_str2=(char*)calloc(str_size-i+1,sizeof(char));
					for(char j=0;j<str_size-i;j++)
						new_str2[j]=str[i+1+j];
					new_str2[str_size]='\0';
					printCenter(new_str2, xmin, xmax, char_color, back_color, size);
					return;
				}
			}
		}
		if(str_size*font->charWidth*size>xmax-xmin)
			return;
		cursorX=(xmax-xmin-font->charWidth*size*str_size)/2+xmin;
		print(str, char_color, back_color, size, 0, xmin, xmax);
	}*/

	void println(const char *str)
	{
		print(str); 
		print("\n");
	}

	void print(long number, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0)
	{
		bool sign = number < 0;
		if (sign)
			number = -number;
		const char baseChars[] = "0123456789";
		char temp[33];
		temp[32] = 0;
		int i = 31;
		do
		{
			temp[i--] = baseChars[number % 10];
			number /= 10;
		} while (number > 0);
		if (sign)
			temp[i--] = '-';
		for (; i > 30; i--)
			temp[i] = ' ';
		print(&temp[i + 1], char_color, back_color, size, is_vertical);
	}

	void print(double number, unsigned char fractionalDigits = 2, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0)
	{
		if(fractionalDigits > 6)
			fractionalDigits=6;
		long p = long(pow(10, fractionalDigits));
		long long n = (double(number) * p + 0.5f);
		print(long(n / p), char_color, back_color, size, is_vertical);
		if(fractionalDigits)
		{
			print(".", char_color, back_color, size, is_vertical);
			for(char i = 0; i < fractionalDigits; i++)
			{
				p /= 10;
				print(long(n / p) % 10, char_color, back_color, size, is_vertical);
			}
		}
	}

	void print(unsigned long number, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0){print((long)number, char_color, back_color, size, is_vertical);}
	void print(int number, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0){print((long)number, char_color, back_color, size, is_vertical);}
	void print(unsigned int number, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0){print((long)number, char_color, back_color, size, is_vertical);}
	void print(short number, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0){print((long)number, char_color, back_color, size, is_vertical);}
	void print(unsigned short number, char char_color=0xf, char back_color=0x0, char size=1, bool is_vertical=0){print((long)number, char_color, back_color, size, is_vertical);}
	
	void println()
	{
		print("\n");
	}

	virtual void clear(Color color = 0)
	{
		for (int y = 0; y < yres; y++)
			for (int x = 0; x < xres; x++)
				dotFast(x, y, color);
	}

	virtual void xLine(int x0, int x1, int y, Color color)
	{
		if (y < 0 || y >= yres)
			return;
		if (x0 > x1)
		{
			int xb = x0;
			x0 = x1;
			x1 = xb;
		}
		if (x0 < 0)
			x0 = 0;
		if (x1 > xres)
			x1 = xres;
		for (int x = x0; x < x1; x++)
			dotFast(x, y, color);
	}

	void triangle(short *v0, short *v1, short *v2, Color color)
	{
		short *v[3] = {v0, v1, v2};
		if (v[1][1] < v[0][1])
		{
			short *vb = v[0];
			v[0] = v[1];
			v[1] = vb;
		}
		if (v[2][1] < v[1][1])
		{
			short *vb = v[1];
			v[1] = v[2];
			v[2] = vb;
		}
		if (v[1][1] < v[0][1])
		{
			short *vb = v[0];
			v[0] = v[1];
			v[1] = vb;
		}
		int y = v[0][1];
		int xac = v[0][0] << 16;
		int xab = v[0][0] << 16;
		int xbc = v[1][0] << 16;
		int xaci = 0;
		int xabi = 0;
		int xbci = 0;
		if (v[1][1] != v[0][1])
			xabi = ((v[1][0] - v[0][0]) << 16) / (v[1][1] - v[0][1]);
		if (v[2][1] != v[0][1])
			xaci = ((v[2][0] - v[0][0]) << 16) / (v[2][1] - v[0][1]);
		if (v[2][1] != v[1][1])
			xbci = ((v[2][0] - v[1][0]) << 16) / (v[2][1] - v[1][1]);

		for (; y < v[1][1] && y < yres; y++)
		{
			if (y >= 0)
				xLine(xab >> 16, xac >> 16, y, color);
			xab += xabi;
			xac += xaci;
		}
		for (; y < v[2][1] && y < yres; y++)
		{
			if (y >= 0)
				xLine(xbc >> 16, xac >> 16, y, color);
			xbc += xbci;
			xac += xaci;
		}
	}

	void line(int x1, int y1, int x2, int y2, Color color)
	{
		int x, y, xe, ye;
		int dx = x2 - x1;
		int dy = y2 - y1;
		int dx1 = labs(dx);
		int dy1 = labs(dy);
		int px = 2 * dy1 - dx1;
		int py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			dot(x, y, color);
			for (int i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}
					px = px + 2 * (dy1 - dx1);
				}
				dot(x, y, color);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			dot(x, y, color);
			for (int i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}
					py = py + 2 * (dx1 - dy1);
				}
				dot(x, y, color);
			}
		}
	}

	void fillRect(int x, int y, int w, int h, Color color)
	{
		if (x < 0)
		{
			w += x;
			x = 0;
		}
		if (y < 0)
		{
			h += y;
			y = 0;
		}
		if (x + w > xres)
			w = xres - x;
		if (y + h > yres)
			h = yres - y;
		for (int j = y; j < y + h; j++)
			for (int i = x; i < x + w; i++)
				dotFast(i, j, color);
	}

	void rect(int x, int y, int w, int h, Color color)
	{
		fillRect(x, y, w, 1, color);
		fillRect(x, y, 1, h, color);
		fillRect(x, y + h - 1, w, 1, color);
		fillRect(x + w - 1, y, 1, h, color);
	}

	void circle(int x, int y, int r, Color color)
	{
		int oxr = r;
		for(int i = 0; i < r + 1; i++)
		{
			int xr = (int)sqrt(r * r - i * i);
			xLine(x - oxr, x - xr + 1, y + i, color);
			xLine(x + xr, x + oxr + 1, y + i, color);
			if(i) 
			{
				xLine(x - oxr, x - xr + 1, y - i, color);
				xLine(x + xr, x + oxr + 1, y - i, color);
			}
			oxr = xr;
		}
	}

	void fillCircle(int x, int y, int r, Color color)
	{
		for(int i = 0; i < r + 1; i++)
		{
			int xr = (int)sqrt(r * r - i * i);
			xLine(x - xr, x + xr + 1, y + i, color);
			if(i) 
				xLine(x - xr, x + xr + 1, y - i, color);
		}
	}

	void ellipse(int x, int y, int rx, int ry, Color color)
	{
		if(ry == 0)
			return;
		int oxr = rx;
		float f = float(rx) / ry;
		f *= f;
		for(int i = 0; i < ry + 1; i++)
		{
			float s = rx * rx - i * i * f;
			int xr = (int)sqrt(s <= 0 ? 0 : s);
			xLine(x - oxr, x - xr + 1, y + i, color);
			xLine(x + xr, x + oxr + 1, y + i, color);
			if(i) 
			{
				xLine(x - oxr, x - xr + 1, y - i, color);
				xLine(x + xr, x + oxr + 1, y - i, color);
			}
			oxr = xr;
		}
	}

	void fillEllipse(int x, int y, int rx, int ry, Color color)
	{
		if(ry == 0)
			return;
		float f = float(rx) / ry;
		f *= f;		
		for(int i = 0; i < ry + 1; i++)
		{
			float s = rx * rx - i * i * f;
			int xr = (int)sqrt(s <= 0 ? 0 : s);
			xLine(x - xr, x + xr + 1, y + i, color);
			if(i) 
				xLine(x - xr, x + xr + 1, y - i, color);
		}
	}

	virtual void scroll(int dy, Color color)
	{
		if(dy > 0)
		{
			for(int d = 0; d < dy; d++)
			{
				Color *l = backBuffer[0];
				for(int i = 0; i < yres - 1; i++)
				{
					backBuffer[i] = backBuffer[i + 1];
				}
				backBuffer[yres - 1] = l;
				xLine(0, xres, yres - 1, color);
			}
		}
		else
		{
			for(int d = 0; d < -dy; d++)
			{
				Color *l = backBuffer[yres - 1];
				for(int i = 1; i < yres; i++)
				{
					backBuffer[i] = backBuffer[i - 1];
				}
				backBuffer[0] = l;
				xLine(0, xres, 0, color);
			}
		}
		cursorY -= dy;
	}

	virtual Color R5G5B4A2ToColor(unsigned short c)
	{
		int r = (((c << 1) & 0x3e) * 255 + 1) / 0x3e;
		int g = (((c >> 4) & 0x3e) * 255 + 1) / 0x3e;
		int b = (((c >> 9) & 0x1e) * 255 + 1) / 0x1e;
		int a = (((c >> 13) & 6) * 255 + 1) / 6;
		return RGBA(r, g, b, a);
	}

	virtual Color R2G2B2A2ToColor(unsigned char c)
	{
		int r = ((int(c) & 3) * 255 + 1) / 3;
		int g = (((int(c) >> 2) & 3) * 255 + 1) / 3;
		int b = (((int(c) >> 4) & 3) * 255 + 1) / 3;
		int a = (((int(c) >> 6) & 3) * 255 + 1) / 3;
		return RGBA(r, g, b, a);
	}

	virtual Color R4G4B4A4ToColor(unsigned short c)
	{
		int r = (((c << 1) & 0x1e) * 255 + 1) / 0x1e;
		int g = (((c >> 3) & 0x1e) * 255 + 1) / 0x1e;
		int b = (((c >> 7) & 0x1e) * 255 + 1) / 0x1e;
		int a = (((c >> 11) & 0x1e) * 255 + 1) / 0x1e;
		return RGBA(r, g, b, a);
	}

	virtual Color R8G8B8A8ToColor(unsigned long c)
	{
		return RGBA(c & 255, (c >> 8) & 255, (c >> 16) & 255, (c >> 24) & 255);
	}

	virtual void imageR8G8B8A8(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dot(px + x, py + y, R8G8B8A8ToColor(((unsigned long*)image.pixels)[i++]));
		}		
	}

	virtual void imageAddR8G8B8A8(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotAdd(px + x, py + y, R8G8B8A8ToColor(((unsigned long*)image.pixels)[i++]));
		}
	}

	virtual void imageMixR8G8B8A8(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotMix(px + x, py + y, R8G8B8A8ToColor(((unsigned long*)image.pixels)[i++]));
		}
	}	

	virtual void imageR4G4B4A4(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dot(px + x, py + y, R4G4B4A4ToColor(((unsigned short*)image.pixels)[i++]));
		}		
	}

	virtual void imageAddR4G4B4A4(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotAdd(px + x, py + y, R4G4B4A4ToColor(((unsigned short*)image.pixels)[i++]));
		}
	}

	virtual void imageMixR4G4B4A4(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotMix(px + x, py + y, R4G4B4A4ToColor(((unsigned short*)image.pixels)[i++]));
		}
	}	

	virtual void imageR5G5B4A2(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dot(px + x, py + y, R5G5B4A2ToColor(((unsigned short*)image.pixels)[i++]));
		}		
	}

	virtual void imageAddR5G5B4A2(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotAdd(px + x, py + y, R5G5B4A2ToColor(((unsigned short*)image.pixels)[i++]));
		}
	}

	virtual void imageMixR5G5B4A2(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotMix(px + x, py + y, R5G5B4A2ToColor(((unsigned short*)image.pixels)[i++]));
		}
	}	

	virtual void imageR2G2B2A2(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dot(px + x, py + y, R2G2B2A2ToColor(((unsigned char*)image.pixels)[i++]));
		}		
	}

	virtual void imageAddR2G2B2A2(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotAdd(px + x, py + y, R2G2B2A2ToColor(((unsigned char*)image.pixels)[i++]));
		}
	}

	virtual void imageMixR2G2B2A2(Image &image, int x, int y, int srcX, int srcY, int srcXres, int srcYres)
	{
		for (int py = 0; py < srcYres; py++)
		{
			int i = srcX + (py + srcY) * image.xres;
			for (int px = 0; px < srcXres; px++)
				dotMix(px + x, py + y, R2G2B2A2ToColor(((unsigned char*)image.pixels)[i++]));
		}
	}	
};
