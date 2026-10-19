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
	int idx3, idx4, idx5, idx6, idx7, idx8, idx9, idx10, idx11, idx12, idx13, idx14, idx15, idx16;
	int p00, p01, p02, p10, p12, p20, p21, p22;
	int p0_0, p0_1, p0_2, p0_3, p0_4, p0_5, p0_6, p0_7, p0_8, p0_9, p0_10, p0_11, p0_12, p0_13, p0_14, p0_15, p0_16, p0_17;
	int p1_0, p1_1, p1_2, p1_3, p1_4, p1_5, p1_6, p1_7, p1_8, p1_9, p1_10, p1_11, p1_12, p1_13, p1_14, p1_15, p1_16, p1_17;
	int p2_0, p2_1, p2_2, p2_3, p2_4, p2_5, p2_6, p2_7, p2_8, p2_9, p2_10, p2_11, p2_12, p2_13, p2_14, p2_15, p2_16, p2_17;
	unsigned int p;
	unsigned long long mse = 0;
	unsigned int p0_0_sq, p0_1_sq, p0_2_sq, p0_3_sq, p0_4_sq, p0_5_sq, p0_6_sq, p0_7_sq, p0_8_sq, p0_9_sq, p0_10_sq, p0_11_sq, p0_12_sq, p0_13_sq, p0_14_sq, p0_15_sq;
	int res, res_horiz, res_vert, diff;
	int res_horiz0_0, res_horiz0_1, res_horiz0_2, res_horiz0_3, res_horiz0_4, res_horiz0_5, res_horiz0_6, res_horiz0_7, res_horiz0_8, res_horiz0_9, res_horiz0_10, res_horiz0_11, res_horiz0_12, res_horiz0_13, res_horiz0_14, res_horiz0_15;
	int res_vert0_0, res_vert0_1, res_vert0_2, res_vert0_3, res_vert0_4, res_vert0_5, res_vert0_6, res_vert0_7, res_vert0_8, res_vert0_9, res_vert0_10, res_vert0_11, res_vert0_12, res_vert0_13, res_vert0_14, res_vert0_15;
	int res0_0, res0_1, res0_2, res0_3, res0_4, res0_5, res0_6, res0_7, res0_8, res0_9, res0_10, res0_11, res0_12, res0_13, res0_14, res0_15;
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
	for (i=1; i<SIZE-1; i+=1 ) {
		r0 = (i - 1)*SIZE;
		r1 = i*SIZE;
		r2 = (i + 1)*SIZE;
		for (j=1; j<SIZE-16; j+=16) {
			idx0 = r0 + j;
			idx1 = r1 + j;
			idx2 = r2 + j;

			p0_0 = input[idx0 - 1];
			p0_1 = input[idx0];
			p0_2 = input[idx0 + 1];
			p0_3 = input[idx0 + 2];
			p0_4 = input[idx0 + 3];
			p0_5 = input[idx0 + 4];
			p0_6 = input[idx0 + 5];
			p0_7 = input[idx0 + 6];
			p0_8 = input[idx0 + 7];
			p0_9 = input[idx0 + 8];
			p0_10 = input[idx0 + 9];
			p0_11 = input[idx0 + 10];
			p0_12 = input[idx0 + 11];
			p0_13 = input[idx0 + 12];
			p0_14 = input[idx0 + 13];
			p0_15 = input[idx0 + 14];
			p0_16 = input[idx0 + 15];
			p0_17 = input[idx0 + 16];

			p1_0 = input[idx1 - 1];
			p1_1 = input[idx1];
			p1_2 = input[idx1 + 1];
			p1_3 = input[idx1 + 2];
			p1_4 = input[idx1 + 3];
			p1_5 = input[idx1 + 4];
			p1_6 = input[idx1 + 5];
			p1_7 = input[idx1 + 6];
			p1_8 = input[idx1 + 7];
			p1_9 = input[idx1 + 8];
			p1_10 = input[idx1 + 9];
			p1_11 = input[idx1 + 10];
			p1_12 = input[idx1 + 11];
			p1_13 = input[idx1 + 12];
			p1_14 = input[idx1 + 13];
			p1_15 = input[idx1 + 14];
			p1_16 = input[idx1 + 15];
			p1_17 = input[idx1 + 16];

			p2_0 = input[idx2 - 1];
			p2_1 = input[idx2];
			p2_2 = input[idx2 + 1];
			p2_3 = input[idx2 + 2];
			p2_4 = input[idx2 + 3];
			p2_5 = input[idx2 + 4];
			p2_6 = input[idx2 + 5];
			p2_7 = input[idx2 + 6];
			p2_8 = input[idx2 + 7];
			p2_9 = input[idx2 + 8];
			p2_10 = input[idx2 + 9];
			p2_11 = input[idx2 + 10];
			p2_12 = input[idx2 + 11];
			p2_13 = input[idx2 + 12];
			p2_14 = input[idx2 + 13];
			p2_15 = input[idx2 + 14];
			p2_16 = input[idx2 + 15];
			p2_17 = input[idx2 + 16];

			res_horiz0_0 = -p0_0 + p0_2 - (p1_0 << 1) + (p1_2 << 1) - p2_0 + p2_2;
			res_vert0_0  =  p0_0 + (p0_1 << 1) + p0_2 - p2_0 - (p2_1 << 1) - p2_2;
			p0_0_sq = (res_horiz0_0 * res_horiz0_0) + (res_vert0_0 * res_vert0_0);
			res0_0 = (int)sqrt(p0_0_sq);
			if (res0_0 > 255) output[idx1] = 255;
			else            output[idx1] = (unsigned char)res0_0;

			res_horiz0_1 = -p0_1 + p0_3 - (p1_1 << 1) + (p1_3 << 1) - p2_1 + p2_3;
			res_vert0_1  =  p0_1 + (p0_2 << 1) + p0_3 - p2_1 - (p2_2 << 1) - p2_3;
			p0_1_sq = (res_horiz0_1 * res_horiz0_1) + (res_vert0_1 * res_vert0_1);
			res0_1 = (int)sqrt(p0_1_sq);
			if (res0_1 > 255) output[idx1 + 1] = 255;
			else            output[idx1 + 1] = (unsigned char)res0_1;

			res_horiz0_2 = -p0_2 + p0_4 - (p1_2 << 1) + (p1_4 << 1) - p2_2 + p2_4;
			res_vert0_2  =  p0_2 + (p0_3 << 1) + p0_4 - p2_2 - (p2_3 << 1) - p2_4;
			p0_2_sq = (res_horiz0_2 * res_horiz0_2) + (res_vert0_2 * res_vert0_2);
			res0_2 = (int)sqrt(p0_2_sq);
			if (res0_2 > 255) output[idx1 + 2] = 255;
			else            output[idx1 + 2] = (unsigned char)res0_2;

			res_horiz0_3 = -p0_3 + p0_5 - (p1_3 << 1) + (p1_5 << 1) - p2_3 + p2_5;
			res_vert0_3  =  p0_3 + (p0_4 << 1) + p0_5 - p2_3 - (p2_4 << 1) - p2_5;
			p0_3_sq = (res_horiz0_3 * res_horiz0_3) + (res_vert0_3 * res_vert0_3);
			res0_3 = (int)sqrt(p0_3_sq);
			if (res0_3 > 255) output[idx1 + 3] = 255;
			else            output[idx1 + 3] = (unsigned char)res0_3;

			res_horiz0_4 = -p0_4 + p0_6 - (p1_4 << 1) + (p1_6 << 1) - p2_4 + p2_6;
			res_vert0_4  =  p0_4 + (p0_5 << 1) + p0_6 - p2_4 - (p2_5 << 1) - p2_6;
			p0_4_sq = (res_horiz0_4 * res_horiz0_4) + (res_vert0_4 * res_vert0_4);
			res0_4 = (int)sqrt(p0_4_sq);
			if (res0_4 > 255) output[idx1 + 4] = 255;
			else            output[idx1 + 4] = (unsigned char)res0_4;

			res_horiz0_5 = -p0_5 + p0_7 - (p1_5 << 1) + (p1_7 << 1) - p2_5 + p2_7;
			res_vert0_5  =  p0_5 + (p0_6 << 1) + p0_7 - p2_5 - (p2_6 << 1) - p2_7;
			p0_5_sq = (res_horiz0_5 * res_horiz0_5) + (res_vert0_5 * res_vert0_5);
			res0_5 = (int)sqrt(p0_5_sq);
			if (res0_5 > 255) output[idx1 + 5] = 255;
			else            output[idx1 + 5] = (unsigned char)res0_5;

			res_horiz0_6 = -p0_6 + p0_8 - (p1_6 << 1) + (p1_8 << 1) - p2_6 + p2_8;
			res_vert0_6  =  p0_6 + (p0_7 << 1) + p0_8 - p2_6 - (p2_7 << 1) - p2_8;
			p0_6_sq = (res_horiz0_6 * res_horiz0_6) + (res_vert0_6 * res_vert0_6);
			res0_6 = (int)sqrt(p0_6_sq);
			if (res0_6 > 255) output[idx1 + 6] = 255;
			else            output[idx1 + 6] = (unsigned char)res0_6;

			res_horiz0_7 = -p0_7 + p0_9 - (p1_7 << 1) + (p1_9 << 1) - p2_7 + p2_9;
			res_vert0_7  =  p0_7 + (p0_8 << 1) + p0_9 - p2_7 - (p2_8 << 1) - p2_9;
			p0_7_sq = (res_horiz0_7 * res_horiz0_7) + (res_vert0_7 * res_vert0_7);
			res0_7 = (int)sqrt(p0_7_sq);
			if (res0_7 > 255) output[idx1 + 7] = 255;
			else            output[idx1 + 7] = (unsigned char)res0_7;

			res_horiz0_8 = -p0_8 + p0_10 - (p1_8 << 1) + (p1_10 << 1) - p2_8 + p2_10;
			res_vert0_8  =  p0_8 + (p0_9 << 1) + p0_10 - p2_8 - (p2_9 << 1) - p2_10;
			p0_8_sq = (res_horiz0_8 * res_horiz0_8) + (res_vert0_8 * res_vert0_8);
			res0_8 = (int)sqrt(p0_8_sq);
			if (res0_8 > 255) output[idx1 + 8] = 255;
			else            output[idx1 + 8] = (unsigned char)res0_8;

			res_horiz0_9 = -p0_9 + p0_11 - (p1_9 << 1) + (p1_11 << 1) - p2_9 + p2_11;
			res_vert0_9  =  p0_9 + (p0_10 << 1) + p0_11 - p2_9 - (p2_10 << 1) - p2_11;
			p0_9_sq = (res_horiz0_9 * res_horiz0_9) + (res_vert0_9 * res_vert0_9);
			res0_9 = (int)sqrt(p0_9_sq);
			if (res0_9 > 255) output[idx1 + 9] = 255;
			else            output[idx1 + 9] = (unsigned char)res0_9;

			res_horiz0_10 = -p0_10 + p0_12 - (p1_10 << 1) + (p1_12 << 1) - p2_10 + p2_12;
			res_vert0_10  =  p0_10 + (p0_11 << 1) + p0_12 - p2_10 - (p2_11 << 1) - p2_12;
			p0_10_sq = (res_horiz0_10 * res_horiz0_10) + (res_vert0_10 * res_vert0_10);
			res0_10 = (int)sqrt(p0_10_sq);
			if (res0_10 > 255) output[idx1 + 10] = 255;
			else            output[idx1 + 10] = (unsigned char)res0_10;

			res_horiz0_11 = -p0_11 + p0_13 - (p1_11 << 1) + (p1_13 << 1) - p2_11 + p2_13;
			res_vert0_11  =  p0_11 + (p0_12 << 1) + p0_13 - p2_11 - (p2_12 << 1) - p2_13;
			p0_11_sq = (res_horiz0_11 * res_horiz0_11) + (res_vert0_11 * res_vert0_11);
			res0_11 = (int)sqrt(p0_11_sq);
			if (res0_11 > 255) output[idx1 + 11] = 255;
			else            output[idx1 + 11] = (unsigned char)res0_11;

			res_horiz0_12 = -p0_12 + p0_14 - (p1_12 << 1) + (p1_14 << 1) - p2_12 + p2_14;
			res_vert0_12  =  p0_12 + (p0_13 << 1) + p0_14 - p2_12 - (p2_13 << 1) - p2_14;
			p0_12_sq = (res_horiz0_12 * res_horiz0_12) + (res_vert0_12 * res_vert0_12);
			res0_12 = (int)sqrt(p0_12_sq);
			if (res0_12 > 255) output[idx1 + 12] = 255;
			else            output[idx1 + 12] = (unsigned char)res0_12;

			res_horiz0_13 = -p0_13 + p0_15 - (p1_13 << 1) + (p1_15 << 1) - p2_13 + p2_15;
			res_vert0_13  =  p0_13 + (p0_14 << 1) + p0_15 - p2_13 - (p2_14 << 1) - p2_15;
			p0_13_sq = (res_horiz0_13 * res_horiz0_13) + (res_vert0_13 * res_vert0_13);
			res0_13 = (int)sqrt(p0_13_sq);
			if (res0_13 > 255) output[idx1 + 13] = 255;
			else            output[idx1 + 13] = (unsigned char)res0_13;

			res_horiz0_14 = -p0_14 + p0_16 - (p1_14 << 1) + (p1_16 << 1) - p2_14 + p2_16;
			res_vert0_14  =  p0_14 + (p0_15 << 1) + p0_16 - p2_14 - (p2_15 << 1) - p2_16;
			p0_14_sq = (res_horiz0_14 * res_horiz0_14) + (res_vert0_14 * res_vert0_14);
			res0_14 = (int)sqrt(p0_14_sq);
			if (res0_14 > 255) output[idx1 + 14] = 255;
			else            output[idx1 + 14] = (unsigned char)res0_14;

			res_horiz0_15 = -p0_15 + p0_17 - (p1_15 << 1) + (p1_17 << 1) - p2_15 + p2_17;
			res_vert0_15  =  p0_15 + (p0_16 << 1) + p0_17 - p2_15 - (p2_16 << 1) - p2_17;
			p0_15_sq = (res_horiz0_15 * res_horiz0_15) + (res_vert0_15 * res_vert0_15);
			res0_15 = (int)sqrt(p0_15_sq);
			if (res0_15 > 255) output[idx1 + 15] = 255;
			else            output[idx1 + 15] = (unsigned char)res0_15;
		}

		for (; j<SIZE-1; j++) {
			idx0 = r0 + j;
			idx1 = r1 + j;
			idx2 = r2 + j;
			/* Apply the sobel filter and calculate the magnitude *
			 * of the derivative.								  */
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
			/* If the resulting value is greater than 255, clip it *
			 * to 255.											   */
			if (res > 255)
				output[idx1] = 255;      
			else
				output[idx1] = (unsigned char)res;
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

