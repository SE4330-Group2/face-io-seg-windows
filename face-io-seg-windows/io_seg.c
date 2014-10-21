#include "config_parser.h"
#include "face_common.h"
#include "face_ios.h"
#include "face_messages.h"
#include "globals.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utildefs.h"
#define MAX_CONNECTION_DATA 10

static FACE_CONFIG_DATA_TYPE  configData[MAX_CONNECTION_DATA];

// Number of Configured conectionData
static uint32_t numconectionData = 0;
CEI_INT16 board;         // board device number
CEI_INT16 num_xmtrs;     // number of transmitters purchased
CEI_INT16 num_rcvrs;     // number of receivers purchased

static void error_exit(CEI_INT16 board,CEI_INT16 status)
{
   // display the error message
   printf("Error while testing board %d:\n",board);
   printf("  Error reported:  \'%s\'\n",ar_get_error(status));
   printf("  Additional info: \'%s\'\n",ar_get_error(ARS_LAST_ERROR));

   // prompt the user to hit return
   printf("\nPress <Enter> to exit...\n");
   getchar();

   // terminate the application
   exit(0);
}

void IO_Seg_Initialize
   ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   _Bool success = PasrseConfigFile(configuration_file + 1, configData, &numconectionData);
   CEI_INT16 status;        // API status value


   if(success)
   {
      board = 0;

      // load the board (address, port, and size are not used - must specify zero)
      status = ar_loadslv(board,0,0,0);
      if (status != ARS_NORMAL)
         error_exit(board,status);

      // display the board type
      printf("%s detected ",ar_get_boardname(board,NULL));

      // display the number of receivers and transmitters
      num_xmtrs = ar_num_xchans(board);
      num_rcvrs = ar_num_rchans(board);
      printf("supporting %d transmitters and %d receivers.\n",num_xmtrs,num_rcvrs);


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
   uint8_t channel;
   CEI_INT32 discrete_reg;  // used to read discretes

   FACE_IO_MESSAGE_TYPE * faceMsg = (FACE_IO_MESSAGE_TYPE *)data_buffer_address;
   printf("In IO Seg Read.  ");

   if (faceMsg->busType == FACE_DISCRETE)
   {
      channel = FaceDiscreteChannelNumber(faceMsg);
      printf("Need to read channel %d.  ", channel);

      // read the discrete input values
      //    discrete_reg = ar_get_config(board,ARU_DISCRETE_VALUES);
      discrete_reg = ar_get_config(board,ARU_DISCRETE_INPUTS);
      //    discrete_reg = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

      // display the discrete input values
      printf("\nCurrent discrete input values (reg = 0x%4X):\n\n   ",discrete_reg);


      printf("  Going to return a %d\n", bitVal);
      FaceSetDiscreteState(faceMsg, bitVal);
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
      CEI_INT16 status;        // API status value
      CEI_INT32 discrete_reg;  // used to read/write discretes

      uint8_t channel = FaceDiscreteChannelNumber(faceMsg);
      uint8_t state = FaceDiscreteState(faceMsg);

      printf("Need to write channel %d with a %d.\n", channel, state);

      // discrete i is grounded if bit i is 0 - discrete i floats if bit i is 1

      // TODO: Replace '1' with !state. I think....
      discrete_reg = ~(1 << (channel - 1));

      // set the discrete output value
      //    status = ar_set_config(board,ARU_DISCRETE_VALUES,discrete_reg);
      status = ar_set_config(board,ARU_DISCRETE_OUTPUTS,discrete_reg);
      if (status != ARS_NORMAL)
         error_exit(board,status);

      // update the display
      printf("\nDiscrete output reg has been set to 0x%4X.\n",discrete_reg & 0xFFFF);

      *return_code = FACE_NO_ERROR;
   }
   else
   {
      printf("Can't handle the bustype sent: %d.\n", faceMsg->busType);
      *return_code = FACE_INVALID_PARAM;
   }
}