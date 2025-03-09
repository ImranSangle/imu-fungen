#include <math.h>
#include <stdexcept>
#include <unistd.h>
#include <fstream>

#include "imufungen.h"
#include <mathfunctions.h>


Imufungen::Imufungen(const std::string& filename,float sampleRate,DEPTH bitDepth,CHANNELS channels){
  this->m_filename = filename;
  this->m_SampleRate = sampleRate;
  this->m_bitDepth = bitDepth;
  this->m_channels = channels;
  
  output.open(this->m_filename,std::ios::binary);

  if(output.is_open()){
    output.seekp(sizeof(WavHeader),std::ios::beg);
  }else{
    throw std::runtime_error("Failed to create file "+this->m_filename);
  }

}

void Imufungen::addTone(float frequency, float duration){

    int length = this->m_SampleRate*duration;
    short* data = new short[length];

    for(int i = 0;i<length;i++){
      double time = (float)i/(float)this->m_SampleRate;
      data[i] = std::sin(time*2.0f*PI*frequency)*32767;
    }
    
    output.write((char*)data,length*sizeof(short));
    this->m_totalDataLength+=length*sizeof(short);
    delete[] data;
}

void Imufungen::addSweep(float startFrequency, float endFrequency, float duration){

    int length = this->m_SampleRate*duration;
    short* data = new short[length];
    double phase = 0.0f;
    double phaseIncrement;

    for(int i = 0;i<length;i++){
      double time = (double)i/this->m_SampleRate;
      double frequency = rangemap(time,0.0f,duration,startFrequency,endFrequency);
      phaseIncrement = (2.0*PI*frequency)/this->m_SampleRate;
      phase+= phaseIncrement;
      data[i] = std::sin(phase)*32767;
    }

    output.write((char*)data,length*sizeof(short));
    this->m_totalDataLength+= length*sizeof(short);
    delete[] data;
}

void Imufungen::addSilence(float duration){

  int length = this->m_SampleRate*duration;
  short* data = new short[length];
  for(int i = 0;i<length;i++){
    data[i] = 0;
  }

  output.write((char*)data,length*sizeof(short));
  this->m_totalDataLength+= length*sizeof(short);
  delete[] data;
}

void Imufungen::finish(){
  this->m_header.ChunkSize = 4+(8+16)+(8+(this->m_totalDataLength*this->m_channels*(this->m_bitDepth/8) ) );
  this->m_header.Subchunk1Size = 16;
  this->m_header.AudioFormat = 1;
  this->m_header.NumChannels = this->m_channels;
  this->m_header.SampleRate = this->m_SampleRate;
  this->m_header.ByteRate = this->m_SampleRate*this->m_channels*(this->m_bitDepth/8);
  this->m_header.Blockalign = this->m_channels*this->m_bitDepth;
  this->m_header.BitsPerSample = this->m_bitDepth;
  this->m_header.Subchunk2Size = this->m_totalDataLength*this->m_channels*(this->m_bitDepth/8);

  output.seekp(0,std::ios::beg);
  output.write((char*)&this->m_header,sizeof(WavHeader));
  output.close();
}
