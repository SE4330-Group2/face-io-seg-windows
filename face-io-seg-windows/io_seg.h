//face-io-lib
#include <config_parser.h>
#include <face_messages.h>

//ARINC Board
#include "utildefs.h"

// For when PSS and I/O Seg are in the same program and I/O Lib calls I/O Lib 
void IO_Seg_Initialize
( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
  /* out */ FACE_RETURN_CODE_TYPE *return_code);

// For Direct Read when PSS and I/O Seg are in the same program
void IO_Seg_Read
( /* inout */ FACE_MESSAGE_LENGTH_TYPE *message_length,
  /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
  /* out */ FACE_RETURN_CODE_TYPE *return_code);

// For Direct Write when PSS and I/O Seg are in the same program
void IO_Seg_Write
(  /* in */ FACE_MESSAGE_LENGTH_TYPE message_length,
   /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code);