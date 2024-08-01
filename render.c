#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

const float theta_spacing = 0.07;
const float phi_spacing = 0.02;

const float R1 = 1;
const float R2 = 2;
const float K2 = 5;
/*
char *render(float A, float B, int size)
{
    const float K1 = size*K2*3/(R1+R2);
    float cosA = cos(A);
    float cosB = cos(B);
    float sinA = sin(A);
    float sinB = sin(B);
    
    char *output = malloc(size*size);
    *output = ' ';
    float **zbuffer = malloc(sizeof(float*) * size);
    for (int i = 0; i < size; i++)
        zbuffer[i] = calloc(size, sizeof(float));

    for (float theta = 0.0; theta < 2 * 3.14; theta += theta_spacing)
    {
        float costheta = cos(theta);
        float sintheta = sin(theta);

        for (float phi = 0; phi < 2 * 3.14; phi += phi_spacing)
        {
            float cosphi = cos(phi);
            float sinphi = sin(phi);

            float circlex = R2 + R1*costheta;
            float circley = R1*sintheta;

            float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
            float y = circlex * (cosB * cosphi - sinA * sinB * sinphi) + circley * cosA * sinB;
            float z = K2 + cosA * circlex * sinphi + circley * sinA;
            float ooz = 1/z;

            int xp = (int) ((size / 2) + (K1 * ooz * x));
            int yp = (int) ((size / 2) - (K1 * ooz * y));

            float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

            if (L > 0)
            {
                printf("%i-%i\n", xp, yp);
                if  (ooz > zbuffer[xp][yp])
                {
                    zbuffer[xp][yp] = ooz;
                    int luminance_index = L*8;
                    output[(xp * size) + yp] = ".,-~:;=!*#$@"[luminance_index];
                }
            }
        }
    }
    //place the new lines and free zbuffer
    for (int i = 0; i < size; i++)
    {
        output[(size * i) + size] = '\n';
        free(zbuffer[i]);
    }
    free(zbuffer);
    return output;
}
*/
float A = 0, B = 0;
float i, j;
int k;

void render(int size)
{
    float scale = size / 45;
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
            int x = (int) (scale + 40 + 30 * D * (l * h * m - t * n));
            int y= (int) (scale + 12 + 15 * D * (l * h * n + t * m));
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

