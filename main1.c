#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vc.h"

int main(int argc, char *argv[]){

    IVC* image;    // Imagem a trabalhar
    OVC plate;     // Blob da matricula
    OVC* letters;  // Blobs das letras da matricula
    int nletters;  // Numero de letras lidas

    // Verificação dos parametros de entrada
    if(argc < 2 ){
        printf("ERROR -> Unspecified file!\n");
        getchar();
        return 0;
    }
    
    // Inicialização da imagem de entrada
    image = vc_read_image(argv[1]);

    IVC* temp  = vc_image_new(image->width, image->height, 1, 255);
    IVC* temp1  = vc_image_new(image->width, image->height, 1, 255);
    IVC* temp2  = vc_image_new(image->width, image->height, 1, 255);

    // Verificação de erro ao iniciar a imagem
    if(image == NULL){
        printf("ERROR -> VC_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    vc_gray_to_binary_midpoint(image, temp, 81);
    
    //vc_binary_dilate(temp, temp1, 2);
    
    vc_binary_erode(temp, temp1, 6);
    
    //vc_binary_dilate(temp1, temp2, 30);

    vc_write_image("vc-0001.ppm", temp1);

    vc_image_free(image);
    vc_image_free(temp);
    vc_image_free(temp1);
    vc_image_free(temp2);


    return 1;
}