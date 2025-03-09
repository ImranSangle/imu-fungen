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

#include <math.h>
#include <memory>
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
    int data_length;

    switch(this->m_bitDepth){
      case DEPTH8:
      {
        data_length = sizeof(char);
        std::unique_ptr<char[]> data = std::make_unique<char[]>(length);

        for(int i = 0;i<length;i++){
          double time = (float)i/this->m_SampleRate;
          data[i] = std::sin(time*2.0f*PI*frequency)*127+127;
        }
        output.write((char*)data.get(),length*sizeof(char));
      }
      break;
      case DEPTH16:
       {
        data_length = sizeof(short);
        std::unique_ptr<short[]> data = std::make_unique<short[]>(length);

        for(int i = 0;i<length;i++){
          double time = (float)i/this->m_SampleRate;
          data[i] = std::sin(time*2.0f*PI*frequency)*32767;
        }
        output.write((char*)data.get(),length*sizeof(short));
       }
      break;
    }

    this->m_totalDataLength+=length*data_length;
}

void Imufungen::addSweep(float startFrequency, float endFrequency, float duration){

    int length = this->m_SampleRate*duration;
    int data_length;

    switch(this->m_bitDepth){
      case DEPTH8:
       {
        std::unique_ptr<char[]> data = std::make_unique<char[]>(length);
        data_length = sizeof(char);
        double phase = 0.0f;
        double phaseIncrement;

        for(int i = 0;i<length;i++){
          double time = (double)i/this->m_SampleRate;
          double frequency = rangemap(time,0.0f,duration,startFrequency,endFrequency);
          phaseIncrement = (2.0*PI*frequency)/this->m_SampleRate;
          phase+= phaseIncrement;
          data[i] = std::sin(phase)*127+127;
        }
        output.write((char*)data.get(),length*sizeof(char));
       }
      break;
      case DEPTH16:
       {
        std::unique_ptr<short[]> data = std::make_unique<short[]>(length);
        data_length = sizeof(short);
        double phase = 0.0f;
        double phaseIncrement;

        for(int i = 0;i<length;i++){
          double time = (double)i/this->m_SampleRate;
          double frequency = rangemap(time,0.0f,duration,startFrequency,endFrequency);
          phaseIncrement = (2.0*PI*frequency)/this->m_SampleRate;
          phase+= phaseIncrement;
          data[i] = std::sin(phase)*32767;
        }
        output.write((char*)data.get(),length*sizeof(short));
       }
      break;
    }

    this->m_totalDataLength+= length*data_length;
}

void Imufungen::addSilence(float duration){

  int length = this->m_SampleRate*duration;
  int data_length;

  switch(this->m_bitDepth){
    case DEPTH8:
     {
      data_length = sizeof(char);
      std::unique_ptr<char[]> data = std::make_unique<char[]>(length);
      for(int i = 0;i<length;i++){
        data[i] = 127;
      }
      output.write((char*)data.get(),length*sizeof(char));
     }
    break;
    case DEPTH16:
     {
      data_length = sizeof(short);
      std::unique_ptr<short[]> data = std::make_unique<short[]>(length);
      for(int i = 0;i<length;i++){
        data[i] = 0;
      }
      output.write((char*)data.get(),length*sizeof(short));
     }
    break;
  }

  this->m_totalDataLength+= length*data_length;
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
