#include <stdio.h>
#include "common.h"

void progress_bar(int progress, int max){
    const int bar_width = 30;
    int filled_length = (progress * bar_width) / max;
    int remaining_length = bar_width - filled_length;
    int percentage = (progress*100)/max;
    if (percentage>=100) {
        percentage = 100;
    }
    if(percentage==1){
        for (int i = 0; i<100; i++) {
            printf(" ");
        }
        printf("\r");
    }
    

    printf("[");
    for (int i = 0; i < filled_length; i++) {
        printf("#");
    }
    for (int i = 0; i < remaining_length; i++) {
        printf(".");
    }
    printf("] %3d%%\r", percentage);
    fflush(stdout); 
}