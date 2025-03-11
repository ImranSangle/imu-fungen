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

#pragma once

#include <string>
#include <fstream>

#define PI 3.14159265359f

struct WavHeader{
 struct{
  char chunkID[4] = {'R','I','F','F'};
  int ChunkSize;
  char Format[4] = {'W','A','V','E'};
 };

 struct{
  char Subchunk1ID[4] = {'f','m','t',' '}; 
  int Subchunk1Size;
  short AudioFormat;
  short NumChannels;
  int SampleRate;
  int ByteRate;
  short Blockalign;
  short BitsPerSample;
 };

 struct{
  char Subchunk2ID[4] = {'d','a','t','a'};
  int Subchunk2Size;
 };
};

class Imufungen{
  public:
  enum DEPTH{
    DEPTH8 = 8,
    DEPTH16 = 16
  };
  enum CHANNELS{
    MONO = 1,
    STEREO = 2
  };
  enum class SELECT{
    LEFT,
    RIGHT,
    ALL
  };
  enum class BLEND{
    NORMAL,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE
  };
  private:
   WavHeader m_header;
   float m_SampleRate;
   DEPTH m_bitDepth;
   CHANNELS m_channels;
   BLEND m_blend = BLEND::NORMAL;
   float m_volume[2] = {1.0,1.0};
   size_t m_totalDataLength;
   size_t m_marker = sizeof(WavHeader);
   std::string m_filename;
   std::fstream file;

  template<typename T>
  void blend(T& data,const double& value);

  template<typename T>
  void loadData(T& data,int length);

  public:

  Imufungen(const std::string& filename,float SampleRate,DEPTH bitDepth,CHANNELS channels);

  void setVolume(SELECT channel,float percent);

  void setMarker();

  void setBlendMode(BLEND option);

  void gotoMarker();

  void addTone(float frequency,float duration);
  
  void addSweep(float startFrequency,float endFrequency,float duration);

  void addSilence(float duration);

  void finish();
};
