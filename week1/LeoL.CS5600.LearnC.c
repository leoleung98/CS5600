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

float input;

float mpg2kml() {
    float mpg;
    printf("Please enter the value of mpg to be converted.\n");
    if (scanf("%f", &mpg) != 1) {
        printf("input is invalid\n");
        input = -1;
        char* temp;
        fgets(temp,20,stdin);
        return 0;
    }
    else {
        input = mpg;
        return mpg * 0.425144;
    }
}

float mpg2lphm() {
    float mpg;
    printf("Please enter the value of mpg to be converted.\n");
    if (scanf("%f", &mpg) != 1) {
        printf("input is invalid\n");
        input = -1;
        char* temp;
        fgets(temp,20,stdin);
        return 0; 
    }
    else {
        input = mpg;
        return 100 / (mpg * 0.425144);
    }
}

float kml2mpg() {
    float kml;
    printf("Please enter the value of kml to be converted.\n");
    if (scanf("%f", &kml) != 1) {
        printf("input is invalid\n");
        input = -1;
        char* temp;
        fgets(temp,20,stdin);
        return 0;
    }
    else {
        input = kml;
        return kml / 0.425144;
    }
    
}

int main() {
    printf("good test\n" );
    float result1 = mpg2kml();
    if(fabs(result1 - input * 1.60934 / 3.78541) / result1 < 0.01f) {
        printf("test1 passes: The input is %fmpg and expected to have %.2fkml\n\n", input, result1);
    }
    else {
        printf("test1 fails\n");
    }
    
    float result2 = mpg2lphm();
    if(fabs(result2 - 100 / (input * 1.60934 / 3.78541)) / result2 < 0.01f) {
        printf("test2 passes: The input is %fmpg and expected to have %.2fphm\n\n", input, result2);
    }
    else {
        printf("test2 fails\n");
    }
    
    float result3 = kml2mpg();
    if(fabs(result3 - input * 3.78541 / 1.60934) / result3 < 0.01f) {
        printf("test3 passes: The input is %fkml and expected to have %.2fmpg\n\n", input, result3);
    }
    else {
        printf("test3 fails\n");
    }

    printf("\n\nbad test\n" );
    printf("please don't type in number\n" );
    float result4 = mpg2kml();
    if(result4 ==0) {
        printf("test4 passes: The input is not valid and expected to have output as  0kml\n\n");
    }
    else {
        printf("test4 fails\n");
    }

    float result5 = mpg2lphm();
    if(result5 == 0) {
        printf("test5 passes: The input is not valid and expected to have output as  0phm\n\n");
    }
    else {
        printf("test5 fails\n");
    }
    
    float result6 = kml2mpg();
    if(result6 == 0) {
        printf("test6 passes: The input is not valid and expected to have output as  0mpg\n\n");
    }
    else {
        printf("test6 fails\n");
    }
}