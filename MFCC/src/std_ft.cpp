#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

char buffer[512];
char tokenizer [] = ",\t\n\0";
static const int vector_max_len =  13;
static const int min_vector_len =  5;
<<<<<<< HEAD
float vector_char[vector_max_len];

int standard_features(const char* file_name, const char* file_name2, int sample_num, bool debug_print){
    float normalize_value=0;
    float instance_value = 0;
    float max_value = 0;
    float min_value = 0;
=======
char vector_char[vector_max_len];
static const int test_num = 54;
static const int vectors_num = 54;


int standard_features(const char* file_name, const char* file_name2, int sample_num, bool debug_print){
    int normalize_value=0;
    int instance_value = 0;
    int max_value = 0;
    int min_value = 0;
>>>>>>> 28054e8e404cd2a322d5a96b8b9108a0d92f0643
    
    FILE *fp = fopen(file_name, "r");
    if(fp == NULL){
        return -1;
        printf("File not found!!!\n");
    }
<<<<<<< HEAD
=======
    FILE *fp2 = fopen(file_name2, "w");
    fclose(fp2);    
>>>>>>> 28054e8e404cd2a322d5a96b8b9108a0d92f0643
    while(!feof(fp)){
        fgets(buffer, sizeof(buffer), fp);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;
<<<<<<< HEAD
        while(tok_buffer!=NULL){
            vector_char[vector_index] = atof(tok_buffer);            
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);            
        }
        for(int i=0; i<vector_max_len-1; i++){                 
=======
        while(tok_buffer != NULL){
            vector_char[vector_index] = (char)atoi(tok_buffer);
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);            
        }
        if(vector_index < min_vector_len) continue;
         for(int i=0; i<vector_max_len; i++){
>>>>>>> 28054e8e404cd2a322d5a96b8b9108a0d92f0643
            if(max_value < vector_char[i]){
                max_value = vector_char[i];            
            }
            if(min_value > vector_char[i]){
                min_value = vector_char[i];            
            }            
<<<<<<< HEAD
        }        
    }
    fclose(fp);
     FILE *fp2 = fopen(file_name, "r");
    if(fp2 == NULL){
        return -1;
        printf("File not found!!!\n");
    }
    FILE *fp3 = fopen(file_name2, "w");
    fclose(fp3);    
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
            // if(debug_print){
            //     printf("%d, ", normalize_value);            
            // }                        
            printf("%d, ", int(normalize_value*256));
            FILE *fp3 = fopen(file_name2, "a+");
            fprintf(fp3, "%d,", normalize_value);
            fclose(fp3);
        }
        printf("\n");
        
        

    }
    fclose(fp2);
=======
        }
        printf("max : %d, min %d : ", max_value, min_value);
        // for(int i=0; i<vector_max_len; i++){
        //     instance_value = vector_char[i];
        //     normalize_value = (instance_value - min_value)/(max_value - min_value);
        //     FILE *fp2 = fopen(file_name2, "a+");
        //     fprintf(fp2, "%d, ", normalize_value);
        //     fclose(fp);
        // }
        
        // if(debug_print){
        //     printf("%d normalize value : ", normalize_value);
        //     for(int i =0; i < vector_index ; i++) printf("%d, ",vector_char[i]);
        //     putchar('\n');
        //     putchar('\n');
        // }

        for(int i =0; i < vector_max_len ; i++) vector_char[i] = 0;

        // if(sample_num > 0 && cat == sample_num + 1) break; // for dubug
    }
    fclose(fp);
>>>>>>> 28054e8e404cd2a322d5a96b8b9108a0d92f0643
    return 0;
}

int main(){
<<<<<<< HEAD
    standard_features("cepstrum2.csv", "feature_nor.txt", 57, true);
}
=======
    standard_features("cepstrum2.txt", "feature_nor.txt", 32, true);
}
>>>>>>> 28054e8e404cd2a322d5a96b8b9108a0d92f0643
