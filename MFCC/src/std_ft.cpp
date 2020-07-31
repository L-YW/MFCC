#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "std_ft.h"

char buffer[512];
char tokenizer [] = ",\t\n\0";
static const int vector_max_len =  13;
static const int min_vector_len =  5;
float vector_char[vector_max_len];

standardization::standardization(){}

int standardization::standard_features(const char* cep_filename, const char* feature_filename){
    float normalize_value=0;
    float instance_value = 0;
    float max_value = 0;
    float min_value = 0;
    
    FILE *fp = fopen(cep_filename, "r");
    if(fp == NULL){
        return -1;
        printf("File not found!!!\n");
    }
    while(!feof(fp)){
        fgets(buffer, sizeof(buffer), fp);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;
        while(tok_buffer!=NULL){
            vector_char[vector_index] = atof(tok_buffer);            
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);            
        }
        for(int i=0; i<vector_max_len-1; i++){                 
            if(max_value < vector_char[i]){
                max_value = vector_char[i];            
            }
            if(min_value > vector_char[i]){
                min_value = vector_char[i];            
            }            
        }        
    }
    fclose(fp);
    FILE *fp2 = fopen(cep_filename, "r");
    if(fp2 == NULL){ 
        return -1;       
        printf("File not found!!!\n");
    }
    FILE *fp3 = fopen(feature_filename, "w");    
    while(!feof(fp2)){
        fgets(buffer, sizeof(buffer), fp2);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;
        while(tok_buffer!=NULL){
            vector_char[vector_index] = atof(tok_buffer);                                  
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);            
        }        
        for(int i=0; i<vector_max_len; i++){
            instance_value = vector_char[i];            
            normalize_value = (instance_value - min_value)/(max_value - min_value);                        
            fprintf(fp3, "%d,", int(normalize_value*256));
            
        }
        fprintf(fp3, "\n");
    }
    fclose(fp3);
    fclose(fp2);

}

