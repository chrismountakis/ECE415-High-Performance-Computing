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


/* The main computational function of the program. The input, output and *
 * golden arguments are pointers to the arrays used to store the input   *
 * image, the output produced by the algorithm and the output used as    *
 * golden standard for the comparisons.									 */
double sobel(unsigned char *input, unsigned char *output, unsigned char *golden)
{
	double PSNR = 0;
	int i, j, r, r0, r1, r2, idx, idx0, idx1, idx2;
	int r3, r4, r5;
	int idx3, idx4, idx5, idx6, idx7, idx8, idx9, idx10, idx11, idx12, idx13, idx14, idx15, idx16;
	int p00, p01, p02, p10, p12, p20, p21, p22;
	int p0_0, p0_1, p0_2, p0_3, p0_4, p0_5;
	int p1_0, p1_1, p1_2, p1_3, p1_4, p1_5;
	int p2_0, p2_1, p2_2, p2_3, p2_4, p2_5;
	int p3_0, p3_1, p3_2, p3_3, p3_4, p3_5;
	int p4_0, p4_1, p4_2, p4_3, p4_4, p4_5;
	int p5_0, p5_1, p5_2, p5_3, p5_4, p5_5;
	unsigned int p;
	unsigned long long mse = 0;
	unsigned int p0_0_sq, p0_1_sq, p0_2_sq, p0_3_sq;
	unsigned int p1_0_sq, p1_1_sq, p1_2_sq, p1_3_sq;
	unsigned int p2_0_sq, p2_1_sq, p2_2_sq, p2_3_sq;
	unsigned int p3_0_sq, p3_1_sq, p3_2_sq, p3_3_sq;
	int res, res_horiz, res_vert, diff;
	int res_horiz0_0, res_horiz0_1, res_horiz0_2, res_horiz0_3;
	int res_vert0_0, res_vert0_1, res_vert0_2, res_vert0_3;
	int res_horiz1_0, res_horiz1_1, res_horiz1_2, res_horiz1_3;
	int res_vert1_0, res_vert1_1, res_vert1_2, res_vert1_3;
	int res_horiz2_0, res_horiz2_1, res_horiz2_2, res_horiz2_3;
	int res_vert2_0, res_vert2_1, res_vert2_2, res_vert2_3;
	int res_horiz3_0, res_horiz3_1, res_horiz3_2, res_horiz3_3;
	int res_vert3_0, res_vert3_1, res_vert3_2, res_vert3_3;
	int diff1, diff2, diff3, diff4, diff5, diff6, diff7, diff8, diff9, diff10, diff11, diff12, diff13, diff14, diff15, diff16;
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
			res = (int)sqrt(p0_0_sq);
			if (res > 255) output[idx1] = 255;
			else            output[idx1] = (unsigned char)res;

			res_horiz0_1 = -p0_1 + p0_3 - (p1_1 << 1) + (p1_3 << 1) - p2_1 + p2_3;
			res_vert0_1  =  p0_1 + (p0_2 << 1) + p0_3 - p2_1 - (p2_2 << 1) - p2_3;
			p0_1_sq = (res_horiz0_1 * res_horiz0_1) + (res_vert0_1 * res_vert0_1);
			res = (int)sqrt(p0_1_sq);
			if (res > 255) output[idx1 + 1] = 255;
			else            output[idx1 + 1] = (unsigned char)res;

			res_horiz0_2 = -p0_2 + p0_4 - (p1_2 << 1) + (p1_4 << 1) - p2_2 + p2_4;
			res_vert0_2  =  p0_2 + (p0_3 << 1) + p0_4 - p2_2 - (p2_3 << 1) - p2_4;
			p0_2_sq = (res_horiz0_2 * res_horiz0_2) + (res_vert0_2 * res_vert0_2);
			res = (int)sqrt(p0_2_sq);
			if (res > 255) output[idx1 + 2] = 255;
			else            output[idx1 + 2] = (unsigned char)res;

			res_horiz0_3 = -p0_3 + p0_5 - (p1_3 << 1) + (p1_5 << 1) - p2_3 + p2_5;
			res_vert0_3  =  p0_3 + (p0_4 << 1) + p0_5 - p2_3 - (p2_4 << 1) - p2_5;
			p0_3_sq = (res_horiz0_3 * res_horiz0_3) + (res_vert0_3 * res_vert0_3);
			res = (int)sqrt(p0_3_sq);
			if (res > 255) output[idx1 + 3] = 255;
			else            output[idx1 + 3] = (unsigned char)res;

			res_horiz1_0 = -p1_0 + p1_2 - (p2_0 << 1) + (p2_2 << 1) - p3_0 + p3_2;
			res_vert1_0  =  p1_0 + (p1_1 << 1) + p1_2 - p3_0 - (p3_1 << 1) - p3_2;
			p1_0_sq = (res_horiz1_0 * res_horiz1_0) + (res_vert1_0 * res_vert1_0);
			res = (int)sqrt(p1_0_sq);
			if (res > 255) output[idx2] = 255;
			else            output[idx2] = (unsigned char)res;

			res_horiz1_1 = -p1_1 + p1_3 - (p2_1 << 1) + (p2_3 << 1) - p3_1 + p3_3;
			res_vert1_1  =  p1_1 + (p1_2 << 1) + p1_3 - p3_1 - (p3_2 << 1) - p3_3;
			p1_1_sq = (res_horiz1_1 * res_horiz1_1) + (res_vert1_1 * res_vert1_1);
			res = (int)sqrt(p1_1_sq);
			if (res > 255) output[idx2 + 1] = 255;
			else            output[idx2 + 1] = (unsigned char)res;

			res_horiz1_2 = -p1_2 + p1_4 - (p2_2 << 1) + (p2_4 << 1) - p3_2 + p3_4;
			res_vert1_2  =  p1_2 + (p1_3 << 1) + p1_4 - p3_2 - (p3_3 << 1) - p3_4;
			p1_2_sq = (res_horiz1_2 * res_horiz1_2) + (res_vert1_2 * res_vert1_2);
			res = (int)sqrt(p1_2_sq);
			if (res > 255) output[idx2 + 2] = 255;
			else            output[idx2 + 2] = (unsigned char)res;

			res_horiz1_3 = -p1_3 + p1_5 - (p2_3 << 1) + (p2_5 << 1) - p3_3 + p3_5;
			res_vert1_3  =  p1_3 + (p1_4 << 1) + p1_5 - p3_3 - (p3_4 << 1) - p3_5;
			p1_3_sq = (res_horiz1_3 * res_horiz1_3) + (res_vert1_3 * res_vert1_3);
			res = (int)sqrt(p1_3_sq);
			if (res > 255) output[idx2 + 3] = 255;
			else            output[idx2 + 3] = (unsigned char)res;

			res_horiz2_0 = -p2_0 + p2_2 - (p3_0 << 1) + (p3_2 << 1) - p4_0 + p4_2;
			res_vert2_0  =  p2_0 + (p2_1 << 1) + p2_2 - p4_0 - (p4_1 << 1) - p4_2;
			p2_0_sq = (res_horiz2_0 * res_horiz2_0) + (res_vert2_0 * res_vert2_0);
			res = (int)sqrt(p2_0_sq);
			if (res > 255) output[idx3] = 255;
			else            output[idx3] = (unsigned char)res;

			res_horiz2_1 = -p2_1 + p2_3 - (p3_1 << 1) + (p3_3 << 1) - p4_1 + p4_3;
			res_vert2_1  =  p2_1 + (p2_2 << 1) + p2_3 - p4_1 - (p4_2 << 1) - p4_3;
			p2_1_sq = (res_horiz2_1 * res_horiz2_1) + (res_vert2_1 * res_vert2_1);
			res = (int)sqrt(p2_1_sq);
			if (res > 255) output[idx3 + 1] = 255;
			else            output[idx3 + 1] = (unsigned char)res;

			res_horiz2_2 = -p2_2 + p2_4 - (p3_2 << 1) + (p3_4 << 1) - p4_2 + p4_4;
			res_vert2_2  =  p2_2 + (p2_3 << 1) + p2_4 - p4_2 - (p4_3 << 1) - p4_4;
			p2_2_sq = (res_horiz2_2 * res_horiz2_2) + (res_vert2_2 * res_vert2_2);
			res = (int)sqrt(p2_2_sq);
			if (res > 255) output[idx3 + 2] = 255;
			else            output[idx3 + 2] = (unsigned char)res;

			res_horiz2_3 = -p2_3 + p2_5 - (p3_3 << 1) + (p3_5 << 1) - p4_3 + p4_5;
			res_vert2_3  =  p2_3 + (p2_4 << 1) + p2_5 - p4_3 - (p4_4 << 1) - p4_5;
			p2_3_sq = (res_horiz2_3 * res_horiz2_3) + (res_vert2_3 * res_vert2_3);
			res = (int)sqrt(p2_3_sq);
			if (res > 255) output[idx3 + 3] = 255;
			else            output[idx3 + 3] = (unsigned char)res;

			res_horiz3_0 = -p3_0 + p3_2 - (p4_0 << 1) + (p4_2 << 1) - p5_0 + p5_2;
			res_vert3_0  =  p3_0 + (p3_1 << 1) + p3_2 - p5_0 - (p5_1 << 1) - p5_2;
			p3_0_sq = (res_horiz3_0 * res_horiz3_0) + (res_vert3_0 * res_vert3_0);
			res = (int)sqrt(p3_0_sq);
			if (res > 255) output[idx4] = 255;
			else            output[idx4] = (unsigned char)res;

			res_horiz3_1 = -p3_1 + p3_3 - (p4_1 << 1) + (p4_3 << 1) - p5_1 + p5_3;
			res_vert3_1  =  p3_1 + (p3_2 << 1) + p3_3 - p5_1 - (p5_2 << 1) - p5_3;
			p3_1_sq = (res_horiz3_1 * res_horiz3_1) + (res_vert3_1 * res_vert3_1);
			res = (int)sqrt(p3_1_sq);
			if (res > 255) output[idx4 + 1] = 255;
			else            output[idx4 + 1] = (unsigned char)res;

			res_horiz3_2 = -p3_2 + p3_4 - (p4_2 << 1) + (p4_4 << 1) - p5_2 + p5_4;
			res_vert3_2  =  p3_2 + (p3_3 << 1) + p3_4 - p5_2 - (p5_3 << 1) - p5_4;
			p3_2_sq = (res_horiz3_2 * res_horiz3_2) + (res_vert3_2 * res_vert3_2);
			res = (int)sqrt(p3_2_sq);
			if (res > 255) output[idx4 + 2] = 255;
			else            output[idx4 + 2] = (unsigned char)res;

			res_horiz3_3 = -p3_3 + p3_5 - (p4_3 << 1) + (p4_5 << 1) - p5_3 + p5_5;
			res_vert3_3  =  p3_3 + (p3_4 << 1) + p3_5 - p5_3 - (p5_4 << 1) - p5_5;
			p3_3_sq = (res_horiz3_3 * res_horiz3_3) + (res_vert3_3 * res_vert3_3);
			res = (int)sqrt(p3_3_sq);
			if (res > 255) output[idx4 + 3] = 255;
			else            output[idx4 + 3] = (unsigned char)res;
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
				res = (int)sqrt(p);
				if (res > 255) output[idx1] = 255;
				else            output[idx1] = (unsigned char)res;
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
				res = (int)sqrt(p);
				if (res > 255) output[idx1] = 255;
				else            output[idx1] = (unsigned char)res;
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
				res = (int)sqrt(p);
				if (res > 255) output[idx1] = 255;
				else            output[idx1] = (unsigned char)res;
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
				res = (int)sqrt(p);
				if (res > 255) output[idx1] = 255;
				else            output[idx1] = (unsigned char)res;
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
			res = (int)sqrt(p);
			if (res > 255) output[idx1] = 255;
			else            output[idx1] = (unsigned char)res;
		}
	}

	/* Now run through the output and the golden output to calculate *
	 * the MSE and then the PSNR.									 */
	for (i=1; i<SIZE-1; i++) {
		r = i*SIZE;
		for ( j=1; j<SIZE-16; j+=16 ) {
			idx1 = r + j;
			idx2 = r + j + 1;
			idx3 = r + j + 2;
			idx4 = r + j + 3;
			idx5 = r + j + 4;
			idx6 = r + j + 5;
			idx7 = r + j + 6;
			idx8 = r + j + 7;
			idx9 = r + j + 8;
			idx10 = r + j + 9;
			idx11 = r + j + 10;
			idx12 = r + j + 11;
			idx13 = r + j + 12;
			idx14 = r + j + 13;
			idx15 = r + j + 14;
			idx16 = r + j + 15;
			diff1 = output[idx1] - golden[idx1];
			diff2 = output[idx2] - golden[idx2];
			diff3 = output[idx3] - golden[idx3];
			diff4 = output[idx4] - golden[idx4];
			diff5 = output[idx5] - golden[idx5];
			diff6 = output[idx6] - golden[idx6];
			diff7 = output[idx7] - golden[idx7];
			diff8 = output[idx8] - golden[idx8];
			diff9 = output[idx9] - golden[idx9];
			diff10 = output[idx10] - golden[idx10];
			diff11 = output[idx11] - golden[idx11];
			diff12 = output[idx12] - golden[idx12];
			diff13 = output[idx13] - golden[idx13];
			diff14 = output[idx14] - golden[idx14];
			diff15 = output[idx15] - golden[idx15];
			diff16 = output[idx16] - golden[idx16];
			mse += (diff1 * diff1) + (diff2 * diff2) + (diff3 * diff3) + (diff4 * diff4) + (diff5 * diff5) + (diff6 * diff6) + (diff7 * diff7) + (diff8 * diff8) + (diff9 * diff9) + (diff10 * diff10) + (diff11 * diff11) + (diff12 * diff12) + (diff13 * diff13) + (diff14 * diff14) + (diff15 * diff15) + (diff16 * diff16);
		}
		for (; j<SIZE-1; j++) {
			idx = r + j;
			diff = output[idx] - golden[idx];
			mse += diff * diff;
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

