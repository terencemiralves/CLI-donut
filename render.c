#include "render.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

const float R1 = 1;
const float R2 = 2;
const float K2 = 5;
float A = 0, B = 0;
float i, j;
int k;

void render(int size)
{
    float z[1760];
    char b[1760];
    printf("\x1b[2J");
    memset(b, 32, 1760);
    memset(z, 0, 7040);
    for(j=0; j < 6.28; j += 0.07) 
    {
        for(i=0; i < 6.28; i += 0.02) 
        {
            float c = sin(i);
            float d = cos(j);
            float e = sin(A);
            float f = sin(j);
            float g = cos(A);
            float h = d + 2;
            float D = 1 / (c * h * e + f * g + 5);
            float l = cos(i);
            float m = cos(B);
            float n = sin(B);
            float t = c * h * g - f * e;
            int x = (int) (40 + size * D * (l * h * m - t * n));
            int y= (int) (12 + (size / 2) * D * (l * h * n + t * m));
            int o = x + 80 * y;
            int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
            if(22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                z[o] = D;
                b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
            }
        }
    }
    printf("\x1b[H");
    for(k = 0; k < 1761; k++) 
    {
        putchar(k % 80 ? b[k] : 10);
        A += 0.00004;
        B += 0.00002;
    }
}

