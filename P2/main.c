#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vc.h"

int main(int argc, char *argv[]){

    IVC* image;
    OVC* blobs;
    int nblobs;
    int *nlabels;

    int labels;
    int *ptrLabels = &labels;

    // Verificação dos parametros de entrada
    if(argc < 2 ){
        printf("ERROR -> Unspecified file!\n");
        getchar();
        return 0;
    }
    
    // Inicialização da imagem de entrada
    image = vc_read_image(argv[1]);

    IVC* temp  = vc_image_new(image->width, image->height, 1, 255);
    IVC* temp1  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp2  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp3  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp4  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp5  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp6  = vc_image_new(image->width, image->height, 1, 1);


    // Verificação de erro ao iniciar a imagem
    if(image == NULL){
        printf("ERROR -> VC_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    vc_rgb_get_blue_gray(image);  
    vc_write_image("get_blue.ppm", image);

    vc_rgb_to_gray(image, temp);
    vc_write_image("grayscale.pgm", temp);

    vc_gray_to_binary(temp, temp1, 100);
    vc_write_image("binary.pbm", temp1);

    vc_binary_close(temp1, temp2, 13, 17);
    vc_write_image("close.pbm", temp2);

    blobs = 
    



    vc_image_free(image);
    vc_image_free(temp);
    vc_image_free(temp1);
    vc_image_free(temp2);
    vc_image_free(temp3);
    vc_image_free(temp4);
    vc_image_free(temp5);
    vc_image_free(temp6);
    
    return 1;
}