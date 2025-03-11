/*
 * Copyright (C) 2025 ImranSangle
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstddef>
#include <math.h>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <fstream>

#include "imufungen.h"
#include <mathfunctions.h>

template<typename T>
void Imufungen::blend(T& data,const double& value){

  double l_data = data;
  double l_value = value;

  if(this->m_bitDepth == DEPTH8){
    l_data = l_data-127;
    l_value = l_value-127;
  }

  switch(this->m_blend){
    case BLEND::NORMAL:
      l_data = l_value;
    break;
    case BLEND::ADD:
      l_data+= l_value;
    break;
    case BLEND::SUBTRACT:
      l_data-= l_value;
    break;
    case BLEND::MULTIPLY:
      l_data+= l_value;
    break;
    case BLEND::DIVIDE:
      l_data/= l_value;
    break;
  }

  if(this->m_bitDepth == DEPTH8){
    l_data = l_data+127;
  }

  data = l_data;

}

template<typename T>
void Imufungen::loadData(T& data,int length){

  if(this->m_blend != BLEND::NORMAL){
     size_t savePosition = this->file.tellp();
     this->file.seekg(this->file.tellp(),std::ios::beg);
     this->file.read((char*)data.get(),length);
     if(this->file.eof()){
       this->file.clear();
     }
     this->file.seekp(savePosition,std::ios::beg);
  }

}

Imufungen::Imufungen(const std::string& filename,float sampleRate,DEPTH bitDepth,CHANNELS channels){
  this->m_filename = filename;
  this->m_SampleRate = sampleRate;
  this->m_bitDepth = bitDepth;
  this->m_channels = channels;
  
  this->file.open(this->m_filename,std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

  if(this->file.is_open()){
    this->file.seekp(sizeof(WavHeader),std::ios::beg);
  }else{
    throw std::runtime_error("Failed to create file "+this->m_filename);
  }

}

void Imufungen::setVolume(SELECT channel,float percent){

  switch(channel){
    case SELECT::LEFT:
      this->m_volume[0] = percent;
    break;
    case SELECT::RIGHT:
      this->m_volume[1] = percent;
    break;
    case SELECT::ALL:
      this->m_volume[0] = percent;
      this->m_volume[1] = percent;
    break;
  }

}

void Imufungen::setBlendMode(BLEND option){

  this->m_blend = option;
}

void Imufungen::setMarker(){

  this->m_marker = this->file.tellp();

}

void Imufungen::gotoMarker(){

  size_t currentPos = this->file.tellp();
  this->m_totalDataLength-= (currentPos-this->m_marker)/this->m_channels/(this->m_bitDepth/8);
  this->file.seekp(this->m_marker,std::ios::beg);

}

void Imufungen::setParams(int& scale,int& add){

    switch(this->m_bitDepth){
      case DEPTH8:
        scale = 127;
        add = 127;
      break;
      case DEPTH16:
        scale = 32767;
        add = 0;
      break;
    }

}

template<typename T>
void Imufungen::toneProcessor(float frequency,float duration){

    int length = this->m_SampleRate*this->m_channels*duration;
    int data_scale;
    int data_add;

    this->setParams(data_scale,data_add);

    std::unique_ptr<T[]> data = std::make_unique<T[]>(length);

    this->loadData(data,length*sizeof(T));

    for(int i = 0;i<length;i+=this->m_channels){
      double time = (float)i/this->m_channels/this->m_SampleRate;
      for(int c =0;c<this->m_channels;c++){
        double value = std::sin(time*2.0f*PI*frequency)*(data_scale*this->m_volume[c])+data_add;
        this->blend(data[i+c],value);
      }
    }
    file.write((char*)data.get(),length*sizeof(T));

    this->m_totalDataLength+=this->m_SampleRate*duration;
}

void Imufungen::addTone(float frequency, float duration){

    switch(this->m_bitDepth){
      case DEPTH8:
        toneProcessor<char>(frequency,duration);
      break;
      case DEPTH16:
        toneProcessor<short>(frequency,duration);
      break;
    }

}

template<typename T>
void Imufungen::sweepProcessor(float startFrequency,float endFrequency,float duration){

    int length = this->m_SampleRate*this->m_channels*duration;
    int data_scale;
    int data_add;

   this->setParams(data_scale,data_add);

   std::unique_ptr<T[]> data = std::make_unique<T[]>(length);
   this->loadData(data,length*sizeof(T));
   double phase = 0.0f;
   double phaseIncrement;

   for(int i = 0;i<length;i+=this->m_channels){
     double time = (double)i/this->m_channels/this->m_SampleRate;
     double frequency = rangemap(time,0.0f,duration,startFrequency,endFrequency);
     phaseIncrement = (2.0*PI*frequency)/this->m_SampleRate;
     phase+= phaseIncrement;
     for(int c =0;c<this->m_channels;c++){
       double value = std::sin(phase)*(data_scale*this->m_volume[c])+data_add;
       this->blend(data[i+c],value);
     }
   }
   file.write((char*)data.get(),length*sizeof(T));

   this->m_totalDataLength+=this->m_SampleRate*duration;
}

void Imufungen::addSweep(float startFrequency, float endFrequency, float duration){

    switch(this->m_bitDepth){
      case DEPTH8:
        sweepProcessor<char>(startFrequency,endFrequency,duration);
      break;
      case DEPTH16:
        sweepProcessor<short>(startFrequency,endFrequency,duration);
      break;
    }
}

template<typename T>
void Imufungen::silenceProcessor(float duration){

  int length = this->m_SampleRate*this->m_channels*duration;
  
  int silent;

  switch(this->m_bitDepth){
    case DEPTH8:
      silent = 127;
    break;
    case DEPTH16:
      silent = 0;
    break;
  }

  std::unique_ptr<T[]> data = std::make_unique<T[]>(length);
  for(int i = 0;i<length;i++){
    data[i] = silent;
  }

  file.write((char*)data.get(),length*sizeof(T));

  this->m_totalDataLength+= this->m_SampleRate*duration;
}

void Imufungen::addSilence(float duration){

    switch(this->m_bitDepth){
      case DEPTH8:
        silenceProcessor<char>(duration);
      break;
      case DEPTH16:
        silenceProcessor<short>(duration);
      break;
    }
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

  file.seekp(0,std::ios::beg);
  file.write((char*)&this->m_header,sizeof(WavHeader));
  file.close();
}
