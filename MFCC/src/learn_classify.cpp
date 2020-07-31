#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./intellino_spi.h"
#include "learn_classify.h"

using namespace std;
Intellino_spi manager;

char buffer[512];
char tokenizer [] = ",\t\n\0";
static const int vector_max_len =  Intellino_spi::vector_max_len;
static const int min_vector_len =  5;
char vector_char[vector_max_len];
static const int test_num = 54;
static const int vectors_num = 54;

learn_and_classify::learn_and_classify(){}

int learn_and_classify::train_intellino(const char* input_train_file, int sample_num, bool debug_print){    
    FILE *fp = fopen(input_train_file, "r");
    if(fp == NULL){
        return -1;
        printf("File not found!!!\n");
    }
    int cat = 1;
    while(!feof(fp)){
        fgets(buffer, sizeof(buffer), fp);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;
        while(tok_buffer != NULL){
            vector_char[vector_index] = (char)atoi(tok_buffer);
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);            
        }
        if(vector_index < min_vector_len) continue;

        manager.learn(vector_index, vector_char, cat);

        if(debug_print){
            printf("%d VECTOR : ", cat);
            for(int i =0; i < vector_index ; i++) printf("%d, ",vector_char[i]);
            putchar('\n');
            putchar('\n');
        }

        for(int i =0; i < vector_max_len ; i++) vector_char[i] = 0;

        // cat++;
        // if(sample_num > 0 && cat == sample_num + 1) break; // for dubug
    }
    fclose(fp);
    return 0;
}

int learn_and_classify::test_intellino(const char* input_test_file, const char* result_file_name, bool debug_print){
    FILE *fp = fopen(input_test_file, "r");
    if(fp == NULL){
        return -1;
        printf("File not found!!!\n");
    }
    int cat = 1;
    int ret_dist =0, ret_cat=0;
    int index = 1;
    FILE *fp2 = fopen(result_file_name, "w");
    
    while(!feof(fp)){
        fgets(buffer, sizeof(buffer), fp);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;
        while(tok_buffer != NULL){
            vector_char[vector_index] = (char)atoi(tok_buffer) + 10;
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);
        }
        if(vector_index < min_vector_len) continue;

        manager.classify(vector_index, vector_char, &ret_dist, &ret_cat);
        int j=vectors_num-1;                                    
        if(debug_print){
            printf("VECTOR : ");
            for(int i =0; i < vector_index ; i++) printf("%d, ",vector_char[i]);
            putchar('\n');
            printf("Expected Cat : %d, Distance : %d, Category : %d\n", cat, ret_dist, ret_cat);
            }                                                 
        if(fp2 == NULL){
            return -1;
            printf("Results File not found!!!\n");
        }
        fprintf(fp2, "%d, %d, %d,\n", index, ret_dist, ret_cat);
        index++;                                  
    }
    fclose(fp2);
    fclose(fp);
    return 0;
}

int learn_and_classify::test_multi(const char* input_test_file, const char* recog_file_name, int sample_num, bool debug_print){             
    FILE *fp = fopen(input_test_file, "r");    
    if(fp == NULL){
        return -1;
        printf("fp File not found!!!\n");
    }
    int line_num = 1;
    int ret_dist[vector_max_len];
    int ret_cat[vector_max_len];
    int vectors_id = 0;
    char vectors[vectors_num][vector_max_len] = {0};
    char vector_buffer[vector_max_len] = {0};
    int count = 0;
    int data_num = 0;
    int rotation_num = 0;
    int extra_data_num = 0;    
    
    while(!feof(fp)){
        fgets(buffer, sizeof(buffer), fp);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;        
        while(tok_buffer != NULL){
            vectors[vectors_id][vector_index]  = (char)atoi(tok_buffer);
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);
        }
        if(vector_index < min_vector_len){
            printf("[WARNING] A line number %d does not have more %d elements\n", line_num, min_vector_len);
            data_num = line_num - 1;
            rotation_num = data_num / vectors_num; 
            extra_data_num = data_num - (rotation_num * vectors_num);            
            line_num = 1;
            break;
        }
        vector_index = 0;
        line_num++;
    }
    fclose(fp);
    FILE *fp2 = fopen(input_test_file, "r");
    if(fp2 == NULL){
        return -1;
        printf("fp2 File not found!!!\n");
    }
    FILE *fp3;
    fp3 = fopen("../data/recog_result.txt", "w"); 
    if(fp3 == NULL){
        return -1;
        printf("fp3 File not found!!!\n");
    }
    fclose(fp3);

    while(!feof(fp2)){
        fgets(buffer, sizeof(buffer), fp2);
        char* tok_buffer = (char*)strtok(buffer, tokenizer);
        int vector_index = 0;
        while(tok_buffer != NULL){
            vectors[vectors_id][vector_index]  = (char)atoi(tok_buffer);
            vector_index++;
            tok_buffer = strtok(NULL, tokenizer);
        }
        if(vector_index < min_vector_len){
            printf("[WARNING] B line number %d does not have more %d elements\n", line_num, min_vector_len);
            break;
        }        
        vectors_id++;        
        if(count != rotation_num){
            if(vectors_id == vectors_num){
                vectors_id = 0;
                count ++;
                manager.classify_multi(vectors_num, vector_index, vectors, ret_dist, ret_cat);            
                int j=vectors_num-1;                                    
                for(int i=0; i < vectors_num ; i++){ 
                    if(debug_print){                                               
                        printf("VECTOR : ");
                        for(int j =0; j < vector_index; j++) printf("%d, ",vectors[i][j]);
                        putchar('\n');
                        printf("Index : %d, Distance : %d, Category : %d\n", line_num-j, ret_dist[i], ret_cat[i]);                            
                        putchar('\n');                                               
                    }
                    int index = line_num-j-1;
                    if(index==1){                    
                        fp3 = fopen(recog_file_name, "w");                    
                        if(fp3 == NULL){
                            return -1;
                            printf("fp3 File not found!!!\n");
                        }
                        fprintf(fp3, "%d, %d, %d,\n", index, ret_dist[i], ret_cat[i]);
                        fclose(fp3);                                  
                        j--;
                    }
                    else {                    
                        fp3 = fopen(recog_file_name, "a");                    
                        if(fp3 == NULL){
                            return -1;
                            printf("fp3 File not found!!!\n");
                        }
                        fprintf(fp3, "%d, %d, %d,\n", index, ret_dist[i], ret_cat[i]);
                        fclose(fp3);                                  
                        j--;
                    }
                }                                
            }
        }
        else if(count == rotation_num){
            int vectors_num_temp = extra_data_num;
            if(vectors_id == vectors_num_temp){
                vectors_id = 0;                
                manager.classify_multi(vectors_num_temp, vector_index, vectors, ret_dist, ret_cat);
                int j=vectors_num_temp-1;                                    
                for(int i=0; i < vectors_num_temp ; i++){ 
                    if(debug_print){                                               
                        printf("VECTOR : ");
                        for(int j =0; j < vector_index; j++) printf("%d, ",vectors[i][j]);
                        putchar('\n');
                        printf("Index : %d, Distance : %d, Category : %d\n", line_num-j, ret_dist[i], ret_cat[i]);                            
                        putchar('\n');                                               
                    }
                    int index = line_num-j-1;                    
                    fp3=fopen(recog_file_name, "a");                    
                    if(fp3 == NULL){
                        return -1;
                        printf("fp3 File not found!!!\n");
                    }
                    fprintf(fp3, "%d, %d, %d,\n", index, ret_dist[i], ret_cat[i]);
                    fclose(fp3);                                          
                    j--; 
                } 
            }
        }        
        vector_index = 0;
        line_num++;
        if(sample_num > 0 && line_num >= sample_num + 1) break; // for dubug
    }        
    fclose(fp2);   
    return 0;
}