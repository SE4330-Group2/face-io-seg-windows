//---------------------------------------------------------------------------
// Prototype 3
// This is the c file for io_seg.h which will be used by the I/O library
// when it needs to access hardware.
//---------------------------------------------------------------------------

#include "io_seg.h"
#include "windows_wrappers.h"

//ARINC Board
#include "utildefs.h"

//Processes and Queues
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include "queue.h"

#define MAX_BUFF_SIZE  1024
#define MAX_CONNECTION_DATA 10

#define IO_SEG_TESTING
#define FACE_READ_TIMEOUT 5000000000LL

#define CONVERT_TO_MICRO 1000

#define IO_SEG_GUID 42

// Contains the information for a429 connections
typedef struct
{
   FACE_CONFIG_DATA_TYPE*      info;
   QUEUE                       q;
   FACE_INTERFACE_HANDLE_TYPE  handle;
   struct timeval              lastUDPWrite;
} A429_CONNECTION;

// Internal methods
void a429_handler( void * ignored );
static void handle_logical_connection(A429_CONNECTION * connection);
static void handle_physical_connection(A429_CONNECTION * connection);
static FACE_RETURN_CODE_TYPE discrete_read(FACE_IO_MESSAGE_TYPE * faceMsg);
static A429_CONNECTION * get_connection_by_channel(uint8_t channel, FACE_DIRECTIONALTY_TYPE direction);
static FACE_RETURN_CODE_TYPE a429_read(FACE_IO_MESSAGE_TYPE * faceMsg);
static void package_queue_contents(FACE_A429_MESSAGE_TYPE * A429Data, QUEUE * queue);
static void unpack_words_into_queue(FACE_A429_MESSAGE_TYPE * A429Data, QUEUE * queue);
static FACE_RETURN_CODE_TYPE discrete_write(FACE_IO_MESSAGE_TYPE * faceMsg);
static FACE_RETURN_CODE_TYPE a429_write(FACE_IO_MESSAGE_TYPE * faceMsg);

// Board configuration data
static FACE_CONFIG_DATA_TYPE  configData[MAX_CONNECTION_DATA];
static uint32_t numconectionData = 0;
static CEI_INT16 board;         // board device number
uint32_t numQueue;
A429_CONNECTION connectionQueues[MAX_CONNECTION_DATA];

IO_Seg_Initialize_PtrType IO_Seg_Initialize_Ptr = IO_Seg_Initialize;
IO_Seg_Read_PtrType IO_Seg_Read_Ptr = IO_Seg_Read;
IO_Seg_Write_PtrType IO_Seg_Write_Ptr = IO_Seg_Write;

//---------------------------------------------------------------------------
// error_exit is used to return and write errors.
// This method is called if an error occured and prints out the error,
// the associated id and allows user to exit program.
//---------------------------------------------------------------------------
void error_exit(CEI_INT16 board,CEI_INT16 status)
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

//---------------------------------------------------------------------------
// a429_handler is used to run the physical and logical connections.
// This method runs constantly and handles all of the a429 connections both
// physical and logical.
//---------------------------------------------------------------------------
static void a429_handler( void * ignored )
{
   uint32_t i;
   while(1)
   {
      for(i = 0; i < numQueue; i++)
      {
         if(connectionQueues[i].info->connectionType == FACE_DIRECT_CONNECTION)
         {
            handle_physical_connection(&connectionQueues[i]);
         }
         else if(connectionQueues[i].info->connectionType == FACE_UDP_CONNECTION)
         {
            handle_logical_connection(&connectionQueues[i]);
         }
      }
   }
}

//---------------------------------------------------------------------------
// handle_logical_connection is used to handle logical a429 connections.
// This method puts and gets a429 words for the thread and updates queues.
//---------------------------------------------------------------------------
static void handle_logical_connection(A429_CONNECTION * connection)
{
   FACE_RETURN_CODE_TYPE retCode;
   FACE_MESSAGE_LENGTH_TYPE message_length;
   if(connection->info->direction == FACE_TRANSMIT)
   {
      char rxBuff[MAX_BUFF_SIZE];

      FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *) rxBuff;
      FACE_A429_MESSAGE_TYPE * rxA429Data = (FACE_A429_MESSAGE_TYPE *) rxFaceMsg->data;

      // Zero them out
      memset(rxBuff, 0, MAX_BUFF_SIZE);

      rxFaceMsg->guid = htonl(IO_SEG_GUID);
      rxFaceMsg->busType = connection->info->busType;
      rxFaceMsg->message_type = htons(FACE_DATA);

      message_length = FACE_MSG_HEADER_SIZE + sizeof(FACE_A429_MESSAGE_TYPE);
      FACE_IO_Read(connection->handle, FACE_READ_TIMEOUT, &message_length, rxFaceMsg, &retCode);
      if (retCode == FACE_NO_ERROR)
      {
         unpack_words_into_queue(rxA429Data, &connection->q);
      }
      //else
      //  printf ("I/O API Test bed - IO Seg just read DataLog with error: %d.\n", retCode);
   }
   else if(connection->info->direction == FACE_RECEIVE)
   {
      struct timeval currentTime, result;
      int x;
      uint64_t res;
      x = 0;
      clock_gettime(x, &currentTime);
      timeval_subtract(&result, &currentTime, &connection->lastUDPWrite);
      res = (result.tv_sec * CONVERT_TO_MICRO * CONVERT_TO_MICRO) + result.tv_usec;
      if(res > connection->info->refreshPeriod / CONVERT_TO_MICRO)
      {
         //Get all Arinc Words from appropriate Queue

         char txBuff[MAX_BUFF_SIZE];

         LONG64 uptime;

         FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;
         FACE_A429_MESSAGE_TYPE * txA429Data = (FACE_A429_MESSAGE_TYPE *)txFaceMsg->data;

         connection->lastUDPWrite = currentTime;

         // Zero them out
         memset(txBuff, 0, MAX_BUFF_SIZE);

         // Set the fixed fields - make sure to convert to network order when needed
         txFaceMsg->guid = htonl(IO_SEG_GUID);
         txFaceMsg->busType =  connection->info->busType;
         txFaceMsg->message_type = htons(FACE_DATA);

         uptime = ar_get_timercnt(board) * 5 * CONVERT_TO_MICRO * CONVERT_TO_MICRO;
         txFaceMsg->timestamp_LSW = (uint32_t)uptime;
         txFaceMsg->timestamp_MSW = uptime >> 32;

         // Send a IGS Message
         txA429Data->channel = (uint8_t)(connection->info->channel);

         package_queue_contents(txA429Data, &connection->q);

         FaceSetPayLoadLength(txFaceMsg, sizeof(FACE_A429_MESSAGE_TYPE) + (2 * txA429Data->num_labels));

         FACE_IO_Write(connection->handle, 0, FACE_MSG_HEADER_SIZE + FacePayLoadLength(txFaceMsg), txFaceMsg, &retCode);
      }

   }
   else
      printf ("Doing nothing for FACE_DIRECTIONALTY_TYPE: %d\n", connection->info->direction);
}

//---------------------------------------------------------------------------
// handle_physical_connection is used to handle physical a429 connections.
// This method puts and gets a429 words for the thread and updates queues.
//---------------------------------------------------------------------------
static void handle_physical_connection(A429_CONNECTION * connection)
{
   if(connection->info->direction == FACE_TRANSMIT)
   {
      CEI_INT16 ret_code = 0;
      if(!queue_empty(&connection->q))
      {
         uint32_t xmit_word = queue_remove(&connection->q);
         ret_code = ar_putword(board, connection->info->channel, xmit_word);
         if(ret_code != ARS_NORMAL) // ARS_XMITOVRFLO
         {
            printf ("Failed to do ARINC putword. Returned: %d\n", ret_code);
         }
      }
   }
   else if(connection->info->direction == FACE_RECEIVE)
   {
      CEI_INT32 rcv_word = 0;
      CEI_INT16 ret_code = ar_getword(board, connection->info->channel, &rcv_word);
      if(ret_code != ARS_NODATA)
      {
         queue_add(&connection->q, rcv_word);
      }
      else
      {
         //printf("Did not recieve data\n");
      }
   }
}

//---------------------------------------------------------------------------
// discrete_read is used to read a state from the hardware.
// This method reads the discrete channel and puts the state into the
// face message.
//---------------------------------------------------------------------------
static FACE_RETURN_CODE_TYPE discrete_read(FACE_IO_MESSAGE_TYPE * faceMsg)
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

//---------------------------------------------------------------------------
// get_connection_by_channel is used to get a specific connection.
// This method uses the channel and its direction in order to find the
// proper a429 connection.
//---------------------------------------------------------------------------
static A429_CONNECTION * get_connection_by_channel(uint8_t channel, FACE_DIRECTIONALTY_TYPE direction)
{
   int i;

   for(i = 0; i < MAX_CONNECTION_DATA; i++)
   {
      if(connectionQueues[i].info->channel == channel && connectionQueues[i].info->direction == direction)
      {
         return (connectionQueues + i); //return reference to element in array.
      }
   }
   return NULL;
}

//---------------------------------------------------------------------------
// a429_read is used to read information from A429 hardware channel.
// This method takes the information in the connection queue and packages
// it into the face message.
//---------------------------------------------------------------------------
static FACE_RETURN_CODE_TYPE a429_read(FACE_IO_MESSAGE_TYPE * faceMsg)
{
   FACE_A429_MESSAGE_TYPE * txA429Data = (FACE_A429_MESSAGE_TYPE *)faceMsg->data;
   A429_CONNECTION * connection = get_connection_by_channel(txA429Data->channel, FACE_RECEIVE);

   if(connection!=NULL)
   {
      package_queue_contents(txA429Data, &connection->q);

      FaceSetPayLoadLength(faceMsg, sizeof(FACE_A429_MESSAGE_TYPE) + (2 * txA429Data->num_labels));

      return FACE_NO_ERROR;
   }
   return FACE_INVALID_CONFIG;
}

//---------------------------------------------------------------------------
// package_queue_contents is used to retrieve a429 words from queues.
// This method takes all of the information in the connection's queue and
// places them into the a429_message.
//---------------------------------------------------------------------------
static void package_queue_contents(FACE_A429_MESSAGE_TYPE * A429Data, QUEUE * queue)
{
   uint16_t i = 0;
   while(!queue_empty(queue))
   {
      A429Data->data[i++] = htonl(queue_remove(queue));
   }
   A429Data->num_labels = i;
}

//---------------------------------------------------------------------------
// unpack_words_into_queue is used to put a429 words into queues.
// This method takes the words from the a429_message and places them into
// the appropriate connection's queue.
//---------------------------------------------------------------------------
static void unpack_words_into_queue(FACE_A429_MESSAGE_TYPE * A429Data, QUEUE * queue)
{
   uint16_t i;
   for(i = 0; i < A429Data->num_labels; i++)
   {
      queue_add(queue, ntohl(A429Data->data[i]));
   }
}

//---------------------------------------------------------------------------
// discrete_write is used to write a state to hardware.
// This method takes the discrete state in the face message and write it to 
// the appropriate channel.
//---------------------------------------------------------------------------
static FACE_RETURN_CODE_TYPE discrete_write(FACE_IO_MESSAGE_TYPE * faceMsg)
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

//---------------------------------------------------------------------------
// a429_write is used to write information to A429 hardware channel.
// This method takes the information in the face message and unpacks it into
// the appropriate queue to be written to hardware.
//---------------------------------------------------------------------------
static FACE_RETURN_CODE_TYPE a429_write(FACE_IO_MESSAGE_TYPE * faceMsg)
{
   FACE_A429_MESSAGE_TYPE * A429Data = (FACE_A429_MESSAGE_TYPE *)faceMsg->data;
   A429_CONNECTION * connection = get_connection_by_channel(A429Data->channel, FACE_TRANSMIT);

   if(connection != NULL)
      unpack_words_into_queue(A429Data, &connection->q);
   else
      printf("Error: Could not find connection.\n");
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
   numconectionData = MAX_CONNECTION_DATA;
   if(ParseConfigFile(configuration_file + 1, configData, &numconectionData))
   {
      printf("  Successfully parsed config file, found %d connections\n",numconectionData);

      board = 0;

      // load the board (address, port, and size are not used - must specify zero)
      status = ar_loadslv(board,0,0,0);
      if (status != ARS_NORMAL)
         error_exit(board,status);

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
            connection.info = &configData[i];
            connection.handle = (FACE_INTERFACE_HANDLE_TYPE)(((long)i) + 1);
            queue_init(&connection.q);
            connectionQueues[numQueue] = connection;

            if(connection.info->direction == FACE_TRANSMIT)
            {
               status = ar_set_config(board, (CEI_INT16)(ARU_TX_CH01_BIT_RATE + connection.info->channel - 1), connection.info->a429Speed);
            }
            else if (connection.info->direction == FACE_RECEIVE)
            {
               status = ar_set_config(board, (CEI_INT16)(ARU_RX_CH01_BIT_RATE + connection.info->channel - 1), connection.info->a429Speed);
            }
            else
               printf("Unable to init");

            if (status != ARS_NORMAL)
               error_exit(board,status);

            numQueue++;
         }
      }

      // launch the board
      status = ar_go(board);
      if (status != ARS_NORMAL)
         error_exit(board,status);

      _beginthread( a429_handler, 0, NULL );

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
// After reading this information, this method calls the appropriate bus type 
// then retrieves the value from the hardware and copies it to the memory 
// location specified by the data buffer address.
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
      *return_code = discrete_read(faceMsg);
   }
   else if (faceMsg->busType == FACE_ARINC_429)
   {
      *return_code = a429_read(faceMsg);
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
// After reading this information, this method writes the desired information
// to the appropriate bus type.
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
      *return_code = discrete_write(faceMsg);
   }
   else if (faceMsg->busType == FACE_ARINC_429)
   {
      *return_code = a429_write(faceMsg);
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

   rxFaceMsg->guid = htonl(IO_SEG_GUID);
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

   rxFaceMsg->guid = htonl(IO_SEG_GUID);
   rxFaceMsg->busType = FACE_SERIAL;
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

   txFaceMsg->guid = htonl(IO_SEG_GUID);
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

   txFaceMsg->guid = htonl(IO_SEG_GUID);
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

   txFaceMsg->guid = htonl(IO_SEG_GUID);
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

// Returns true if full is one less the size of the queue
_Bool TEST_QUEUE_BURNS_SLOT()
{
   QUEUE q;
   int i = 0;
   queue_init(&q);


   while(!queue_full(&q))
      queue_add(&q, i++);

   printf("  Number of items in queue: %d\n", i);

   return i = MAXQUEUE - 1;
}

// Returns true if the amount read information from the
// queue is the same as the ammount put into it
_Bool TEST_QUEUE_CONTENTS()
{
   QUEUE q;
   int i = 0;
   int j = 0;
   queue_init(&q);


   while(!queue_full(&q))
      queue_add(&q, i++);

   while(!queue_empty(&q))
   {
      queue_remove(&q);
      j++;
   }

   return i == j;
}

// Returns true when a new queue is empty
_Bool TEST_NEW_QUEUE_IS_EMPTY()
{
   QUEUE q;
   int i = 0;
   queue_init(&q);

   return queue_empty(&q) && !queue_full(&q);
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
   txFaceMsg->guid = htonl(IO_SEG_GUID);
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
   txFaceMsg->guid = htonl(IO_SEG_GUID);
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
   rxFaceMsg->guid = htonl(IO_SEG_GUID);   // Pick a couple of numbers
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

// Returns true if ar_putword is able to put a word to the hardware
// and ar_getword gets the same word back
_Bool TEST_A429_HARDWARE_RW_GOOD()
{
   CEI_INT16 status;        // library status value
   CEI_INT32 xmit_word = 0x12345678;     // word sent out the transmitter
   CEI_INT32 recv_word;     // stores received word

   // Turn on wrapping for test
   status = ar_set_config(board,ARU_INTERNAL_WRAP,AR_WRAP_ON);
   if (status != ARS_NORMAL)
      return FALSE;

   printf("  Writing %d to the board\n", xmit_word);

   // write a word out the transmitter
   status = ar_putword(board,2,xmit_word);
   if (status != ARS_NORMAL)
      return FALSE;

   printf("  Attempting to recieve a word\n");

   // attempt to receive a word
   status = ar_getnext(board,2,&recv_word);
   if (status != ARS_GOTDATA)
      return FALSE;

   printf("  Read %d from the board\n", recv_word);

   // Turn off wrapping, done testing
   status = ar_set_config(board,ARU_INTERNAL_WRAP,AR_WRAP_OFF);
   if (status != ARS_NORMAL)
      return FALSE;

   return xmit_word == recv_word;
}

// Returns true if the FACE_IO_WRITE successfully writes a word to
// the board and FACE_IO_READ gets the same word back
_Bool TEST_FACE_IO_A429_RW_GOOD()
{
   FACE_RETURN_CODE_TYPE retCode;
   FACE_INTERFACE_HANDLE_TYPE dataLogHandle;
   FACE_INTERFACE_HANDLE_TYPE igsHandle;

   FACE_MESSAGE_LENGTH_TYPE messLen;
   static uint32_t dummyData1 = 0x12345678;  // Not a valid Arinc - I am just testing
   char txBuff[MAX_BUFF_SIZE];
   char rxBuff[MAX_BUFF_SIZE];
   CEI_INT16 status;        // API status value

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;
   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;
   FACE_A429_MESSAGE_TYPE * txA429Data = (FACE_A429_MESSAGE_TYPE *)txFaceMsg->data;
   FACE_A429_MESSAGE_TYPE * rxA429Data = (FACE_A429_MESSAGE_TYPE *)rxFaceMsg->data;

   status = ar_set_config(board,ARU_INTERNAL_WRAP,AR_WRAP_ON);
   if (status != ARS_NORMAL)
      error_exit(board,status);

   // Zero them out
   memset(txBuff, 0, MAX_BUFF_SIZE);
   memset(rxBuff, 0, MAX_BUFF_SIZE);

   // Set the fixed fields - make sure to convert to network order when needed
   txFaceMsg->guid = htonl(IO_SEG_GUID);
   rxFaceMsg->guid = htonl(IO_SEG_GUID + 1);
   txFaceMsg->busType =  FACE_ARINC_429;
   rxFaceMsg->busType = FACE_ARINC_429;
   txFaceMsg->message_type = htons(FACE_DATA);
   rxFaceMsg->message_type = htons(FACE_DATA);

   // Open the connections.
   FACE_IO_Open("A429_TX_DATALOGGER", &dataLogHandle, &retCode);
   printf("I/O API - Return codes for Open are datalogger %d ", retCode);
   FACE_IO_Open("A429_RX_IGS", &igsHandle, &retCode);
   printf("IGS %d\n", retCode);

   // Send a IGS Message
   FaceSetPayLoadLength(txFaceMsg,sizeof(FACE_A429_MESSAGE_TYPE) + 4);  // Send two words
   txA429Data->channel = 1;
   txA429Data->num_labels = 1;
   txA429Data->data[0] = htonl(dummyData1);
   FACE_IO_Write(igsHandle, 5000000000LL, FACE_MSG_HEADER_SIZE + FacePayLoadLength(txFaceMsg), txFaceMsg, &retCode);
   if (retCode == FACE_NO_ERROR)
      printf ("I/O API Test bed - IO Seg just wrote IGS with no error.\n");
   else
      printf ("I/O API Test bed - IO Seg just wrote IGS with error: %d.\n", retCode);

   Sleep(500);

   // Read the Data Logger Message
   messLen = FACE_MSG_HEADER_SIZE + sizeof(FACE_A429_MESSAGE_TYPE);
   rxA429Data->channel = 1;
   //rxA429Data->num_labels = 2;
   FACE_IO_Read(dataLogHandle, 5000000000LL, &messLen, rxFaceMsg, &retCode);   // Give it time for other side to send
   if (retCode == FACE_NO_ERROR)
      printf ("I/O API Test bed - IO Seg just read DataLog no error.  length: %d. A429 Word was: %0X.\n", rxA429Data->num_labels, ntohl(rxA429Data->data[0]));
   else
      printf ("I/O API Test bed - IO Seg just read DataLog with error: %d.\n", retCode);

   status = ar_set_config(board,ARU_INTERNAL_WRAP,AR_WRAP_OFF);
   if (status != ARS_NORMAL)
      error_exit(board,status);

   FACE_IO_Close(dataLogHandle, &retCode);
   FACE_IO_Close(igsHandle, &retCode);

   return ntohl(rxA429Data->data[0]) == dummyData1;
}

// Returns true if the FACE_IO_READ is unable to get any input
_Bool TEST_FACE_IO_A429_READ_NOTHING()
{
   FACE_RETURN_CODE_TYPE retCode;
   FACE_INTERFACE_HANDLE_TYPE dataLogHandle;
   FACE_INTERFACE_HANDLE_TYPE igsHandle;

   FACE_MESSAGE_LENGTH_TYPE messLen;
   static uint32_t dummyData1 = 0x12345678;  // Not a valid Arinc - I am just testing
   char rxBuff[MAX_BUFF_SIZE];
   CEI_INT16 status;        // API status value

   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;
   FACE_A429_MESSAGE_TYPE * rxA429Data = (FACE_A429_MESSAGE_TYPE *)rxFaceMsg->data;

   status = ar_set_config(board,ARU_INTERNAL_WRAP,AR_WRAP_ON);
   if (status != ARS_NORMAL)
      error_exit(board,status);

   // Zero them out
   memset(rxBuff, 0, MAX_BUFF_SIZE);

   FACE_IO_Open("A429_TX_DATALOGGER", &dataLogHandle, &retCode);
   printf("I/O API - Return codes for Open are datalogger %d ", retCode);
   FACE_IO_Open("A429_RX_IGS", &igsHandle, &retCode);
   printf("IGS %d\n", retCode);

   // Set the fixed fields - make sure to convert to network order when needed
   rxFaceMsg->guid = htonl(IO_SEG_GUID + 1);
   rxFaceMsg->busType = FACE_ARINC_429;
   rxFaceMsg->message_type = htons(FACE_DATA);

   // Read the Data Logger Message
   messLen = FACE_MSG_HEADER_SIZE + sizeof(FACE_A429_MESSAGE_TYPE);
   rxA429Data->channel = 1;
   //rxA429Data->num_labels = 2;
   FACE_IO_Read(dataLogHandle, 5000000000LL, &messLen, rxFaceMsg, &retCode);   // Give it time for other side to send
   if (retCode == FACE_NO_ERROR)
      printf ("I/O API Test bed - IO Seg just read DataLog no error.  length: %d. First A429 Word was: %0X.\n", rxA429Data->num_labels, ntohl(rxA429Data->data[0]));
   else
      printf ("I/O API Test bed - IO Seg just read DataLog with error: %d.\n", retCode);

   status = ar_set_config(board,ARU_INTERNAL_WRAP,AR_WRAP_OFF);
   if (status != ARS_NORMAL)
      error_exit(board,status);

   FACE_IO_Close(dataLogHandle, &retCode);
   FACE_IO_Close(igsHandle, &retCode);

   return rxA429Data->num_labels == 0;
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
   rxFaceMsg->guid = htonl(IO_SEG_GUID);   // Pick a couple of numbers
   rxFaceMsg->busType = FACE_DISCRETE;
   rxFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(rxFaceMsg, 4);

   //IO_Seg_Initialize("1config.xml", &retCode);

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
   printf("---------------------\n\n");

   handle_test("Init - Handle bad config file", TEST_IO_SEG_INIT_INVALID_FILE);
   handle_test("Init - Config file valid", TEST_IO_SEG_INIT_GOOD);

   printf("Discrete\n");
   printf("--------\n\n");

   handle_test("Read - Unimplemented bus type", TEST_IO_SEG_READ_BAD_BUSTYPE);
   handle_test("Read - Successful", TEST_IO_SEG_READ_GOOD);

   handle_test("Write - Channels are preserved", TEST_IO_SEG_WRITE_CHANNELS_PRESERVED);

   handle_test("Write - On successful", TEST_IO_SEG_WRITE_GOOD_ON);
   handle_test("Write - Off successful", TEST_IO_SEG_WRITE_GOOD_OFF);

   printf("A429\n");
   printf("----\n\n");

   handle_test("R/W - A429 Hardware successful", TEST_A429_HARDWARE_RW_GOOD);

   printf("Queue\n");
   printf("-----\n\n");

   handle_test("Burns slot", TEST_QUEUE_BURNS_SLOT);
   handle_test("New queue is empty", TEST_NEW_QUEUE_IS_EMPTY);
   handle_test("Can remove all contents", TEST_QUEUE_CONTENTS);

   printf("Unit Tests Complete\n\n\n");

   printf("Integration Tests\n");
   printf("-----------------\n\n");

   printf("Discrete\n");
   printf("--------\n\n");


   handle_test("FACE_IO Init - Success", TEST_FACE_IO_INITIALIZE);
   handle_test("FACE_IO Open - Success", TEST_FACE_IO_OPEN);
   handle_test("FACE_IO Write On - Success", TEST_FACE_IO_WRITE_ON);
   handle_test("FACE_IO Write Off - Success", TEST_FACE_IO_WRITE_OFF);
   handle_test("FACE_IO Read - Success", TEST_FACE_IO_READ);
   handle_test("FACE_IO Close - Success", TEST_FACE_IO_CLOSE);

   printf("A429\n");
   printf("----\n\n");

   handle_test("FACE_IO A429 Read/Write - Success", TEST_FACE_IO_A429_RW_GOOD);
   handle_test("FACE_IO_A429 Read - Read Nothing", TEST_FACE_IO_A429_READ_NOTHING);

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