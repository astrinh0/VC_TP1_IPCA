//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2020/2021
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "vc.h"

//------------------------------------------------------------------------
//            FUNCOES: ALOCAR E LIBERTAR UMA IMAGEM
//------------------------------------------------------------------------


// Alocar memória para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *) malloc(sizeof(IVC));

	if(image == NULL) return NULL;
	if((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *) malloc(image->width * image->height * image->channels * sizeof(char));

	if(image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}


// Libertar memória de uma imagem
IVC *vc_image_free(IVC *image)
{
	if(image != NULL)
	{
		if(image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}


//------------------------------------------------------------------------
//    FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//------------------------------------------------------------------------


char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;
	
	for(;;)
	{
		while(isspace(c = getc(file)));
		if(c != '#') break;
		do c = getc(file);
		while((c != '\n') && (c != EOF));
		if(c == EOF) break;
	}
	
	t = tok;
	
	if(c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));
		
		if(c == '#') ungetc(c, file);
	}
	
	*t = 0;
	
	return tok;
}


long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);
				
				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}


void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;
				
				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}


IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;
	
	// Abre o ficheiro
	if((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if(strcmp(tok, "P4") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if(strcmp(tok, "P5") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if(strcmp(tok, "P6") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
			#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
			#endif

			fclose(file);
			return NULL;
		}
		
		if(levels == 1) // PBM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			if((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			size = image->width * image->height * image->channels;

			if((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}
		
		fclose(file);
	}
	else
	{
		#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
		#endif
	}
	
	return image;
}


int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;
	
	if(image == NULL) return 0;

	if((file = fopen(filename, "wb")) != NULL)
	{
		if(image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;
			
			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);
			
			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if(fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);
		
			if(fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				return 0;
			}
		}
		
		fclose(file);

		return 1;
	}
	
	return 0;
}


//Calcular Grayscale pelo Blue
int vc_rgb_get_blue_gray(IVC *src)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	
	int width = src->width;
	int height = src->height;
	int bytesperline = src->width * src->channels;
	int channels = src->channels;
	int x, y;
	long int pos;

	//verifica erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (src->channels != 3) return 0;

	//Pega no valor azul e iguala todos os outros a esse valor
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			datasrc[pos] = datasrc[pos + 2];	   //Red
			datasrc[pos + 1] = datasrc[pos + 2]; //Green
		}
	}
	return 1;
}

//Calcular RGB para Grey com intensidades de cor
int vc_rgb_to_gray(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float r, g, b;

	//verificação de erros

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->channels != 3) || (dst->channels != 1)) return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			r = (float)datasrc[pos_src];
			g = (float)datasrc[pos_src + 1];
			b = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((r * 0.299) + (g * 0.587) + (b * 0.114));
		}
	}

	return 1;
}

// Converter uma imagem em tons de cinzento numa imagem binária
//  src: imagem em tons de cinzento
//  dst: imagem binária
//  threshold: threshold a utilizar na conversão
int vc_gray_to_binary(IVC *src, IVC *dst, int threshold)
{

	unsigned char *datasrc = (unsigned char*)src->data;
	int bytesperline = src->width*src->channels;
	int channels = src->channels;
	
	unsigned char *datadst = (unsigned char*)dst->data;
	
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))return 0;
	if ((src->width != dst->width) || (src->height != dst->height))return 0;
	if ((src->channels != 1) || (dst->channels != 1))return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y*bytesperline + x*channels;

			if (datasrc[pos] < threshold)
			{
				datadst[pos] = 0;
			}
			else
			{
				datadst[pos] = 255;
			}

		}
	}

	return 1;
}


// Aplica uma erosão aos elementos de uma imagem binária
// Utilizámos um kernel quadrado devido a sua eficiencia em questoes de processamento e a aplicação dada a esta função, neste caso em especifico
//  src: imagem binaria original
//  dst: imagem binaria erodida
//  kernel: tamanho do kernel a utilizar (número ímpar)
int vc_binary_erode(IVC *src, IVC *dst, int kernel) {
	
	unsigned char *datasrc = (unsigned char*)src->data;
	int bytesperline = src->width*src->channels;
	int channels = src->channels;
	
	unsigned char *datadst = (unsigned char*)dst->data;
	
	int width = src->width;
	int height = src->height;
	int x, y, x2, y2;
	long int pos_src, pos_dst;
	int verifica;
	kernel *= 0.5;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))return 0;
	if ((src->width != dst->width) || (src->height != dst->height))return 0;
	if ((src->channels != 1) || (dst->channels != 1))return 0;


	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_dst = y*bytesperline + x*channels;

			verifica = 0;

			for (y2 = y - kernel; y2 <= y + kernel; y2++)
			{
				for (x2 = x - kernel; x2 <= x + kernel; x2++)
				{
					if (y2 < 0 || y2 > height || x2 < 0 || x2 > width) continue;  // Não exceder os limites da imagem

					pos_src = y2*bytesperline + x2*channels;
					if (datasrc[pos_src] == 0) { verifica = 1; break;}   // Se algum dos pixeis do kernel for verificado, já não é necessário percorrer o resto do kernel
				}
				if (verifica) break;                                     // Parar o ciclo quando algum pixel do kernel for verificado
			}

			if (verifica == 1) { datadst[pos_dst] = 0; }
			else { datadst[pos_dst] = 255; }

		}
	}

	return 1;
}

// Operadores Morfológicos: Operam a forma dos objetos da imagem

// Aplica uma dilatação aos elementos de uma imagem binária
//  Utilizámos um kernel quadrado devido a sua eficiência em questões de processamento e a aplicação dada a esta função, neste caso em específico
//  src: imagem binária original
//  dst: imagem binária dilatada
//  kernel -> tamanho do kernel a utilizar (número ímpar)
int vc_binary_dilate(IVC *src, IVC *dst, int kernel)
{
   
    unsigned char *datasrc = (unsigned char *) src->data;
    int bytesperline_src = src->width* src->channels;
    int channels_src = src->channels;
    
    unsigned char *datadst = (unsigned char *) dst->data;
    int width = src->width;
    int height = src->height;
    
    int x,y,xx,yy;
    long int pos,posk;
    
    if((src->width <=0) || (src->height <=0) || (src->data == NULL)) return 0;
    if((src->width != dst->width) || (src->height != dst->height) ) return 0;
    if(( src->channels !=1 ) || ( dst->channels!=1 )) return 0;
    
    for(y=0;y<height;y++){
        for(x=0; x<width;x++){
            pos = y*bytesperline_src + x*channels_src;
            datadst[pos]=0;
            if ((((y - kernel/2)>=0) && ((x-kernel/2)>=0) && ((y+kernel/2)<height-1) && ((x+kernel/2)<width-1))) {
                for(yy=y-kernel /2; yy<=y+kernel/2;yy++){
                    for(xx=x-kernel/2;xx<=x+kernel/2;xx++){
                        posk= yy * bytesperline_src + xx * channels_src;
                        if(datasrc[posk]==255) datadst[pos] = 255;
                    }
                }
            }
        }
    }
    return 1;
}

// Aplica uma operação de abertura nos elementos da imagem
//  src: imagem original
//  dst: imagem destino
//  kerode: kernel a aplicar na erosão
//  kdilate: kernel a aplicar na dilatação
int vc_binary_open(IVC *src, IVC *dst, int kerode, int kdilate)
{

	int verifica=1;

	IVC *dstTemp = vc_image_new(src->width, src->height, src->channels, src->levels);

	verifica &= vc_binary_erode(src, dstTemp, kerode);
	verifica &= vc_binary_dilate(dstTemp, dst, kdilate);

	vc_image_free(dstTemp);

	return verifica;
}

// Aplica uma operação de fecho nos elementos da imagem
//  src: imagem original
//  dst: imagem final
//  kdilate: kernel a aplicar na dilatação
//  kerode: kernel a aplicar na erosão
int vc_binary_close(IVC *src, IVC *dst, int kdilate, int kerode)
{

	int verifica = 1;
	
	IVC *dstTemp = vc_image_new(src->width, src->height, src->channels, src->levels);

	verifica &= vc_binary_dilate(src, dstTemp, kdilate);
	verifica &= vc_binary_erode(dstTemp, dst, kerode);

	vc_image_free(dstTemp);

	return verifica;
}


