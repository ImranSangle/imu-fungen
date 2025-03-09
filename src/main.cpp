#include "imufungen.h"

int main(){

    Imufungen sound("file.wav",41000,Imufungen::DEPTH16,Imufungen::MONO);

    sound.addTone(3000,0.07);
    
    sound.finish();

    return 0;
}
