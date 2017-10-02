#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SAMPLE_RATE 44100
#define TIME_CNT 10
#define SAMPLE_CNT (SAMPLE_RATE * TIME_CNT)
#define PI 3.14159265358979323846
typedef struct 
{
    char riff_mark[4];
    uint32_t file_size;
    char file_type[4];
    char format_chunk_marker[4];
    uint32_t format_data_length;
    uint16_t format_type;
    uint16_t channel_no;
    uint32_t sample_rate;
    uint32_t bit_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_header[4];
    uint32_t data_file_size;
} __attribute__((packed)) wav_header;

typedef int16_t sample_16;

sample_16 samples[SAMPLE_CNT];

double randr(unsigned int min, unsigned int max)
{
       double scaled = (double)rand()/RAND_MAX;
       return (max - min +1)*scaled + min;
}

void create_noise(double amplitude){
    srand(time(0));
    int i = 0;
    for (i = 0; i < SAMPLE_CNT; i++){
        samples[i]=randr(0, amplitude);
    }
}
void create_sine(double frequency, double amplitude){
    int i = 0;
    for (i = 0; i < SAMPLE_CNT; i++){
        samples[i]=sin(2*M_PI*frequency*i/SAMPLE_RATE)*amplitude;
    }
}

void create_saw(double frequency, double amplitude){
    int i = 0;
    for (i = 0; i < SAMPLE_CNT; i++){
        samples[i]=fmod(frequency*i/SAMPLE_RATE, 1.0)*amplitude;
    }
}

void create_square(double frequency, double amplitude){
    int i = 0;
    for (i = 0; i < SAMPLE_CNT; i++){
        samples[i]=sin(2*M_PI*frequency*i/SAMPLE_RATE) >= 0 ? amplitude : -1*amplitude;
    }
}

void write_wav_file(wav_header * header, char * filename){
    FILE * f = fopen(filename, "w+");
    int i = 0;
    fwrite(header, sizeof(wav_header), 1, f);
    for (i = 0; i < SAMPLE_CNT; i++){
        fwrite(&samples[i], sizeof(sample_16), 1, f);
    } 
    fclose(f);
}

void load_wav_header(wav_header * wav_file, int data_file_size){
    strncpy(wav_file->riff_mark, "RIFF", 4);
    strncpy(wav_file->file_type, "WAVE", 4);
    strncpy(wav_file->format_chunk_marker, "fmt ", 4);
    wav_file->format_data_length=16;
    wav_file->format_type=1;
    wav_file->channel_no=2;
    wav_file->sample_rate=SAMPLE_RATE;
    wav_file->bits_per_sample=16;
    wav_file->bit_rate=wav_file->sample_rate * wav_file->channel_no * wav_file->bits_per_sample / 8;
    wav_file->block_align=wav_file->bits_per_sample * wav_file->channel_no / 8;
    strncpy(wav_file->data_header, "data", 4);
    wav_file->data_file_size=data_file_size;
    wav_file->file_size=wav_file->data_file_size+sizeof(wav_header);
    printf("%s%d\n","size of struct header: ", sizeof(wav_header));
} 

typedef enum {
    SINE = 0,
    SAW = 1,
    SQUARE = 2,
    NOISE = 3
} wave_type;

int main(int argc, char * argv[]){
    printf("%s\n", "creating wav file");
    printf("%s%d\n", "samples:", SAMPLE_CNT);
    wav_header wav_header_test;
    double frequency=100;
    double amplitude=5000;
    wave_type wavetype = 0; 
    int i = 0;
    for (i=0; i < argc; i++){ 
        printf("%s\n", argv[i]);
    }
    if (argc > 1){
        frequency = strtod(argv[1], 0);
    }
    if (argc > 2){
        amplitude = strtod(argv[2], 0);
    }
    if (argc > 3){
        wavetype = strtod(argv[3], 0);
    }

    switch (wavetype){
        case NOISE:
            create_noise(frequency);
            break;
        case SINE:
            create_sine(frequency, amplitude);
            break;
        case SAW:
            create_saw(frequency, amplitude);
            break;
        case SQUARE:
            create_square(frequency, amplitude);
            break;
    }

    load_wav_header(&wav_header_test, SAMPLE_CNT);
    write_wav_file(&wav_header_test, "mywav.wav");
//    for (i = 0; i < 2000; i++){
//        int j = 0;
//        for (j = 0; j < 500+(int)(samples[i]/300); j++){
//            printf("%s", " ");
//        }
//        printf("%s\n", "-");
//    } 
    printf("%s\n", "Wave Info:");
    printf("%s%f\n", "  Frequency:", frequency);
    printf("%s%f\n", "  Amplitude:", amplitude);
    printf("%s%d\n", "  Wave Type:", wavetype);

}
