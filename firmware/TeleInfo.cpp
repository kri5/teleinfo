/*
TeleInfo - une bibliothèque de décodage des trames du compteur électrique pour Spark Core
Inspiré de la philo de la bibliothèque TinyGPS de Mikal Hart
D'après le document ERDF-NOI-CPT_02E - Version 2 - 01/03/2008
Copyright (C) 2014 Thibault Ducret
Licence MIT
*/

#include "TeleInfo.h"

//definition of labels
#define _LABEL_METER_ADDRESS            "ADCO"
#define _LABEL_RATE_OPTION              "OPTARIF"
#define _LABEL_SUBSCRIBED_INTENSITY     "ISOUSC"
#define _LABEL_OFF_PEAK_HOURS           "HCHC"
#define _LABEL_PEAK_HOURS               "HCHP"
#define _LABEL_ONGOING_RATE_PERIOD      "PTEC"
#define _LABEL_INSTANT_INTENSITY        "IINST"
#define _LABEL_WARNING_OVERUSE          "ADPS"
#define _LABEL_MAX_INTENSITY            "IMAX"
#define _LABEL_APPEARING_POWER          "PAPP"
#define _LABEL_TIMEGROUP                "HHPHC"
#define _LABEL_STATE_WORD               "MOTDETAT"


TeleInfo::TeleInfo()
  :  subscribed_intensity(INVALID_UINT)
  ,  off_peak_hours_index(INVALID_UINT)
  ,  peak_hours_index(INVALID_UINT)
  ,  instant_intensity(INVALID_UINT)
  ,  max_intensity(INVALID_UINT)
  ,  appearing_power(INVALID_UINT)
{
  meter_address[0] = 0;
  rate_option[0] = 0;
  rate_period[0] = 0;
  overuse_warning[0] = 0;
  time_group[0] = 0;
  state_word[0] = 0;
  _nbCarEtat2 = 0;
  _nbCarEtat3 = 0;
  _nbCarEtat4 = 0;
}

bool TeleInfo::decode(char c)
{
  bool is_frame_valid = false;

  switch(this->state)
  {
  case WAIT_FOR_FRAME_BEGINING:
    if (c == 0x02)
		this->state = WAIT_FOR_GROUP_START;
    // we stay in this state while we have not received the begin character
    break;

  case WAIT_FOR_GROUP_START:
    if (c == 0x0A) {
      this->group_numbers = 0;
      memset(this->groups, 0, sizeof(this->groups)); // Reset the array
      this->state = CREATE_LABEL;
    }
    else
		state = HANDLE_ERROR;
    break;

  case CREATE_LABEL:
    if (_nbCarEtat2 == 0)
    {
      memset(label, 0, sizeof(label)); // Reset the label
    }

    if (c == 0x20) // end of the label
    {
      _nbCarEtat2 = 0;
      this->state = CREATE_DATA;
    }
    else if (_nbCarEtat2 < LABEL_MAX_SIZE)
    {
      label[_nbCarEtat2] = c;
      _nbCarEtat2 += 1;
    }
    else
    {
      _nbCarEtat2 = 0;
      state = HANDLE_ERROR;
    }
    break;

  case CREATE_DATA:
    if (_nbCarEtat3 == 0)
    {
      memset(this->data,0,sizeof(this->data)); // reset data
    }

    if (c == 0x20) // end of data
    {
      _nbCarEtat3 = 0;
      this->state = VERIFY_CHECKSUM;
    }
    else if (_nbCarEtat3 < DATA_MAX_SIZE)
    {
      data[_nbCarEtat3] = c;
      _nbCarEtat3 += 1;
    }
    else
    {
      _nbCarEtat3 = 0;
      state = HANDLE_ERROR;
    }
    break;

  case VERIFY_CHECKSUM:
    if (_nbCarEtat4 == 0)
    {
      _checksum = c;
      _nbCarEtat4 += 1;
    }
    else if (c == 0x0D) // end of checksum
    {
      _nbCarEtat4 = 0;
      // TODO : verify checksum
      if (group_numbers >= NB_GROUP_MAX)
      {
        state=HANDLE_ERROR;
      }
      else
      {
        memcpy(groups[group_numbers].label,label,sizeof(label));
        memcpy(groups[group_numbers].data,data,sizeof(data));
        group_numbers += 1;
        state = WAIT_FOR_GROUP_OR_FRAME_END;
      }
    }
    else
    {
      _nbCarEtat4 = 0;
      state = HANDLE_ERROR;
    }
    break;

  case WAIT_FOR_GROUP_OR_FRAME_END:
    if (c == 0x0A) // start of a new group
	{
		state = CREATE_LABEL;
	}
    else if (c == 0x03) // end of frame
    { // valid frame
      is_frame_valid = true;
      setVariables();
      this->state = WAIT_FOR_FRAME_BEGINING;
    }
    else
	{
		this->state = HANDLE_ERROR;
	}
    break;

  case HANDLE_ERROR:
    // error
    state = WAIT_FOR_FRAME_BEGINING;
    break;
  }

  return is_frame_valid;
}

void TeleInfo::setVariables()
{
  unsigned int i;

  for (i=0; i < group_numbers; i+=1)
  {
	if (strcmp(groups[i].label,_LABEL_METER_ADDRESS) == 0)
	{
  	    memcpy(meter_address,groups[i].data,sizeof(meter_address));
  	}
  	else if (strcmp(groups[i].label,_LABEL_APPEARING_POWER) == 0)
  	{
  		appearing_power=(unsigned int) atoi(groups[i].data);
  	}
  	else if (strcmp(groups[i].label,_LABEL_INSTANT_INTENSITY) == 0)
  	{
  		instant_intensity=(unsigned int) atoi(groups[i].data);
  	}
  	else if (strcmp(groups[i].label,_LABEL_OFF_PEAK_HOURS) == 0)
  	{
  		off_peak_hours_index=(unsigned int) atoi(groups[i].data);
  	}
  	else if (strcmp(groups[i].label,_LABEL_PEAK_HOURS) == 0)
  	{
  		peak_hours_index=(unsigned int) atoi(groups[i].data);
  	}
  }
}
