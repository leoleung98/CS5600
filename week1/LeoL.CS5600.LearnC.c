/*
* LeoL.CS5600.LearnC.c
* 
* Leo Liang / CS5600 / Northeastern University
* Spring 2023 / Jan 13, 2023
*
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float mpg2kml(float mpg) {
    return mpg * 0.425144;
}

float mpg2lphm(float mpg) {
    return 100 / (mpg2kml(mpg));
}

float kml2mpg(float kml) {
    return kml / 0.425144;
}

int main() {
    printf("good test\n" );
    float data1 = 12.1234;
    float result1 = mpg2kml(data1);
    float result2 = mpg2lphm(data1);
    if(fabs(result1 - data1 * 1.60934 / 3.78541) / result1 < 0.01f) {
        printf("test1 passes: The input is %fmpg and expected to have %.2fkml\n", data1, result1);
    }
    else {
        printf("test1 fails\n");
    }
    
    if(fabs(result2 - 100 / (data1 * 1.60934 / 3.78541)) / result2 < 0.01f) {
        printf("test2 passes: The input is %fmpg and expected to have %.2fphm\n", data1, result2);
    }
    else {
        printf("test2 fails\n");
    }
    
    float data2 = 192.42;
    float result3 = kml2mpg(data2);
    if(fabs(result3 - data2 * 3.78541 / 1.60934) / result3 < 0.01f) {
        printf("test3 passes: The input is %fkml and expected to have %.2fmpg\n", data2, result3);
    }
    else {
        printf("test3 fails\n\n");
    }
}