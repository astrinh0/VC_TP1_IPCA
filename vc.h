#pragma warning (disable: 4996)


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2019/2020
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//         
//          
//          
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                           MACROS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                  ESTRUTURAS DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct {
	unsigned char *data;
	int width, height;
	int channels;			// Bin�rio/Cinzentos=1; RGB=3
	int levels;				// Bin�rio=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UM BLOB (OBJECTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct {
	int x, y, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// �rea
	int xc, yc;					// Centro-de-massa
	int perimeter;				// Per�metro
	int label;					// Etiqueta
} OVC;


/* typedef struct {
	int radius;       // Raio do filtro
	double **matrix;  // Matriz do filtro
} FILTER; */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Assinaturas
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);



int vc_gray_to_binary(IVC *src, IVC *dst, int threshold);
int vc_binary_erode(IVC *src, IVC *dst, int kernel);
int vc_binary_erode(IVC *src, IVC *dst, int kernel);
int vc_binary_dilate(IVC *src, IVC *dst, int kernel);
int vc_binary_close(IVC *src, IVC *dst,int sizedilate, int sizeerode);
int vc_binary_open(IVC *src, IVC *dst, int sizeerode, int sizedilate);
int vc_gray_edge_prewitt(IVC *src, IVC *dst, float th);


OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);


