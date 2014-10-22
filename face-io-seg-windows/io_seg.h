//---------------------------------------------------------------------------
// Prototype 2
// This is the header file for io_seg.c which will be used by the I/O library
// when it needs to access hardware.
//---------------------------------------------------------------------------
//face-io-lib
#include <config_parser.h>
#include <face_messages.h>

//ARINC Board
#include "utildefs.h"

//---------------------------------------------------------------------------
// IO_Seg_Initialize is used to initialize the hardware.
// This method reads an XML config file and sets hardware accordingly.
// The config file should be a file name preceded by a character which will
// be ignored.
//---------------------------------------------------------------------------
void IO_Seg_Initialize
( /* in */ const FACE_CONGIGURATION_FILE_NAME configuration_file,
  /* out */ FACE_RETURN_CODE_TYPE *return_code);


//---------------------------------------------------------------------------
// IO_Seg_Read is used to read the hardware.
// This method puts the read value of the hardware in the memory location
// specified by the data buffer address.
//---------------------------------------------------------------------------
void IO_Seg_Read
( /* inout */ FACE_MESSAGE_LENGTH_TYPE *message_length,
  /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
  /* out */ FACE_RETURN_CODE_TYPE *return_code);

//---------------------------------------------------------------------------
// IO_Seg_Write is used to write to the hardware.
// This method puts the read value of the hardware in the memory location
// specified by the data buffer address.
//---------------------------------------------------------------------------
void IO_Seg_Write
(  /* in */ FACE_MESSAGE_LENGTH_TYPE message_length,
   /* in */ FACE_MESSAGE_ADDR_TYPE data_buffer_address,
   /* out */ FACE_RETURN_CODE_TYPE *return_code);