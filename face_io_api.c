// Need more comments

/*********************************************************************************
Assumes the first character in configFileName passed to
FACE_IO_Initialize specifies whether this module lives
in the same program as the I/O Seg or in the PSS Seg.
  0 = The API is on the PSS side
  1 = The API is on the I/O Seg Side
  2 = I/O Seg and PSS are in the same program, Init was called by PSS
The rest of configFileName is a file name.
The header file associated with this C file is the FACE standard file ios.h
**********************************************************************************/

// In Visual, to set struct alignment to 1 byte, do:
//   Project -> Properties -> Configuration Properties -> C/C++ -> Code Generation -> Struct Memory Alignment -> 1 Byte
// Need to check for gcc

// This was for my testing - you comment it out
#define TEST_MAIN


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "globals.h"
   #include "face_common.h"
   #include "face_ios.h"
#include "config_parser.h"

#ifdef LINUX_OS
   #include <sys/socket.h>
   #include <sys/select.h>
   #include <netinet/in.h>
#endif

#ifdef WINDOWS_OS
   #include <winsock2.h>
   #pragma comment(lib,"Ws2_32.lib")  // Yes, it's just that easy to get posix sockets in Windows!
#endif

// For when PSS and I/O Seg are in the same program and I/O Lib calls I/O Lib 
extern void IO_Seg_Initialize
     ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
       /* out */ FACE_RETURN_CODE_TYPE *return_code);

// For Direct Read when PSS and I/O Seg are in the same program
extern void IO_Seg_Read
   ( /* inout */ FACE_MESSAGE_LENGTH_TYPE *message_length,
     /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
     /* out */ FACE_RETURN_CODE_TYPE *return_code);

// For Direct Write when PSS and I/O Seg are in the same program
extern void IO_Seg_Write
   (  /* in */ FACE_MESSAGE_LENGTH_TYPE message_length,
      /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
      /* out */ FACE_RETURN_CODE_TYPE *return_code);

typedef struct
{
   uint32_t                      handle;                   // API handle to use for Reads, Writes, etc. - 0 means not assigned

   // Will need these later
   int                          serverSocket;             // Socket used to do Reads
   struct sockaddr_in           serverAddr;               // Address used to do Reads
   int                          clientSocket;             // Socket used to do Writes
   struct sockaddr_in           clientAddr;               // Address used to do Writes
} CONECTION_DATA_TYPE;


#define MAX_conectionData 10

// Parallel arrays to manage the connection
static FACE_CONFIG_DATA_TYPE  configData[MAX_conectionData];
static CONECTION_DATA_TYPE conectionData[MAX_conectionData];


// Specifies whether this API is on the IO Segment side or not
static _Bool isItIOSeg;

// Number of Configured conectionData
static uint32_t numconectionData = 0;

// Private Functions
static int InterfaceNameSearch(const char * name);
static _Bool CreateSockets(void);



/*************************************************************************
*********************  Public Methods ***********************************
*************************************************************************/

// Note that all must be reentrant, so all variables must be local.

// For the handles, I will simply use:  handle_num = slot_num + 1
// 0 will be reserved to mean an unused handle



// First character of the passed configuration_file
// must be a digit where:
//     0 = The API is on the PSS side
//     1 = The API is on the I/O Seg Side
//     2 = I/O Seg and PSS are in the same program, Init was called by PSS
// The rest is a file name, full path or relative to execution directory.
void FACE_IO_Initialize
   ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   uint32_t i;
   _Bool success;

   if ( configuration_file[0] != '0' && configuration_file[0] != '1' && configuration_file[0] != '2')
   {
      *return_code = FACE_INVALID_PARAM;
      return;
   }

   isItIOSeg = configuration_file[0] != '0';   // Assume yes except for '0'

   numconectionData = MAX_conectionData;
   success = PasrseConfigFile(configuration_file + 1, configData, &numconectionData);

   for (i = 0; i < numconectionData; i++)
   {
      conectionData[i].handle = 0;    // Initialize to unused
   }

   if(success)
   {
      if (configuration_file[0] == '2')
      {
         IO_Seg_Initialize (configuration_file, return_code);
         if (*return_code != FACE_NO_ERROR)
         {
            return;
         }
      }
      CreateSockets();                // Not sure what to do if this fails - no FACE_RETURN_CODE_TYPE seems appropriate

      *return_code = FACE_NO_ERROR;
   }
   else
   {
      *return_code = FACE_INVALID_CONFIG;
   }
}

void FACE_IO_Open
   ( /* in */ const FACE_INTERFACE_NAME_TYPE name,
   /* out */ FACE_INTERFACE_HANDLE_TYPE *handle,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{

   int index = InterfaceNameSearch(name);

   if(index < 0)
   {
      *return_code = FACE_INVALID_PARAM;
   }
   else
   {
      if(conectionData[index].handle > 0 )
      {
         *return_code = FACE_ADDR_IN_USE;
      }
      else
      {
         // For now, the handle will simply be the index+1 in the array.
         // That can change if a more complicated scheme is needed.
         *return_code = FACE_NO_ERROR;
         *handle = (void *)((long)(index + 1));   // / Need double cast to remove warnings from IO and Windows
         conectionData[index].handle = index + 1;
      }
   }
}

void FACE_IO_Register
   ( /* in */ FACE_INTERFACE_HANDLE_TYPE handle,
   /* in */ FACE_CALLBACK_ADDRESS_TYPE callback_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   // Don't implement for now.
   *return_code = FACE_NOT_AVAILABLE;
}

void FACE_IO_Read
   ( /* in */ FACE_INTERFACE_HANDLE_TYPE handle,
   /* in */ FACE_TIMEOUT_TYPE timeout,
   /* inout */ FACE_MESSAGE_LENGTH_TYPE *message_length,
   /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   uint32_t iHandle = (uint32_t)((long)handle);      // Need double cast to remove warnings from IO and Windows                  
   int index = iHandle - 1;                          // Handle is one more than the array possition

   if (iHandle > numconectionData || iHandle <= 0)
   {
      *return_code = FACE_INVALID_PARAM;
   }
   else if (conectionData[index].handle <= 0)
   {
      *return_code = FACE_CONNECTION_CLOSED;
   }
   else if (configData[index].connectionType == FACE_DIRECT_CONNECTION)
   {
      IO_Seg_Read(message_length, data_buffer_address, return_code);
   }
   else if (configData[index].connectionType == FACE_UDP_CONNECTION)
   {
      int32_t rcvResult;
      fd_set fdset;          // Set up file descriptors to monitor (just this one)
      int selResult;
      struct timeval timeoutTime;

      // Set it up to use the timeout
      FD_ZERO(&fdset);
      FD_SET(conectionData[index].serverSocket, &fdset);          // monitor this one
      timeoutTime.tv_sec = (long)(timeout / 1000000000UL);
      timeoutTime.tv_usec = (timeout % 1000000000UL) / 1000;      // Convert nanoseconds to microseconds
      selResult = select(conectionData[index].serverSocket + 1,   // Highest descriptor + 1
         &fdset,                                                  // input set
         NULL,                                                    // output set
         NULL,                                                    // error set
         &timeoutTime);                                           // timeout

      if(selResult <= 0)
      {
         *return_code = FACE_TIMED_OUT;
      }
      else
      {
         rcvResult = recvfrom( conectionData[index].serverSocket,
                               (char *)data_buffer_address, *message_length, 0, NULL, NULL);
         if(rcvResult <= 0)
         {
            *return_code = DATA_BUFFER_TOO_SMALL;   // There are other errors it could be, they could be checked
         }
         else
         {
            *message_length = rcvResult;
            *return_code = FACE_NO_ERROR;
         }
      }
   }
   else
   {
      // Unknown FACE_CONNECTION_TYPE
      *return_code = FACE_INVALID_PARAM;
   }
}


void FACE_IO_Write
   ( /* in */ FACE_INTERFACE_HANDLE_TYPE handle,
   /* in */ FACE_TIMEOUT_TYPE timeout,
   /* in */ FACE_MESSAGE_LENGTH_TYPE message_length,
   /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{

   uint32_t iHandle = (uint32_t)((long)handle);      // Need double cast to remove warnings from IO and Windows 
   int index = iHandle - 1;                          // Handle is one more than the array position

      if( iHandle > numconectionData )
   {
      *return_code = FACE_INVALID_PARAM;
   }
   else if ( conectionData[index].handle <= 0 )
   {
      *return_code = FACE_CONNECTION_CLOSED;
   }
   else if (configData[index].connectionType == FACE_DIRECT_CONNECTION)
   {
      IO_Seg_Write(message_length, data_buffer_address, return_code);
   }
   else if (configData[index].connectionType == FACE_UDP_CONNECTION)
   {
      int32_t len;
      int sendResult;
      fd_set fdset;
      int selResult;
      struct timeval timeoutTime;

      // Set it up to use the timeout
      FD_ZERO(&fdset);
      FD_SET(conectionData[index].clientSocket, &fdset);           // monitor this one

      timeoutTime.tv_sec = (long)(timeout / 1000000000UL);
      timeoutTime.tv_usec = (timeout % 1000000000UL) / 1000;       // Convert nanoseconds to microseconds
      selResult = select(conectionData[index].serverSocket + 1,    // Highest descriptor + 1
         &fdset,        // input set
         NULL,         // output set
         NULL,         // error set
         &timeoutTime);  // timeout
      if(selResult <= 0)
      {
         *return_code = FACE_TIMED_OUT;
      }
      else
      {
         len = sizeof(conectionData[index].clientAddr);
         sendResult = sendto( conectionData[index].clientSocket, (char *)data_buffer_address, message_length, 0,
            (struct sockaddr *)&conectionData[index].clientAddr, len);
         if(sendResult < 0)
         {
            *return_code = FACE_TIMED_OUT;  // None really match
         }
         else
         {
            *return_code = FACE_NO_ERROR;
         }
      }
   }
   else
   {
      // Unknown FACE_CONNECTION_TYPE
      *return_code = FACE_INVALID_PARAM;
   }
}


void FACE_IO_Get_Status
   ( /* in */ FACE_INTERFACE_HANDLE_TYPE handle,
   /* out */ FACE_STATUS_TYPE *status,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   // Don't implement for now.
   *return_code = FACE_NOT_AVAILABLE;
}


void FACE_IO_Close
   ( /* in */ FACE_INTERFACE_HANDLE_TYPE handle,
   /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   uint32_t iHandle = (uint32_t)((long)handle);      // Need double cast to remove warnings from IO and Windows 
   int index = iHandle - 1;                          // Handle is one more than the array possition

   if (iHandle > 0 && iHandle <= numconectionData)
   {
      conectionData[index].handle = 0;
      *return_code = FACE_NO_ERROR;
   }
   else
   {
      *return_code = FACE_INVALID_PARAM;
   }
}


/*************************************************************************
*********************  Private Methods ***********************************
*************************************************************************/


// Searches nameToPort to find if index of name.  Returns -1 if name not found.
static int InterfaceNameSearch(const char * name)
{
   uint32_t i;

   for(i = 0; i < numconectionData; ++i)
   {
      if (strncmp(name, configData[i].name, sizeof(FACE_INTERFACE_NAME_TYPE)) == 0)
      {
         return i;
      }
   }
   return -1;
}


static _Bool CreateSockets(void)
{
   // TBD
   return true;
}




//********************************************************************************************************************
//********************************************************************************************************************
//*******************  TESTBED MAIN  *********************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************

// Use these as samples of what you will need in the PSS and I/O Seg.

#define TESTING
#ifdef TESTING

#include <stdio.h>
#include "face_messages.h"

#define MAX_BUFF_SIZE  1024

int main()
{
   int i;
   FACE_RETURN_CODE_TYPE retCode;
   static uint8_t bitVal = 1;
   FACE_INTERFACE_HANDLE_TYPE wowHandle;
   FACE_INTERFACE_HANDLE_TYPE emergencyHandle;
   int wowIndex;
   int emergencyIndex;
   FACE_MESSAGE_LENGTH_TYPE messLen;
   

   char txBuff[MAX_BUFF_SIZE];
   char rxBuff[MAX_BUFF_SIZE];

   FACE_IO_MESSAGE_TYPE * txFaceMsg = (FACE_IO_MESSAGE_TYPE *)txBuff;
   FACE_IO_MESSAGE_TYPE * rxFaceMsg = (FACE_IO_MESSAGE_TYPE *)rxBuff;

   // Zero them out
   memset(txBuff, 0, MAX_BUFF_SIZE);
   memset(rxBuff, 0, MAX_BUFF_SIZE);

   // Set the fixed fields - make sure to convert to network order when needed
   txFaceMsg->guid = htonl(100);
   rxFaceMsg->guid = htonl(200);   // Pick a couple of numbers
   txFaceMsg->busType = FACE_DISCRETE;
   rxFaceMsg->busType = FACE_DISCRETE;
   txFaceMsg->message_type = htons(FACE_DATA);
   rxFaceMsg->message_type = htons(FACE_DATA);
   FaceSetPayLoadLength(txFaceMsg, 4);
   FaceSetPayLoadLength(rxFaceMsg, 4);

   printf("Starting testbed main for I/O API\n");

   // Call Init
   FACE_IO_Initialize("1config.xml", &retCode);

   if (retCode == FACE_NO_ERROR)
   {
      printf("Config file parsed okay.  Number of devices is %d\n", numconectionData);
   }
   else
   {
      printf("Error parsing config file.\n");
   }

   // Get the channel numbers from config and set them in the messages
   wowIndex = InterfaceNameSearch("DISCRETE_INPUT_WOW");
   emergencyIndex = InterfaceNameSearch("DISCRETE_OUTPUT_EMERGENCY");
   FaceSetDiscreteChannelNumber(txFaceMsg, configData[emergencyIndex].channel);
   FaceSetDiscreteChannelNumber(rxFaceMsg, configData[wowIndex].channel);


   // Open the connections.
   FACE_IO_Open("DISCRETE_INPUT_WOW", &wowHandle, &retCode);
   FACE_IO_Open("DISCRETE_OUTPUT_EMERGENCY", &emergencyHandle, &retCode);


   // Read and write a few of messages, pretending to be PSS
   for (i = 0; i < 3; i++)
   {
      // Transmit
      FaceSetDiscreteState(txFaceMsg, bitVal);
      printf ("PSS going to to write a %d to Emergency channel number %d.\n", bitVal, FaceDiscreteChannelNumber(txFaceMsg));
      FACE_IO_Write(emergencyHandle, 0, FACE_MSG_HEADER_SIZE + 4, txFaceMsg, &retCode);
      if (retCode == FACE_NO_ERROR)
      {
          printf ("PSS Write worked.\n", bitVal);
      }
      else
      {
          printf ("PSS Write failed with: %d.\n", retCode);
      }
      bitVal = 1 - bitVal;

      // Receive
      printf ("PSS going to try to read WOW on channel %d.\n", FaceDiscreteChannelNumber(rxFaceMsg));
      messLen = FACE_MSG_HEADER_SIZE + 4;
      FACE_IO_Read(wowHandle, 0, &messLen, rxFaceMsg, &retCode);
      if (retCode == FACE_NO_ERROR)
      {
          printf ("PSS Read worked.  Value read was %d.  Number of bytes in message was: %d.\n", FaceDiscreteState(rxFaceMsg), messLen);
      }
      else
      {
          printf ("PSS Write failed with: %d.\n", retCode);
      }
   }

   FACE_IO_Close(wowHandle, &retCode);
   FACE_IO_Close(emergencyHandle, &retCode);

   return 0;
}


// Implement test bodies for the I/O Direct Function calls

//

void IO_Seg_Initialize
     ( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
       /* out */ FACE_RETURN_CODE_TYPE *return_code)
{
   // Needs to be written by IO Seg.  Will parse the file 
   *return_code = FACE_NO_ERROR;
}


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



#endif
