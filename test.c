#include<stdio.h>
#include<stdlib.h>
typedef struct {
		unsigned char r1_bit0:1;
		unsigned char g1_bit1:1;
		unsigned char b1_bit2:1;
		unsigned char x1_bit3:1;
		unsigned char r2_bit4:1;
		unsigned char g2_bit5:1;
		unsigned char b2_bit6:1;
		unsigned char x2_bit7:1;
} rgbdata;

typedef struct {
	unsigned char red,green,blue;
} PPMPixel;

typedef struct {
	int x, y;
	PPMPixel *data;
} PPMImage;

#define CREATOR "RPFELGUEIRAS"
#define RGB_COMPONENT_COLOR 255
/*
   P3
# The P3 means colors are in ASCII, then 3 columns and 2 rows,
# then 255 for max color, then RGB triplets
3 2
255
255   0   0     0 255   0     0   0 255
255 255   0   255 255 255     0   0   0
 *
 *
 * */

static PPMImage *readPPM(const char *filename)
{
	char buff[16];
	PPMImage *img;
	FILE *fp;
	unsigned char temp;
	int c, rgb_comp_color;
	int i=0, line, j;
	unsigned char *rgbdatas;
	//open PPM file for reading
	fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//read image format
	if (!fgets(buff, sizeof(buff), fp)) {
		perror(filename);
		exit(1);
	}

	//check the image format
	printf("%c%c",buff[0], buff[1]);
	printf("\n");
	if (buff[0] != 'P' || buff[1] != '6') {
		fprintf(stderr, "Invalid image format (must be 'P6')\n");
		exit(1);
	}

	//alloc memory form image
	img = (PPMImage *)malloc(sizeof(PPMImage));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//check for comments
	c = getc(fp);
	printf("%c", c);
	while (c == '#') {
		while ((c = getc(fp)) != '\n') {
			printf("%c", c);
		}
		c = getc(fp);
	}
	printf("\n");

	ungetc(c, fp);
	//read image size information
	if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
		fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
		exit(1);
	}
	printf("%d %d", img->x, img->y);
	printf("\n");

	//read rgb component
	if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
		fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
		exit(1);
	}
	printf("%d", rgb_comp_color);
	printf("\n");

	//check rgb component depth
	if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
		fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
		exit(1);
	}

	while (fgetc(fp) != '\n');
	//memory allocation for pixel data
	img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));
	rgbdatas = (unsigned char *)malloc((img->x * img->y)/2 + 1 );
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}
	//read pixel data from file
	if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
		fprintf(stderr, "Error loading image '%s'\n", filename);
		exit(1);
	}

	//for(i=0; i < img->x*img->y;i++) {
	for(i=0; i < img->x*img->y;i++) {
	//	if (i%32==0) {
	//		if (i!=0) printf("\n");
	//	}
		//printf("0x%02x,0x%02x,0x%02x,",img->data[i].red,img->data[i].green,img->data[i].blue);
		temp = 0;
		temp = (img->data[i].red&0x01) | ((img->data[i].green&0x01)<<1) | ((img->data[i].blue&0x01)<<2);
		if (i%2==1) {
			rgbdatas[j] |= (temp<<4); 
			j++;
		} else {
			rgbdatas[j] |= temp; 
		}
	}
	for(i=0; i < img->x*img->y/2;i++) {
		for(j=0;j<img->x/2;j++) {
			printf("0x%02x,",rgbdatas[i*16+j]);
		}
	}
	fclose(fp);
	return img;
}
void writePPM(const char *filename, PPMImage *img)
{
	FILE *fp;
	//open file for output
	fp = fopen(filename, "wb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//write the header file
	//image format
	fprintf(fp, "P6\n");

	//comments
	fprintf(fp, "# Created by %s\n",CREATOR);

	//image size
	fprintf(fp, "%d %d\n",img->x,img->y);

	// rgb component depth
	fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

	// pixel data
	fwrite(img->data, 3 * img->x, img->y, fp);
	fclose(fp);
}

void changeColorPPM(PPMImage *img)
{
	int i;
	if(img){

		for(i=0;i<img->x*img->y;i++){
			img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
			img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
			img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;
		}
	}
}

int main(){
	PPMImage *image;
	image = readPPM("1.ppm");
	changeColorPPM(image);
	writePPM("can_bottom2.ppm",image);
//	printf("Press any key...");
//	getchar();
}
