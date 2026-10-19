// This will apply the sobel filter and return the PSNR between the golden sobel and the produced sobel
// sobelized image
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define SIZE	4096
#define INPUT_FILE	"input.grey"
#define OUTPUT_FILE	"output_sobel.grey"
#define GOLDEN_FILE	"golden.grey"

#define MAX_P (2 * 1020 * 1020)

/* The horizontal and vertical operators to be used in the sobel filter */
char horiz_operator[3][3] = {{-1, 0, 1}, 
                             {-2, 0, 2}, 
                             {-1, 0, 1}};
char vert_operator[3][3] = {{1, 2, 1}, 
                            {0, 0, 0}, 
                            {-1, -2, -1}};

double sobel(unsigned char *input, unsigned char *output, unsigned char *golden);

/* The arrays holding the input image, the output image and the output used *
 * as golden standard. The luminosity (intensity) of each pixel in the      *
 * grayscale image is represented by a value between 0 and 255 (an unsigned *
 * character). The arrays (and the files) contain these values in row-major *
 * order (element after element within each row and row after row. 			*/
unsigned char input[SIZE*SIZE], output[SIZE*SIZE], golden[SIZE*SIZE];

/* sqrt_table[p] = min((int)sqrt(p), 255) for every p = gx^2 + gy^2. */
unsigned char sqrt_table[MAX_P + 1];


/* The main computational function of the program. The input, output and *
 * golden arguments are pointers to the arrays used to store the input   *
 * image, the output produced by the algorithm and the output used as    *
 * golden standard for the comparisons.									 */
double sobel(unsigned char *input, unsigned char *output, unsigned char *golden)
{
	double PSNR = 0;
	int i, j, r0, r1, r2, idx0, idx1, idx2;
	int r3, r4, r5;
	int idx3, idx4, idx5;
	int p00, p01, p02, p10, p12, p20, p21, p22;
	int p0_0, p0_1, p0_2, p0_3, p0_4, p0_5;
	int p1_0, p1_1, p1_2, p1_3, p1_4, p1_5;
	int p2_0, p2_1, p2_2, p2_3, p2_4, p2_5;
	int p3_0, p3_1, p3_2, p3_3, p3_4, p3_5;
	int p4_0, p4_1, p4_2, p4_3, p4_4, p4_5;
	int p5_0, p5_1, p5_2, p5_3, p5_4, p5_5;
	unsigned int p;
	unsigned int p0_0_sq, p0_1_sq, p0_2_sq, p0_3_sq;
	unsigned int p1_0_sq, p1_1_sq, p1_2_sq, p1_3_sq;
	unsigned int p2_0_sq, p2_1_sq, p2_2_sq, p2_3_sq;
	unsigned int p3_0_sq, p3_1_sq, p3_2_sq, p3_3_sq;
	unsigned long long mse = 0;
	int res_horiz, res_vert, d;
	int res_horiz0_0, res_horiz0_1, res_horiz0_2, res_horiz0_3;
	int res_vert0_0, res_vert0_1, res_vert0_2, res_vert0_3;
	int res_horiz1_0, res_horiz1_1, res_horiz1_2, res_horiz1_3;
	int res_vert1_0, res_vert1_1, res_vert1_2, res_vert1_3;
	int res_horiz2_0, res_horiz2_1, res_horiz2_2, res_horiz2_3;
	int res_vert2_0, res_vert2_1, res_vert2_2, res_vert2_3;
	int res_horiz3_0, res_horiz3_1, res_horiz3_2, res_horiz3_3;
	int res_vert3_0, res_vert3_1, res_vert3_2, res_vert3_3;
	int d0_0, d0_1, d0_2, d0_3;
	int d1_0, d1_1, d1_2, d1_3;
	int d2_0, d2_1, d2_2, d2_3;
	int d3_0, d3_1, d3_2, d3_3;
	struct timespec  tv1, tv2;
	FILE *f_in, *f_out, *f_golden;

	/* The first and last row of the output array, as well as the first  *
     * and last element of each column are not going to be filled by the *
     * algorithm, therefore make sure to initialize them with 0s.		 */
	memset(output, 0, SIZE*sizeof(unsigned char));
	memset(&output[SIZE*(SIZE-1)], 0, SIZE*sizeof(unsigned char));
	for (i = 1; i < SIZE-1; i++) {
		output[i*SIZE] = 0;
		output[i*SIZE + SIZE - 1] = 0;
	}

	/* Open the input, output, golden files, read the input and golden    *
     * and store them to the corresponding arrays.						  */
	f_in = fopen(INPUT_FILE, "r");
	if (f_in == NULL) {
		printf("File " INPUT_FILE " not found\n");
		exit(1);
	}
  
	f_out = fopen(OUTPUT_FILE, "wb");
	if (f_out == NULL) {
		printf("File " OUTPUT_FILE " could not be created\n");
		fclose(f_in);
		exit(1);
	}  
  
	f_golden = fopen(GOLDEN_FILE, "r");
	if (f_golden == NULL) {
		printf("File " GOLDEN_FILE " not found\n");
		fclose(f_in);
		fclose(f_out);
		exit(1);
	}    

	fread(input, sizeof(unsigned char), SIZE*SIZE, f_in);
	fread(golden, sizeof(unsigned char), SIZE*SIZE, f_golden);
	fclose(f_in);
	fclose(f_golden);
  
	/* This is the main computation. Get the starting time. */
	clock_gettime(CLOCK_MONOTONIC_RAW, &tv1);

	/*  Every p in [i^2, (i+1)^2 - 1] (2i+1 values) *
	 * has (int)sqrt(p) = i. Every p >= 255^2 is clipped to 255. */
	for (i = 0; i < 255; i++)
		memset(&sqrt_table[i*i], i, 2*i + 1);
	memset(&sqrt_table[255*255], 255, MAX_P + 1 - 255*255);

	/* For each pixel of the output image */
	for (i=1; i<SIZE-4; i+=4) {
		r0 = (i - 1)*SIZE;
		r1 = i*SIZE;
		r2 = (i + 1)*SIZE;
		r3 = (i + 2)*SIZE;
		r4 = (i + 3)*SIZE;
		r5 = (i + 4)*SIZE;
		for (j=1; j<SIZE-4; j+=4) {
			idx0 = r0 + j;
			idx1 = r1 + j;
			idx2 = r2 + j;
			idx3 = r3 + j;
			idx4 = r4 + j;
			idx5 = r5 + j;

			p0_0 = input[idx0 - 1];
			p0_1 = input[idx0];
			p0_2 = input[idx0 + 1];
			p0_3 = input[idx0 + 2];
			p0_4 = input[idx0 + 3];
			p0_5 = input[idx0 + 4];

			p1_0 = input[idx1 - 1];
			p1_1 = input[idx1];
			p1_2 = input[idx1 + 1];
			p1_3 = input[idx1 + 2];
			p1_4 = input[idx1 + 3];
			p1_5 = input[idx1 + 4];

			p2_0 = input[idx2 - 1];
			p2_1 = input[idx2];
			p2_2 = input[idx2 + 1];
			p2_3 = input[idx2 + 2];
			p2_4 = input[idx2 + 3];
			p2_5 = input[idx2 + 4];

			p3_0 = input[idx3 - 1];
			p3_1 = input[idx3];
			p3_2 = input[idx3 + 1];
			p3_3 = input[idx3 + 2];
			p3_4 = input[idx3 + 3];
			p3_5 = input[idx3 + 4];

			p4_0 = input[idx4 - 1];
			p4_1 = input[idx4];
			p4_2 = input[idx4 + 1];
			p4_3 = input[idx4 + 2];
			p4_4 = input[idx4 + 3];
			p4_5 = input[idx4 + 4];

			p5_0 = input[idx5 - 1];
			p5_1 = input[idx5];
			p5_2 = input[idx5 + 1];
			p5_3 = input[idx5 + 2];
			p5_4 = input[idx5 + 3];
			p5_5 = input[idx5 + 4];

			res_horiz0_0 = -p0_0 + p0_2 - (p1_0 << 1) + (p1_2 << 1) - p2_0 + p2_2;
			res_vert0_0  =  p0_0 + (p0_1 << 1) + p0_2 - p2_0 - (p2_1 << 1) - p2_2;
			p0_0_sq = (res_horiz0_0 * res_horiz0_0) + (res_vert0_0 * res_vert0_0);
			output[idx1] = sqrt_table[p0_0_sq];

			res_horiz0_1 = -p0_1 + p0_3 - (p1_1 << 1) + (p1_3 << 1) - p2_1 + p2_3;
			res_vert0_1  =  p0_1 + (p0_2 << 1) + p0_3 - p2_1 - (p2_2 << 1) - p2_3;
			p0_1_sq = (res_horiz0_1 * res_horiz0_1) + (res_vert0_1 * res_vert0_1);
			output[idx1 + 1] = sqrt_table[p0_1_sq];

			res_horiz0_2 = -p0_2 + p0_4 - (p1_2 << 1) + (p1_4 << 1) - p2_2 + p2_4;
			res_vert0_2  =  p0_2 + (p0_3 << 1) + p0_4 - p2_2 - (p2_3 << 1) - p2_4;
			p0_2_sq = (res_horiz0_2 * res_horiz0_2) + (res_vert0_2 * res_vert0_2);
			output[idx1 + 2] = sqrt_table[p0_2_sq];

			res_horiz0_3 = -p0_3 + p0_5 - (p1_3 << 1) + (p1_5 << 1) - p2_3 + p2_5;
			res_vert0_3  =  p0_3 + (p0_4 << 1) + p0_5 - p2_3 - (p2_4 << 1) - p2_5;
			p0_3_sq = (res_horiz0_3 * res_horiz0_3) + (res_vert0_3 * res_vert0_3);
			output[idx1 + 3] = sqrt_table[p0_3_sq];

			res_horiz1_0 = -p1_0 + p1_2 - (p2_0 << 1) + (p2_2 << 1) - p3_0 + p3_2;
			res_vert1_0  =  p1_0 + (p1_1 << 1) + p1_2 - p3_0 - (p3_1 << 1) - p3_2;
			p1_0_sq = (res_horiz1_0 * res_horiz1_0) + (res_vert1_0 * res_vert1_0);
			output[idx2] = sqrt_table[p1_0_sq];

			res_horiz1_1 = -p1_1 + p1_3 - (p2_1 << 1) + (p2_3 << 1) - p3_1 + p3_3;
			res_vert1_1  =  p1_1 + (p1_2 << 1) + p1_3 - p3_1 - (p3_2 << 1) - p3_3;
			p1_1_sq = (res_horiz1_1 * res_horiz1_1) + (res_vert1_1 * res_vert1_1);
			output[idx2 + 1] = sqrt_table[p1_1_sq];

			res_horiz1_2 = -p1_2 + p1_4 - (p2_2 << 1) + (p2_4 << 1) - p3_2 + p3_4;
			res_vert1_2  =  p1_2 + (p1_3 << 1) + p1_4 - p3_2 - (p3_3 << 1) - p3_4;
			p1_2_sq = (res_horiz1_2 * res_horiz1_2) + (res_vert1_2 * res_vert1_2);
			output[idx2 + 2] = sqrt_table[p1_2_sq];

			res_horiz1_3 = -p1_3 + p1_5 - (p2_3 << 1) + (p2_5 << 1) - p3_3 + p3_5;
			res_vert1_3  =  p1_3 + (p1_4 << 1) + p1_5 - p3_3 - (p3_4 << 1) - p3_5;
			p1_3_sq = (res_horiz1_3 * res_horiz1_3) + (res_vert1_3 * res_vert1_3);
			output[idx2 + 3] = sqrt_table[p1_3_sq];

			res_horiz2_0 = -p2_0 + p2_2 - (p3_0 << 1) + (p3_2 << 1) - p4_0 + p4_2;
			res_vert2_0  =  p2_0 + (p2_1 << 1) + p2_2 - p4_0 - (p4_1 << 1) - p4_2;
			p2_0_sq = (res_horiz2_0 * res_horiz2_0) + (res_vert2_0 * res_vert2_0);
			output[idx3] = sqrt_table[p2_0_sq];

			res_horiz2_1 = -p2_1 + p2_3 - (p3_1 << 1) + (p3_3 << 1) - p4_1 + p4_3;
			res_vert2_1  =  p2_1 + (p2_2 << 1) + p2_3 - p4_1 - (p4_2 << 1) - p4_3;
			p2_1_sq = (res_horiz2_1 * res_horiz2_1) + (res_vert2_1 * res_vert2_1);
			output[idx3 + 1] = sqrt_table[p2_1_sq];

			res_horiz2_2 = -p2_2 + p2_4 - (p3_2 << 1) + (p3_4 << 1) - p4_2 + p4_4;
			res_vert2_2  =  p2_2 + (p2_3 << 1) + p2_4 - p4_2 - (p4_3 << 1) - p4_4;
			p2_2_sq = (res_horiz2_2 * res_horiz2_2) + (res_vert2_2 * res_vert2_2);
			output[idx3 + 2] = sqrt_table[p2_2_sq];

			res_horiz2_3 = -p2_3 + p2_5 - (p3_3 << 1) + (p3_5 << 1) - p4_3 + p4_5;
			res_vert2_3  =  p2_3 + (p2_4 << 1) + p2_5 - p4_3 - (p4_4 << 1) - p4_5;
			p2_3_sq = (res_horiz2_3 * res_horiz2_3) + (res_vert2_3 * res_vert2_3);
			output[idx3 + 3] = sqrt_table[p2_3_sq];

			res_horiz3_0 = -p3_0 + p3_2 - (p4_0 << 1) + (p4_2 << 1) - p5_0 + p5_2;
			res_vert3_0  =  p3_0 + (p3_1 << 1) + p3_2 - p5_0 - (p5_1 << 1) - p5_2;
			p3_0_sq = (res_horiz3_0 * res_horiz3_0) + (res_vert3_0 * res_vert3_0);
			output[idx4] = sqrt_table[p3_0_sq];

			res_horiz3_1 = -p3_1 + p3_3 - (p4_1 << 1) + (p4_3 << 1) - p5_1 + p5_3;
			res_vert3_1  =  p3_1 + (p3_2 << 1) + p3_3 - p5_1 - (p5_2 << 1) - p5_3;
			p3_1_sq = (res_horiz3_1 * res_horiz3_1) + (res_vert3_1 * res_vert3_1);
			output[idx4 + 1] = sqrt_table[p3_1_sq];

			res_horiz3_2 = -p3_2 + p3_4 - (p4_2 << 1) + (p4_4 << 1) - p5_2 + p5_4;
			res_vert3_2  =  p3_2 + (p3_3 << 1) + p3_4 - p5_2 - (p5_3 << 1) - p5_4;
			p3_2_sq = (res_horiz3_2 * res_horiz3_2) + (res_vert3_2 * res_vert3_2);
			output[idx4 + 2] = sqrt_table[p3_2_sq];

			res_horiz3_3 = -p3_3 + p3_5 - (p4_3 << 1) + (p4_5 << 1) - p5_3 + p5_5;
			res_vert3_3  =  p3_3 + (p3_4 << 1) + p3_5 - p5_3 - (p5_4 << 1) - p5_5;
			p3_3_sq = (res_horiz3_3 * res_horiz3_3) + (res_vert3_3 * res_vert3_3);
			output[idx4 + 3] = sqrt_table[p3_3_sq];
			d0_0 = output[idx1] - golden[idx1];
			d0_1 = output[idx1 + 1] - golden[idx1 + 1];
			d0_2 = output[idx1 + 2] - golden[idx1 + 2];
			d0_3 = output[idx1 + 3] - golden[idx1 + 3];
			d1_0 = output[idx2] - golden[idx2];
			d1_1 = output[idx2 + 1] - golden[idx2 + 1];
			d1_2 = output[idx2 + 2] - golden[idx2 + 2];
			d1_3 = output[idx2 + 3] - golden[idx2 + 3];
			d2_0 = output[idx3] - golden[idx3];
			d2_1 = output[idx3 + 1] - golden[idx3 + 1];
			d2_2 = output[idx3 + 2] - golden[idx3 + 2];
			d2_3 = output[idx3 + 3] - golden[idx3 + 3];
			d3_0 = output[idx4] - golden[idx4];
			d3_1 = output[idx4 + 1] - golden[idx4 + 1];
			d3_2 = output[idx4 + 2] - golden[idx4 + 2];
			d3_3 = output[idx4 + 3] - golden[idx4 + 3];
			mse += d0_0 * d0_0 + d0_1 * d0_1 + d0_2 * d0_2 + d0_3 * d0_3 + d1_0 * d1_0 + d1_1 * d1_1 + d1_2 * d1_2 + d1_3 * d1_3 + d2_0 * d2_0 + d2_1 * d2_1 + d2_2 * d2_2 + d2_3 * d2_3 + d3_0 * d3_0 + d3_1 * d3_1 + d3_2 * d3_2 + d3_3 * d3_3;
		}

		for (; j<SIZE-1; j++) {
			{
				idx0 = r0 + j;
				idx1 = r1 + j;
				idx2 = r2 + j;

				p00 = input[idx0 - 1];
				p01 = input[idx0];
				p02 = input[idx0 + 1];
				p10 = input[idx1 - 1];
				p12 = input[idx1 + 1];
				p20 = input[idx2 - 1];
				p21 = input[idx2];
				p22 = input[idx2 + 1];

				res_horiz = -p00 + p02 - (p10 << 1) + (p12 << 1) - p20 + p22;
				res_vert  =  p00 + (p01 << 1) + p02 - p20 - (p21 << 1) - p22;

				p = (res_horiz * res_horiz) + (res_vert * res_vert);
				output[idx1] = sqrt_table[p];
				d = output[idx1] - golden[idx1];
				mse += d * d;
			}
			{
				idx0 = r1 + j;
				idx1 = r2 + j;
				idx2 = r3 + j;

				p00 = input[idx0 - 1];
				p01 = input[idx0];
				p02 = input[idx0 + 1];
				p10 = input[idx1 - 1];
				p12 = input[idx1 + 1];
				p20 = input[idx2 - 1];
				p21 = input[idx2];
				p22 = input[idx2 + 1];

				res_horiz = -p00 + p02 - (p10 << 1) + (p12 << 1) - p20 + p22;
				res_vert  =  p00 + (p01 << 1) + p02 - p20 - (p21 << 1) - p22;

				p = (res_horiz * res_horiz) + (res_vert * res_vert);
				output[idx1] = sqrt_table[p];
				d = output[idx1] - golden[idx1];
				mse += d * d;
			}
			{
				idx0 = r2 + j;
				idx1 = r3 + j;
				idx2 = r4 + j;

				p00 = input[idx0 - 1];
				p01 = input[idx0];
				p02 = input[idx0 + 1];
				p10 = input[idx1 - 1];
				p12 = input[idx1 + 1];
				p20 = input[idx2 - 1];
				p21 = input[idx2];
				p22 = input[idx2 + 1];

				res_horiz = -p00 + p02 - (p10 << 1) + (p12 << 1) - p20 + p22;
				res_vert  =  p00 + (p01 << 1) + p02 - p20 - (p21 << 1) - p22;

				p = (res_horiz * res_horiz) + (res_vert * res_vert);
				output[idx1] = sqrt_table[p];
				d = output[idx1] - golden[idx1];
				mse += d * d;
			}
			{
				idx0 = r3 + j;
				idx1 = r4 + j;
				idx2 = r5 + j;

				p00 = input[idx0 - 1];
				p01 = input[idx0];
				p02 = input[idx0 + 1];
				p10 = input[idx1 - 1];
				p12 = input[idx1 + 1];
				p20 = input[idx2 - 1];
				p21 = input[idx2];
				p22 = input[idx2 + 1];

				res_horiz = -p00 + p02 - (p10 << 1) + (p12 << 1) - p20 + p22;
				res_vert  =  p00 + (p01 << 1) + p02 - p20 - (p21 << 1) - p22;

				p = (res_horiz * res_horiz) + (res_vert * res_vert);
				output[idx1] = sqrt_table[p];
				d = output[idx1] - golden[idx1];
				mse += d * d;
			}
		}
	}

	for (; i<SIZE-1; i++) {
		r0 = (i - 1)*SIZE;
		r1 = i*SIZE;
		r2 = (i + 1)*SIZE;
		for (j=1; j<SIZE-1; j++) {
			idx0 = r0 + j;
			idx1 = r1 + j;
			idx2 = r2 + j;

			p00 = input[idx0 - 1];
			p01 = input[idx0];
			p02 = input[idx0 + 1];
			p10 = input[idx1 - 1];
			p12 = input[idx1 + 1];
			p20 = input[idx2 - 1];
			p21 = input[idx2];
			p22 = input[idx2 + 1];

			res_horiz = -p00 + p02 - (p10 << 1) + (p12 << 1) - p20 + p22;
			res_vert  =  p00 + (p01 << 1) + p02 - p20 - (p21 << 1) - p22;

			p = (res_horiz * res_horiz) + (res_vert * res_vert);
			output[idx1] = sqrt_table[p];
			d = output[idx1] - golden[idx1];
			mse += d * d;
		}
	}

	PSNR = (double)mse;
	PSNR /= (double)(SIZE*SIZE);
	PSNR = 10*log10(65536/PSNR);

	/* This is the end of the main computation. Take the end time,  *
	 * calculate the duration of the computation and report it. 	*/
	clock_gettime(CLOCK_MONOTONIC_RAW, &tv2);

	printf ("Total time = %10g seconds\n",
			(double) (tv2.tv_nsec - tv1.tv_nsec) / 1000000000.0 +
			(double) (tv2.tv_sec - tv1.tv_sec));

  
	/* Write the output file */
	fwrite(output, sizeof(unsigned char), SIZE*SIZE, f_out);
	fclose(f_out);
  
	return PSNR;
}


int main(int argc, char* argv[])
{
	double PSNR;
	PSNR = sobel(input, output, golden);
	printf("PSNR of original Sobel and computed Sobel image: %g\n", PSNR);
	printf("A visualization of the sobel filter can be found at " OUTPUT_FILE ", or you can run 'make image' to get the jpg\n");

	return 0;
}

