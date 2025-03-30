#include "imufungen.h"

int main(){

    Imufungen sound("file.wav",41000,Imufungen::DEPTH16,Imufungen::MONO);

    sound.addTone(1000,1.0);

    sound.finish();

    return 0;
}
