#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

typedef struct LevelData {
    const unsigned char* data;
    const unsigned int width;
    const unsigned int height;
    const unsigned int startX;
    const unsigned int startY;
    
} LevelData;

#endif