#include "config_parser.h"
#include "face_common.h"
#include "face_ios.h"
#include "face_messages.h"
#include "globals.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CONNECTION_DATA 10

static FACE_CONFIG_DATA_TYPE  configData[MAX_CONNECTION_DATA];

// Number of Configured conectionData
static uint32_t numconectionData = 0;

void IO_Seg_Initialize
     ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
       /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   _Bool success = PasrseConfigFile(configuration_file + 1, configData, &numconectionData);

   
   if(success)
   {

      *return_code = FACE_NO_ERROR;
   }
   else
   {
   
      *return_code = FACE_INVALID_CONFIG;
   }
}

// For Direct Read when PSS and I/O Seg are in the same program
void IO_Seg_Read
   ( /* inout */ FACE_MESSAGE_LENGTH_TYPE *message_length,
     /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
     /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   static uint8_t bitVal = 0;
   
   FACE_IO_MESSAGE_TYPE * faceMsg = (FACE_IO_MESSAGE_TYPE *)data_buffer_address;
   printf("In IO Seg Read.  ");

   if (faceMsg->busType == FACE_DISCRETE)
   {
      printf("Need to read channel %d.  ", FaceDiscreteChannelNumber(faceMsg));
      printf("  Going to return a %d\n", bitVal);
      FaceSetDiscreteState(faceMsg, bitVal);
      bitVal = 1 - bitVal;                     // Change it for next time
      *return_code = FACE_NO_ERROR;
   }
   else
   {
      printf("Can't handle the bustype sent: %d.\n", faceMsg->busType);
      *return_code = FACE_INVALID_PARAM;
   }
}

// For Direct Write when PSS and I/O Seg are in the same program
void IO_Seg_Write
   (  /* in */ FACE_MESSAGE_LENGTH_TYPE message_length,
      /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
      /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   FACE_IO_MESSAGE_TYPE * faceMsg = (FACE_IO_MESSAGE_TYPE *)data_buffer_address;

   printf("In IO Seg Write.  ");
   if (faceMsg->busType == FACE_DISCRETE)
   {
      printf("Need to write channel %d with a %d.\n", FaceDiscreteChannelNumber(faceMsg), FaceDiscreteState(faceMsg));
      *return_code = FACE_NO_ERROR;
   }
   else
   {
      printf("Can't handle the bustype sent: %d.\n", faceMsg->busType);
      *return_code = FACE_INVALID_PARAM;
   }
}