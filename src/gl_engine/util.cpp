#include "engine/util.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



char* readFileToString(const char* path) {
    FILE* file = fopen(path, "rt");
    if (file == NULL) { fclose(file); return NULL; }
    
    int count = 0, capacity = 1024;
    int c;
    char* result = (char*)malloc(sizeof(char) * capacity);

    while (fread(&c, sizeof(char), 1, file) != 0)
    {
        if (count == capacity) {
            capacity *= 2;
            result = (char*)realloc(result, sizeof(char) * capacity);
        }

        result[count] = (char)c;
        count++;
    }
    
    result = (char*)realloc(result, sizeof(char) * (count + 1));
    result[count] = '\0';

    fclose(file);
    return result;
}