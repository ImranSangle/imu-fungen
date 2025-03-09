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
  private:
   WavHeader m_header;
   float m_SampleRate;
   DEPTH m_bitDepth;
   CHANNELS m_channels;
   int m_totalDataLength;
   std::string m_filename;
   std::ofstream output;
  public:

  Imufungen(const std::string& filename,float SampleRate,DEPTH bitDepth,CHANNELS channels);

  void addTone(float frequency,float duration);
  
  void addSweep(float startFrequency,float endFrequency,float duration);

  void addSilence(float duration);

  void finish();
};
