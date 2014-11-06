//---------------------------------------------------------------------------
// Prototype 3
// This is the c file for io_seg.h which will be used by the I/O library
// when it needs to access hardware.
//---------------------------------------------------------------------------

#include "io_seg.h"
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include "queue.h"

#define MAX_CONNECTION_DATA 10

#define IO_SEG_TESTING

typedef struct
{
   FACE_CONFIG_DATA_TYPE  info;
   QUEUE                     q;
} A429_CONNECTION;

// Board configuration data
static FACE_CONFIG_DATA_TYPE  configData[MAX_CONNECTION_DATA];
static uint32_t numconectionData = 0;
static CEI_INT16 board;         // board device number
uint32_t numQueue;
A429_CONNECTION connectionQueues[MAX_CONNECTION_DATA];

IO_Seg_Initialize_PtrType IO_Seg_Initialize_Ptr = IO_Seg_Initialize;
IO_Seg_Read_PtrType IO_Seg_Read_Ptr = IO_Seg_Read;
IO_Seg_Write_PtrType IO_Seg_Write_Ptr = IO_Seg_Write;

void A429_Handler( void * ignored )
{
   uint32_t i;
   while(1)
   {
      for(i = 0; i < numQueue; i++)
      {
         //connectionQueues[i].info = NULL;
      }
      // printf("FOOOOOOOOOOOOOOOOOOOOO");
   }
}

void HandlePhysicalConnection()
{

}

void HandleLogicalConection()
{

}

FACE_RETURN_CODE_TYPE Discrete_Read(FACE_IO_MESSAGE_TYPE * faceMsg)
{
   uint8_t bitVal = 0;
   CEI_INT32 discrete_reg;

   uint8_t channel = FaceDiscreteChannelNumber(faceMsg);
   printf("  Reading from channel %d.\n", channel);

   // read the discrete input values
   discrete_reg = ar_get_config(board,ARU_DISCRETE_INPUTS);

   printf("  Current discrete input values (reg = 0x%4X):\n",discrete_reg);

   bitVal = (discrete_reg >> (channel - 1)) & 1;

   printf("  Going to return a %d\n", bitVal);
   FaceSetDiscreteState(faceMsg, bitVal);
   return FACE_NO_ERROR;
}

FACE_RETURN_CODE_TYPE A429_Read(FACE_IO_MESSAGE_TYPE * faceMsg)
{


   return FACE_NO_ERROR;
}

FACE_RETURN_CODE_TYPE Discrete_Write(FACE_IO_MESSAGE_TYPE * faceMsg)
{
   CEI_INT32 discrete_reg;  // used to read/write discretes
   CEI_INT16 status;        // API status value
   uint8_t channel = FaceDiscreteChannelNumber(faceMsg);
   uint8_t state = FaceDiscreteState(faceMsg);

   printf("  Need to write channel %d with a %d.\n", channel, state);

   discrete_reg = ar_get_config(board,ARU_DISCRETE_OUTPUTS);

   printf("  Current discrete output values (reg = 0x%4X):\n",discrete_reg);

   discrete_reg = state ? discrete_reg |   1 << (channel - 1)
      : discrete_reg & ~(1 << (channel - 1));

   status = ar_set_config(board,ARU_DISCRETE_OUTPUTS,discrete_reg);

   // set the discrete output value
   if (status == ARS_NORMAL)
   {
      printf("  Discrete output reg has been set to 0x%4X.\n",discrete_reg & 0xFFFF);
      return FACE_NO_ERROR;
   }
   else
   {
      printf("  Error setting discrete output on board %d:\n",board);
      printf("  Error reported:  \'%s\'\n",ar_get_error(status));
      printf("  Additional info: \'%s\'\n",ar_get_error(ARS_LAST_ERROR));
      return FACE_NOT_AVAILABLE;
   }
}

FACE_RETURN_CODE_TYPE A429_Write(FACE_IO_MESSAGE_TYPE * faceMsg)
{
   // write a word out the transmitter

   //status = ar_putword(board,tx,xmit_word);

   //if (status != ARS_NORMAL)
   //   *return_code = FACE_NOT_AVAILABLE; //TODO: More descriptive error code
   return FACE_NO_ERROR;
}

//---------------------------------------------------------------------------
// IO_Seg_Initialize is used to initialize the hardware.
// This method reads an XML config file and sets hardware accordingly.
// The config file should be a file name preceded by a character which will
// be ignored.
//---------------------------------------------------------------------------
void IO_Seg_Initialize
   ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   CEI_INT16 status;        // API status value
   CEI_INT16 num_xmtrs;     // number of transmitters purchased
   CEI_INT16 num_rcvrs;     // number of receivers purchased
   uint32_t i;
   numQueue = 0;

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

      printf("  %s detected ",ar_get_boardname(board,NULL));

      // display the number of receivers and transmitters
      num_xmtrs = ar_num_xchans(board);
      num_rcvrs = ar_num_rchans(board);
      printf("supporting %d transmitters and %d receivers.\n",num_xmtrs,num_rcvrs);

      for( i = 0; i < numconectionData; i++ )
      {
         if(configData[i].busType == FACE_ARINC_429)
         {
            A429_CONNECTION connection;
            connection.info = configData[i];
            queue_init(&connection.q);
            connectionQueues[numQueue] = connection;
            numQueue++;
         }
      }

      _beginthread( A429_Handler, 0, NULL );
      //while(1)
      //{
      //   printf("Baaaaaaaaar");
      //}
      *return_code = FACE_NO_ERROR;
   }
   else
   {
      *return_code = FACE_INVALID_CONFIG;
   }
}


//---------------------------------------------------------------------------
// IO_Seg_Read is used to read the hardware.
// This method retrieves the channel number and bus type from the location
// specified by the data buffer address. 
// After reading this information, this method retrieves the value from the
// hardware and copies it to the memory location specified by the data 
// buffer address.
//---------------------------------------------------------------------------
void IO_Seg_Read
   ( /* inout */ FACE_MESSAGE_LENGTH_TYPE *message_length,
   /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   FACE_IO_MESSAGE_TYPE * faceMsg = (FACE_IO_MESSAGE_TYPE *)data_buffer_address;
   printf("  In IO Seg Read.\n");

   if (faceMsg->busType == FACE_DISCRETE)
   {
      *return_code = Discrete_Read(faceMsg);
   }
   else if (faceMsg->busType == FACE_ARINC_429)
   {
      *return_code = A429_Read(faceMsg);
   }
   else
   {
      printf("  Can't handle the bustype sent: %d.\n", faceMsg->busType);
      *return_code = FACE_INVALID_PARAM;
   }
}

//---------------------------------------------------------------------------
// IO_Seg_Write is used to write to the hardware.
// This method retrieves the channel number, bus type, and desired state
// from the location specified by the data buffer address. 
// After reading this information, this method writes the desired state 
// to the hardware.
//---------------------------------------------------------------------------
void IO_Seg_Write
   (  /* in */ FACE_MESSAGE_LENGTH_TYPE message_length,
   /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   FACE_IO_MESSAGE_TYPE * faceMsg = (FACE_IO_MESSAGE_TYPE *)data_buffer_address;
   printf("  In IO Seg Write.\n");

   if (faceMsg->busType == FACE_DISCRETE)
   {
      *return_code = Discrete_Write(faceMsg);
   }
   else if (faceMsg->busType == FACE_ARINC_429)
   {
      *return_code = A429_Write(faceMsg);
   }
   else
   {
      printf("  Can't handle the bustype sent: %d.\n", faceMsg->busType);
      *return_code = FACE_INVALID_PARAM;
   }
}


//---------------------------------------------------------------------------
// This starts the testbed for the IO segment.
//---------------------------------------------------------------------------
#ifdef IO_SEG_TESTING

#define MAX_BUFF_SIZE  1024

static _Bool errorOccured = false;

FACE_INTERFACE_HANDLE_TYPE wowHandle;
FACE_INTERFACE_HANDLE_TYPE emergencyHandle;

// Returns true when file doesn't exist
_Bool TEST_IO_SEG_INIT_INVALID_FILE()
{
   FACE_RETURN_CODE_TYPE retCode;
   printf("  ");
   IO_Seg_Initialize("1garbage", &retCode);
   return retCode == FACE_INVALID_CONFIG;
}

// Returns true when init is successful
_Bool TEST_IO_SEG_INIT_GOOD()
{
   FACE_RETURN_CODE_TYPE retCode;
   IO_Seg_Initialize("1config.xml", &retCode);
   return retCode == FACE_NO_ERROR;
}

// Returns true when read is successful
_Bool TEST_IO_SEG_READ_GOOD()
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

// Returns true when a non-implemented bus type is detected
_Bool TEST_IO_SEG_READ_BAD_BUSTYPE()
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

// Returns true when writing a "0" is successful
_Bool TEST_IO_SEG_WRITE_GOOD_ON()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   CEI_INT32 discrete_reg_old, discrete_reg_new;
   uint8_t channel = 1;
   uint8_t state = 0;

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

// Returns true when writing a "1" is successful
_Bool TEST_IO_SEG_WRITE_GOOD_OFF()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   CEI_INT32 discrete_reg_old, discrete_reg_new;
   uint8_t channel = 1;
   uint8_t state = 1;

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

// Returns true when the channels not written to are preserved after writing to a channel
_Bool TEST_IO_SEG_WRITE_CHANNELS_PRESERVED()
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

// Returns true when FACE_IO_Initialize is successful
_Bool TEST_FACE_IO_INITIALIZE()
{
   FACE_RETURN_CODE_TYPE retCode = FACE_NO_ERROR;
   FACE_IO_Initialize("2config.xml", &retCode);

   return retCode == FACE_NO_ERROR;
}

// Returns true when FACE_IO_Open is successful
_Bool TEST_FACE_IO_OPEN()
{
   FACE_RETURN_CODE_TYPE retCode1, retCode2;

   FACE_IO_Open("DISCRETE_INPUT_WOW", &wowHandle, &retCode1);
   FACE_IO_Open("DISCRETE_OUTPUT_EMERGENCY", &emergencyHandle, &retCode2);

   return (retCode1 == FACE_NO_ERROR) && (retCode2 == FACE_NO_ERROR);
}

// Returns true when FACE_IO_Write state "0" is successful
_Bool TEST_FACE_IO_WRITE_ON()
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

// Returns true when FACE_IO_Write state "1" is successful
_Bool TEST_FACE_IO_WRITE_OFF()
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

// Returns true when FACE_IO_Read is successful
_Bool TEST_FACE_IO_READ()
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

// Returns true when FACE_IO_Close is successful
_Bool TEST_FACE_IO_CLOSE()
{
   FACE_RETURN_CODE_TYPE retCode1, retCode2;

   FACE_IO_Close(wowHandle, &retCode1);
   FACE_IO_Close(emergencyHandle, &retCode2);

   return (retCode1 == FACE_NO_ERROR) && (retCode2 == FACE_NO_ERROR);
}

// This is an interactive test that will prompt the user to toggle the 
// DIP switch before continuing.  The test is successful if the state
// change is detected.
_Bool TEST_FACE_IO_READ_INTERACTIVE()
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

// This is an interactive test that will turn on the LED and ask the
// user if they detect the state change.  The test is successful if 
// the state change is detected.
_Bool TEST_FACE_IO_WRITE_INTERACTIVE()
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

// Displays the name of the test and a message dependent on the
// result of the test
void handle_test(char* name, _Bool (*func)())
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

// Runs IO segment tests
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