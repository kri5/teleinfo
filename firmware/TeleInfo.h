/*
TeleInfo - une bibliothèque de décodage des trames du compteur électrique pour Spark Core
Inspiré de la philo de la bibliothèque TinyGPS de Mikal Hart
D'après le document ERDF-NOI-CPT_02E - Version 2 - 01/03/2008
Copyright (C) 2014 Thibault Ducret
Licence MIT
*/
#ifndef TeleInfo_h
#define TeleInfo_h

#include <stdlib.h>
#include <string.h>

#define LABEL_MAX_SIZE 8 // max size for a label
#define DATA_MAX_SIZE 12 // max size for a data
#define NB_GROUP_MAX 30 // max number of groups
#define INVALID_STRING "NNNNNNNNNNNN"
#define INVALID_UINT 65535

typedef struct {
    char label[LABEL_MAX_SIZE + 1];
    char data[DATA_MAX_SIZE+1];
} TeleInfoGroup;

class TeleInfo
{

public:
  TeleInfo();
  bool decode(char c);
  TeleInfo &operator << (char c)
  {
	  decode(c); return *this;
  }

  inline char* meterAddress() { return meter_address; }
  inline char* rateOption() { return rate_option; }
  inline unsigned int subscribedIntensity() { return subscribed_intensity; }
  inline unsigned int offPeakHoursIndex() { return off_peak_hours_index; }
  inline unsigned int peakHoursIndex() { return peak_hours_index; }
  inline char *ratePeriod() { return rate_period; }
  inline unsigned int instantIntensity() { return instant_intensity; }
  inline char *overuseWarning() { return overuse_warning; }
  inline unsigned int maxIntensity() { return max_intensity; }
  inline unsigned int appearingPower() { return appearing_power; }
  inline char *timeGroup() { return time_group; }
  inline char *stateWord() { return state_word; }

private:
  bool waitForFrameBegining(char c);
  bool waitForGroupStart(char c);
  bool createLabel(char c);
  bool createData(char c);
  bool verifyChecksum(char c);
  bool waitForGroupOrFrameEnd(char c);
  bool handleError();

  bool (TeleInfo::*currentState)(char c);

  // propriétés
  bool is_frame_valid;
  char meter_address[DATA_MAX_SIZE];
  char _new_adCompteur[DATA_MAX_SIZE];
  char rate_option[DATA_MAX_SIZE];
  char _new_opTarif[DATA_MAX_SIZE];
  unsigned int subscribed_intensity;
  unsigned int _new_iSousc;
  unsigned int off_peak_hours_index;
  unsigned int _new_indexHC;
  unsigned int peak_hours_index;
  unsigned int _new_indexHP;
  char rate_period[DATA_MAX_SIZE];
  char _new_perTarif[DATA_MAX_SIZE];
  unsigned int instant_intensity;
  unsigned int _new_iInst;
  char  overuse_warning[DATA_MAX_SIZE];
  char _new_avertDep[DATA_MAX_SIZE];
  unsigned int max_intensity;
  unsigned int _new_iMax;
  unsigned int appearing_power;
  unsigned int _new_pApp;
  char time_group[DATA_MAX_SIZE];
  char _new_typeHoraireHPHC[DATA_MAX_SIZE];
  char state_word[DATA_MAX_SIZE];
  char _new_motEtat[DATA_MAX_SIZE];

  unsigned int _nbCarEtat2, _nbCarEtat3, _nbCarEtat4;
  char label[LABEL_MAX_SIZE];
  char data[DATA_MAX_SIZE];
  char _checksum;
  TeleInfoGroup groups[NB_GROUP_MAX];
  unsigned int group_numbers;

  void setVariables();
};

#endif
