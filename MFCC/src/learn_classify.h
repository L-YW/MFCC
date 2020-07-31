  
#include <stdint.h>

class learn_and_classify{
private:
    int spi_fd = 0;

public:    
    learn_and_classify();
    int train_intellino(const char* input_train_file, int sample_num, bool debug_print);
    int test_intellino(const char* input_test_file, const char* rsult_file_name, bool debug_print);
    int test_multi(const char* input_test_file, const char* recog_file_name, int sample_num, bool debug_print);
};