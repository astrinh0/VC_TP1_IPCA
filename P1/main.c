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

    IVC* temp  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp1  = vc_image_new(image->width, image->height, 1, 1); // <-- Não está a ser usada
    IVC* temp2  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp3  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp4  = vc_image_new(image->width, image->height, 1, 1);

    IVC* temp5  = vc_image_new(image->width, image->height, 1, 255);

    // Verificação de erro ao iniciar a imagem
    if(image == NULL){
        printf("ERROR -> VC_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    vc_gray_to_binary(image, temp, 100);    
    vc_write_image("binary.pbm", temp);

    vc_binary_open(temp, temp2, 7, 7);
    vc_write_image("open.pbm", temp2);

    
    blobs = vc_binary_blob_labelling(temp2, temp3, ptrLabels);
    vc_binary_blob_info(temp3, blobs, labels);
    vc_write_image("labelling.pbm", temp3); 


    vc_binary_dilate(temp2, temp4, 11);
    vc_write_image("dilate.pbm", temp4);

    vc_brains_out(image, temp4, temp5);
    vc_write_image("brain.pgm", temp5);



    printf("\nLabels: %d", labels);
    printf("\nPerimetro: %d", blobs->perimeter);
    printf("\nArea: %d\n", blobs->area);
    printf("\nCentro massa: x: %d y: %d\n", blobs->xc, blobs->yc);




    vc_image_free(image);
    vc_image_free(temp);
    vc_image_free(temp1);
    vc_image_free(temp2);
    vc_image_free(temp3);
    vc_image_free(temp4);
    vc_image_free(temp5);


    return 1;
}