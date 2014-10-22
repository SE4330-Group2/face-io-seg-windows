#include "io_seg.h"
#include <stdio.h>

#define MAX_CONNECTION_DATA 10

#define IO_SEG_TESTING

// Board configuration data
static FACE_CONFIG_DATA_TYPE  configData[MAX_CONNECTION_DATA];
static uint32_t numconectionData = 0;
CEI_INT16 board;         // board device number
CEI_INT16 num_xmtrs;     // number of transmitters purchased
CEI_INT16 num_rcvrs;     // number of receivers purchased

void IO_Seg_Initialize
   ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   CEI_INT16 status;        // API status value

   printf("  In IO Seg Read.\n");

   if(PasrseConfigFile(configuration_file + 1, configData, &numconectionData))
   {
      board = 0;

      // load the board (address, port, and size are not used - must specify zero)
      status = ar_loadslv(board,0,0,0);
      if (status != ARS_NORMAL)
      {
         printf("  Error while testing board %d:\n",board);
         printf("  Error reported:  \'%s\'\n",ar_get_error(status));
         printf("  Additional info: \'%s\'\n",ar_get_error(ARS_LAST_ERROR));
         *return_code = FACE_NOT_AVAILABLE;
         return;
      }

      // display the board type
      printf("  %s detected ",ar_get_boardname(board,NULL));

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
   uint8_t bitVal = 0;
   uint8_t channel;
   CEI_INT32 discrete_reg;  // used to read discretes

   FACE_IO_MESSAGE_TYPE * faceMsg = (FACE_IO_MESSAGE_TYPE *)data_buffer_address;
   printf("  In IO Seg Read.\n");

   if (faceMsg->busType == FACE_DISCRETE)
   {
      channel = FaceDiscreteChannelNumber(faceMsg);
      printf("  Reading from channel %d.\n", channel);

      // read the discrete input values
      discrete_reg = ar_get_config(board,ARU_DISCRETE_INPUTS);

      printf("  Current discrete input values (reg = 0x%4X):\n",discrete_reg);

      bitVal = (discrete_reg >> (channel - 1)) & 1;

      printf("  Going to return a %d\n", bitVal);
      FaceSetDiscreteState(faceMsg, bitVal);
      *return_code = FACE_NO_ERROR;
   }
   else
   {
      printf("  Can't handle the bustype sent: %d.\n", faceMsg->busType);
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

   printf("  In IO Seg Write.\n");
   if (faceMsg->busType == FACE_DISCRETE)
   {
      CEI_INT16 status;        // API status value
      CEI_INT32 discrete_reg;  // used to read/write discretes

      uint8_t channel = FaceDiscreteChannelNumber(faceMsg);
      uint8_t state = FaceDiscreteState(faceMsg);

      printf("  Need to write channel %d with a %d.\n", channel, state);

      discrete_reg = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

      printf("  Current discrete output values (reg = 0x%4X):\n",discrete_reg);

      if(state)
         discrete_reg |= 1 <<(channel - 1);
      else
         discrete_reg &= ~(1 <<(channel - 1));

      status = ar_set_config(board,ARU_DISCRETE_OUTPUTS,discrete_reg);

      // set the discrete output value
      if (status == ARS_NORMAL)
      {
         printf("  Discrete output reg has been set to 0x%4X.\n",discrete_reg & 0xFFFF);
         *return_code = FACE_NO_ERROR;
      }
      else
      {
         printf("  Error setting discrete output on board %d:\n",board);
         printf("  Error reported:  \'%s\'\n",ar_get_error(status));
         printf("  Additional info: \'%s\'\n",ar_get_error(ARS_LAST_ERROR));
         *return_code = FACE_NOT_AVAILABLE;
      }
   }
   else
   {
      printf("  Can't handle the bustype sent: %d.\n", faceMsg->busType);
      *return_code = FACE_INVALID_PARAM;
   }
}

#ifdef IO_SEG_TESTING

#define MAX_BUFF_SIZE  1024

static _Bool errorOccured = false;

FACE_INTERFACE_HANDLE_TYPE wowHandle;
FACE_INTERFACE_HANDLE_TYPE emergencyHandle;

int TEST_IO_SEG_INIT_INVALID_FILE()
{
   FACE_RETURN_CODE_TYPE retCode;
   printf("  ");
   IO_Seg_Initialize("1garbage", &retCode);
   return retCode == FACE_INVALID_CONFIG;
}

int TEST_IO_SEG_INIT_GOOD()
{
   FACE_RETURN_CODE_TYPE retCode;
   IO_Seg_Initialize("1config.xml", &retCode);
   return retCode == FACE_NO_ERROR;
}

int TEST_IO_SEG_READ_GOOD()
{
   FACE_RETURN_CODE_TYPE retCode;
   FACE_MESSAGE_LENGTH_TYPE message_length;

   char rxBuff[1024];

   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;

   rxFaceMsg->guid = htonl(200);
   rxFaceMsg->busType = FACE_DISCRETE;
   rxFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(rxFaceMsg, 4);

   message_length = FACE_MSG_HEADER_SIZE + 4;
   FaceSetDiscreteChannelNumber(rxFaceMsg, 1);
   IO_Seg_Read(&message_length, rxFaceMsg, &retCode);
   return retCode == FACE_NO_ERROR;
}

int TEST_IO_SEG_READ_BAD_BUSTYPE()
{
   FACE_RETURN_CODE_TYPE retCode;
   FACE_MESSAGE_LENGTH_TYPE message_length;

   char rxBuff[1024];

   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;

   rxFaceMsg->guid = htonl(200);
   rxFaceMsg->busType = FACE_ARINC_429;
   rxFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(rxFaceMsg, 4);

   message_length = FACE_MSG_HEADER_SIZE + 4;
   FaceSetDiscreteChannelNumber(rxFaceMsg, 1);
   IO_Seg_Read(&message_length, rxFaceMsg, &retCode);
   return retCode == FACE_INVALID_PARAM;
}

int TEST_IO_SEG_WRITE_GOOD_ON()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   CEI_INT32 discrete_reg_old, discrete_reg_new;
   uint8_t channel = 1;
   int state = 0;

   char txBuff[1024];

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;

   txFaceMsg->guid = htonl(100);
   txFaceMsg->busType = FACE_DISCRETE;
   txFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(txFaceMsg, 4);

   message_length = FACE_MSG_HEADER_SIZE + 4;

   discrete_reg_old = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

   FaceSetDiscreteChannelNumber(txFaceMsg,channel);
   FaceSetDiscreteState(txFaceMsg, state);
   IO_Seg_Write(message_length, txFaceMsg, &retCode);

   discrete_reg_new = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

   return (retCode == FACE_NO_ERROR) && (discrete_reg_new == (discrete_reg_old & ~(1<<(channel-1))));
}

int TEST_IO_SEG_WRITE_GOOD_OFF()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   CEI_INT32 discrete_reg_old, discrete_reg_new;
   uint8_t channel = 1;
   int state = 1;

   char txBuff[1024];

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;

   txFaceMsg->guid = htonl(100);
   txFaceMsg->busType = FACE_DISCRETE;
   txFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(txFaceMsg, 4);

   message_length = FACE_MSG_HEADER_SIZE + 4;

   discrete_reg_old = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

   FaceSetDiscreteChannelNumber(txFaceMsg,channel);
   FaceSetDiscreteState(txFaceMsg, state);
   IO_Seg_Write(message_length, txFaceMsg, &retCode);

   discrete_reg_new = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

   return (retCode == FACE_NO_ERROR) && (discrete_reg_new == (discrete_reg_old | (1<<(channel-1))));
}


int TEST_IO_SEG_WRITE_CHANNELS_PRESERVED()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   CEI_INT32 discrete_reg_old, discrete_reg_new;
   uint8_t channel = 4;
   char txBuff[1024];

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;

   txFaceMsg->guid = htonl(100);
   txFaceMsg->busType = FACE_DISCRETE;
   txFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(txFaceMsg, 4);

   message_length = FACE_MSG_HEADER_SIZE + 4;

   discrete_reg_old = ar_get_config(board,ARU_DISCRETE_OUTPUTS);
   FaceSetDiscreteChannelNumber(txFaceMsg, channel);
   FaceSetDiscreteState(txFaceMsg, 0);
   IO_Seg_Write(message_length, txFaceMsg, &retCode);

   discrete_reg_new = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

   return (discrete_reg_old & ~(1<<(channel-1))) == (discrete_reg_new & ~(1<<(channel-1)));
}


int TEST_FACE_IO_INITIALIZE()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_IO_Initialize("2config.xml", &retCode);

   return retCode == FACE_NO_ERROR;
}

int TEST_FACE_IO_OPEN()
{
   FACE_RETURN_CODE_TYPE retCode1, retCode2;

   FACE_IO_Open("DISCRETE_INPUT_WOW", &wowHandle, &retCode1);
   FACE_IO_Open("DISCRETE_OUTPUT_EMERGENCY", &emergencyHandle, &retCode2);

   return (retCode1 == FACE_NO_ERROR) && (retCode2 == FACE_NO_ERROR);
}

int TEST_FACE_IO_WRITE_ON()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   char txBuff[MAX_BUFF_SIZE];

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;
   txFaceMsg->guid = htonl(100);
   txFaceMsg->busType = FACE_DISCRETE;
   txFaceMsg->message_type = htons(FACE_DATA);

   FaceSetPayLoadLength(txFaceMsg, 4);

   FaceSetDiscreteChannelNumber(txFaceMsg, 1);

   FaceSetDiscreteState(txFaceMsg, 0);

   FACE_IO_Write(emergencyHandle, 0, FACE_MSG_HEADER_SIZE + 4, txFaceMsg, &retCode);

   return retCode == FACE_NO_ERROR;
}

int TEST_FACE_IO_WRITE_OFF()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   char txBuff[MAX_BUFF_SIZE];

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;
   txFaceMsg->guid = htonl(100);
   txFaceMsg->busType = FACE_DISCRETE;
   txFaceMsg->message_type = htons(FACE_DATA);

   FaceSetPayLoadLength(txFaceMsg, 4);

   FaceSetDiscreteChannelNumber(txFaceMsg, 1);

   FaceSetDiscreteState(txFaceMsg, 1);

   FACE_IO_Write(emergencyHandle, 0, FACE_MSG_HEADER_SIZE + 4, txFaceMsg, &retCode);

   return retCode == FACE_NO_ERROR;
}

int TEST_FACE_IO_READ()
{
   FACE_MESSAGE_LENGTH_TYPE messLen = FACE_MSG_HEADER_SIZE + 4;
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   char rxBuff[MAX_BUFF_SIZE];

   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;

   // Zero them out
   memset(rxBuff, 0, MAX_BUFF_SIZE);

   // Set the fixed fields - make sure to convert to network order when needed
   rxFaceMsg->guid = htonl(200);   // Pick a couple of numbers
   rxFaceMsg->busType = FACE_DISCRETE;
   rxFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(rxFaceMsg, 4);

   FaceSetDiscreteChannelNumber(rxFaceMsg, 1);

   FACE_IO_Read(wowHandle, 0, &messLen, rxFaceMsg, &retCode);

   return retCode == FACE_NO_ERROR;
}

int TEST_FACE_IO_CLOSE()
{
   FACE_RETURN_CODE_TYPE retCode1, retCode2;

   FACE_IO_Close(wowHandle, &retCode1);
   FACE_IO_Close(emergencyHandle, &retCode2);

   return (retCode1 == FACE_NO_ERROR) && (retCode2 == FACE_NO_ERROR);
}


int TEST_FACE_IO_READ_INTERACTIVE()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   char c;
   char rxBuff[1024];
   uint8_t state, oldstate;
   uint8_t channel = 1;


   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;

   // Set the fixed fields - make sure to convert to network order when needed
   rxFaceMsg->guid = htonl(200);   // Pick a couple of numbers
   rxFaceMsg->busType = FACE_DISCRETE;
   rxFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(rxFaceMsg, 4);

   IO_Seg_Initialize("1config.xml", &retCode);

   message_length = FACE_MSG_HEADER_SIZE + 4;
   FaceSetDiscreteChannelNumber(rxFaceMsg, channel);

   IO_Seg_Read(&message_length, rxFaceMsg, &retCode);
   oldstate = state = FaceDiscreteState(rxFaceMsg);

   printf("  Read state:%d on channel:%d.\n", state, channel);

   printf("  Toggle dipswitch 1 and press [ENTER] to continue.");
   fflush( stdin );
   c = getchar();

   IO_Seg_Read(&message_length, rxFaceMsg, &retCode);
   state = FaceDiscreteState(rxFaceMsg);

   return (oldstate != state) && (retCode == FACE_NO_ERROR);
}

int TEST_FACE_IO_WRITE_INTERACTIVE()
{
   char c;
   _Bool success;

   TEST_FACE_IO_OPEN();

   printf("  Turning on LED...\n");

   success = TEST_FACE_IO_WRITE_ON();

   printf("  Is the LED on? (y/n)\n");
   fflush( stdin );
   c = getchar();
   if(c == 'n' || !success)
      return 0;

   printf("  Turning off LED...\n");
   
   success = TEST_FACE_IO_WRITE_OFF();

   printf("  Is the LED off? (y/n)\n");
   fflush( stdin );
   c = getchar();
   if(c == 'n' || !success)
      return 0;

   TEST_FACE_IO_CLOSE();

   return success;
}

void handle_test(char* name, int (*func)())
{
   printf("Starting test `%s`\n", name);

   if(func())
      printf("[Test Passed]\n\n");
   else
   {
      errorOccured = true;
      printf("[Test Failed]\n\n");
   }
}

int main(int argc, char *argv[])
{
   printf("Starting Tests\n");
   printf("==============\n\n");

   printf("Unit Tests\n");
   printf("--------------\n\n");

   handle_test("Init - Handle bad config file", TEST_IO_SEG_INIT_INVALID_FILE);
   handle_test("Init - Config file valid", TEST_IO_SEG_INIT_GOOD);

   handle_test("Read - Unimplemented bus type", TEST_IO_SEG_READ_BAD_BUSTYPE);
   handle_test("Read - Successful", TEST_IO_SEG_READ_GOOD);

   handle_test("Write - Channels are preserved", TEST_IO_SEG_WRITE_CHANNELS_PRESERVED);

   handle_test("Write - On successful", TEST_IO_SEG_WRITE_GOOD_ON);
   handle_test("Write - Off successful", TEST_IO_SEG_WRITE_GOOD_OFF);

   printf("Unit Tests Complete\n\n\n");

   printf("Integration Tests\n");
   printf("-----------------\n\n");

   handle_test("FACE_IO Init - Success", TEST_FACE_IO_INITIALIZE);
   handle_test("FACE_IO Open - Success", TEST_FACE_IO_OPEN);
   handle_test("FACE_IO Write On - Success", TEST_FACE_IO_WRITE_ON);
   handle_test("FACE_IO Write Off - Success", TEST_FACE_IO_WRITE_OFF);
   handle_test("FACE_IO Read - Success", TEST_FACE_IO_READ);
   handle_test("FACE_IO Close - Success", TEST_FACE_IO_CLOSE);

   printf("Integration Tests Complete\n\n\n");

   printf("Interactive Tests\n");
   printf("-----------------\n\n");

   handle_test("FACE_IO Write Interactive", TEST_FACE_IO_READ_INTERACTIVE);

   handle_test("FACE_IO Write Interactive", TEST_FACE_IO_WRITE_INTERACTIVE);

   printf("Interactive Tests Complete\n\n\n");

   if(errorOccured)
      printf("One or more errors have occured\n");
   else
      printf("All tests completed successfully\n");

   fflush( stdin );
   getchar();
}

#endif