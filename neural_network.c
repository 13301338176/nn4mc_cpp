#include "neural_network.h"
//#include "adc_collector.h"
#include "neural_network_params.h"
#include <stdlib.h>

#define max(a, b) (((a)>(b) ? (a) : (b)))
#define min(a, b) (((a)<(b) ? (a) : (b)))

void set_conv1D(struct Conv1D * LL, int input_sh1, int input_sh2, int kernel_size, int filters){
    // These will emulate the constructors
    struct Conv1D L; 
    L= *LL;

    L.input_sh1= input_sh1;
    L.input_sh2= input_sh2;
    L.kernel_size = kernel_size;

    L.filters= filters;
    L.h = (float **)malloc((input_sh1-L.kernel_size+1) * sizeof(float*));
    for (int i=0; i< (input_sh1- kernel_size+1); i++){
        L.h[i] = (float*)malloc(filters * sizeof(float));
    }
    num_layers++;
    L.output_shape= (int)((input_sh1-kernel_size+1)/2.0);

    *LL = L;
}

void fwd_conv1D(struct Conv1D * LL, int a, int bb, int c, const float W[a][bb][c], const float * b, int w1, int w2, float window[w1][w2]){
   struct Conv1D L;
   L= *LL;
    for (int i=0; i<L.input_sh1-L.kernel_size+1; i++){
        for (int j=0; j<L.filters; j++){
            L.h[i][j]= b[j];
            for (int x=0; x<L.kernel_size; x++){
                for (int y=0; y<NUM_ADC; y++){
                    L.h[i][j] += W[x][y][j] * window[i+x][y];
                    printf("%.6f", L.h[i][j]);
                }    
                printf("\n");
            }
        }
    }
    *LL = L;
}

void set_maxpool1D(struct MaxPooling1D * LL, int input_sh1,int input_sh2, int pool_size, int strides){
    struct MaxPooling1D L;
    L= *LL;
    // These will emulate the constructors
    L.pool_size = pool_size;
    L.strides= strides;
    L.input_sh1 = input_sh1;
    L.input_sh2= input_sh2;
    num_layers++;
    L.h= malloc((int)(L.input_sh1 * sizeof(float*)));
    for (int i=0; i<(int)L.input_sh1; i++){
        L.h[i]= malloc(pool_size * sizeof(float));
    }
    L.output_shape= pool_size;
    L.input_sh1= L.input_sh1;
    L.input_sh2= pool_size;
    *LL = L;
}

void fwd_maxpool1D(struct MaxPooling1D * LL, int a, int bb, int c, const float W[a][bb][c], const float *b, int w1, int w2, float window[w1][w2]){
    
    struct MaxPooling1D L;
    L= *LL;
    for (int i=0; i< L.input_sh1; i++){
        for (int j=0; j<L.pool_size; j++){
            L.h[i][j] = max(window[2*i][j], window[2*i+1][j]);
        }
    }
    *LL = L;
}

void set_dense(struct Dense * LL, int input_size, int output_size, char activation){
    // These will emulate the constructor
    struct Dense L;
   L= *LL;
   L.input_size= input_size;
   L.output_size= output_size;
   L.activation = activation;
   num_layers++;
   L.h= malloc(output_size * sizeof (float));
   *LL = L;
}

void fwd_dense(struct Dense * LL, int a, int bb, const float W[a][bb], const float * b, float window[a]){
    struct Dense L;
    L= *LL;
    for (int i=0; i<L.output_size; i++){
        L.h[i] = b[i];
        for (int j=0; j<L.input_size; j++){
            L.h[i]+= W[j][i] * window[j]; 
        }
        if (L.activation=='r'){
            L.h[i]= max(L.h[i], 0.0);
        } 
    }
    *LL = L;
}

void flatten2D1DfromConv(struct Flatten2D1D * FLATFLAT, struct Conv1D PREV){
    struct Flatten2D1D FLAT;
    FLAT = *FLATFLAT;
    FLAT.in_shape_0 = sizeof(PREV.h)/ sizeof(PREV.h[0]);
    FLAT.in_shape_1 = sizeof(PREV.h[0])/FLAT.in_shape_0;

    FLAT.h= malloc(FLAT.in_shape_0*FLAT.in_shape_1 * sizeof(float));

    for (int i=0; i<FLAT.in_shape_0; i++){
        for (int j=0; j<FLAT.in_shape_1; j++){
            int idx= FLAT.in_shape_0*i+j;
            FLAT.h[idx] = PREV.h[i][j];
        }
    }
    FLAT.output_size= FLAT.in_shape_0*FLAT.in_shape_1;

    *FLATFLAT= FLAT;
}

void flatten2D1D(struct Flatten2D1D * FLATFLAT, struct MaxPooling1D PREV){
    struct Flatten2D1D FLAT;
    FLAT= *FLATFLAT;
    FLAT.in_shape_0 = sizeof(PREV.h)/ sizeof(PREV.h[0]);
    FLAT.in_shape_1 = sizeof(PREV.h[0])/FLAT.in_shape_0;

    FLAT.h= malloc(FLAT.in_shape_0*FLAT.in_shape_1 * sizeof(float));

    for (int i=0; i<FLAT.in_shape_0; i++){
        for (int j=0; j<FLAT.in_shape_1; j++){
            int idx= FLAT.in_shape_0*i+j;
            FLAT.h[idx] = PREV.h[i][j];
        }
    }
    FLAT.output_size= FLAT.in_shape_0*FLAT.in_shape_1;
    *FLATFLAT= FLAT;
}

void neural_network_forward()
{
    // This will feed forward the neural newtork. 
    // First, we need to declare the structures 
    // for each layer. 
    float window[50][2]= {{1, 1}, {1, 1}, {1,1}, {1, 1}, {1, 1},{1, 1}, {1, 1}, {1,1}, {1, 1},{ 1, 1},{1, 1}, {1, 1}, {1,1}, {1, 1}, {1, 1},{1, 1},{ 1, 1}, {1,1}, {1, 1},{ 1, 1},{1, 1}, {1, 1}, {1,1},{ 1, 1}, {1, 1},
                        {1, 1}, {1, 1}, {1,1}, {1, 1}, {1, 1},{1, 1}, {1, 1}, {1,1}, {1, 1},{ 1, 1},{1, 1}, {1, 1}, {1,1}, {1, 1}, {1, 1},{1, 1},{ 1, 1}, {1,1}, {1, 1},{ 1, 1},{1, 1}, {1, 1}, {1,1},{ 1, 1}, {1, 1}};

    struct Conv1D L1;
    set_conv1D(&L1, WINDOW_SIZE, NUM_ADC, 4, 8);
    fwd_conv1D(&L1, 4, 2, 8, W_0, b_0, L1.input_sh1, L1.input_sh2, window);
    struct Conv1D L2;
    set_conv1D(&L2, L1.output_shape, 8, 4, 8);
    fwd_conv1D(&L2, 4, 8, 8, W_1, b_1, L2.input_sh1, L2.input_sh2, L1.h);
    struct Flatten2D1D FL;
    flatten2D1DfromConv(&FL, L2);
    struct Dense D1;
    set_dense(&D1, FL.output_size, 64, 'r');
    fwd_dense(&D1, D1.input_size, D1.output_size, W_2, b_2, FL.h);
    struct Dense D2;
    set_dense(&D2, D1.output_size, 32, 'r');
    fwd_dense(&D2, D2.input_size, D2.output_size, W_3, b_3, D1.h);
    struct Dense D3;
    set_dense(&D3, D2.output_size, 16, 'r');
    fwd_dense(&D3, D3.input_size, D3.output_size, W_4, b_4, D2.h);
    struct Dense D4;
    set_dense(&D4, D3.output_size, 8, 'r');
    fwd_dense(&D4, D4.input_size, D4.output_size, W_5, b_5, D3.h);
    struct Dense D5;
    set_dense(&D5, D4.output_size, 3, 'n');
    fwd_dense(&D5, D5.input_size, D5.output_size, W_6, b_6, D4.h);
}

