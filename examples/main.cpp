#include <cstddef>
#include <imufungen.h>


void tone(){

  Imufungen sound("beeps_stereo.wav",41000,Imufungen::DEPTH16,Imufungen::STEREO);
   
  for(size_t i = 0;i<5;i++){
   sound.setVolume(Imufungen::SELECT::LEFT,0);
   sound.setVolume(Imufungen::SELECT::RIGHT,1.0);
   sound.addTone(2000,0.5);
   sound.addSilence(0.5);
   sound.setVolume(Imufungen::SELECT::LEFT,1.0);
   sound.setVolume(Imufungen::SELECT::RIGHT,0);
   sound.addTone(2000,0.5);
   sound.addSilence(0.5);
  }
   sound.finish();
}

void sweep(){

   Imufungen sound("ambulance.wav",41000,Imufungen::DEPTH16,Imufungen::MONO);

   for(int i =0;i<10;i++){
     sound.addSweep(700,1500,0.1);
     sound.addSweep(1500,700,0.1);
   }

   sound.finish();
}

void noise(){

   Imufungen sound("whiteNoise.wav",41000,Imufungen::DEPTH16,Imufungen::MONO);

   sound.addNoise(2);

   sound.finish();
}

void chime(){

   Imufungen sound("chimeMelody.wav",41000,Imufungen::DEPTH16,Imufungen::MONO);

   int melody[] = {
     524, 524, 784, 784, 880, 880, 784, 0, 
     698, 698, 660, 660, 588, 588, 524, 0, 
     784, 784, 698, 698, 660, 660, 588, 0, 
     784, 784, 698, 698, 660, 660, 588, 0, 
     524, 524, 784, 784, 880, 880, 784, 0, 
     698, 698, 660, 660, 588, 588, 524, 0
   };

   for(int i = 0;i<48;i++){
     sound.addChime(melody[i],0.3);
   }

   sound.finish();

}

int main(){

   tone();

   sweep();

   noise();

   chime();
}
