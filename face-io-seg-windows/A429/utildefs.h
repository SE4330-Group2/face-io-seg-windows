/*===========================================================================*
 * FILE:                       U T I L D E F S . H
 *===========================================================================*
 *
 *      COPYRIGHT (C) 1998-2010 BY 
 *      GE INTELLIGENT PLATFORMS EMBEDDED SYSTEMS, INC. 
 *      ALL RIGHTS RESERVED.
 *
 *      THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND
 *      COPIED ONLY IN ACCORDANCE WITH THE TERMS OF SUCH LICENSE AND WITH
 *      THE INCLUSION OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE OR ANY
 *      OTHER COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE
 *      AVAILABLE TO ANY OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF THE
 *      SOFTWARE IS HEREBY TRANSFERRED.
 *
 *      THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT
 *      NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY GE INTELLIGENT
 *      PLATFORMS EMBEDDED SYSTEMS.
 *
 *===========================================================================*
 *
 * FUNCTION:
 *  
 *      Equates used by ARINC utilities and user applications that call them.
 *
 * DESCRIPTION: 
 *
 *      General ARINC globals and function prototypes are declared herein. 
 *      This file defines the interface between user applications and the 
 *      following CEI devices:
 *
 *         CEI-100/200
 *         CEI-220/420/520/620/820/820TX
 *         VME/VXI-AIC
 *         IP-Avionics installed on a CEI-300/500/600
 *         CEI-700  
 *
 *      Since this file is shared between many product lines, caution should
 *      be exercised when performing modifications.
 *
 * HISTORY:
 *
 *   Date     By   Vers                        Comments
 * --------  ----  ----  -----------------------------------------------------
 * 05/11/96  rt       1  Made WIN32 compatible.
 * 03/11/97  rt       2  Made all functions stdcall types.
 * 03/28/97  sli      3  Added code for cplusplus name mangling 
 * 09/01/97  dz       4  Enforced CEI formatting/readability standards. Added
 *                       missing prototypes for AR_FORCE_VERSION, 
 *                       AR_FORMATARINCLABEL, AR_FORMATBINARYLABEL,
 *                       AR_GET_LABEL_FILTER, AR_GET_RAW_MODE, and 
 *                       AR_UNFORMAT. Reorganized prototypes -- now grouped by
 *                       API (those functions that appear in the CEI-100/200 
 *                       User's Manual) and utility (all others). Functions
 *                       are organized lexicographically within these two
 *                       groups.
 * 10/27/97  dz       5  Last modification changed label types in calls to
 *                       AR_GET_LATEST, AR_LABEL_FILTER and 
 *                       AR_GET_LABEL_FILTER to unsigned char. This was 
 *                       incompatible with the constant ARU_ALL_LABELS (0x1FF)
 *                       and was thus changed to unsigned short.                               
 * 02/04/98  sli      6  Added support for the CEI-220.
 * 02/04/98  ajh      7  Changed calling sequences to support CEI-220.
 * 02/10/98  sli      8  Added more support for the CEI-220.
 * 02/21/98  ajh      9  Added parametric support for the CEI-220.
 * 02/27/98  ajh     10  Added transmitter shutdown & comments for CEI-220.
 * 04/14/98  ajh     11  Final changes for CEI-220 (CEI-x20 v1.10).
 * 08/13/98  ajh     12  Added support for the CEI-420 board (CEI-x20 v1.30).
 * 12/09/98  sli     13  Added support for the PA-100.
 * 12/12/98  sli     14  Change AR_GETWORDT to resolve linking problem.
 * 12/16/98  ajh     15  Added ability to select scheduled or burst mode on a
 *                       per-channel basis for CEI-220/420 (CEI-x20 v2.20).
 * 01/12/99  sli     16  Fixed interface.dll problem.
 * 01/25/99  bw      17  Added IP429HD to the interface.
 * 02/18/99  bw      18  Added CEI-710 to the interface.
 * 03/17/99  ajh     19  Added support for the CEI-520 (CEI-x20 v2.22).
 * 04/07/99  ajh     20  Completed support for the CEI-520 (CEI-x20 v2.31).
 * 04/15/99  ajh     21  Merged versions back together (CEI-x20 v2.33).
 * 08/27/99  dz      22  Merged ajh version with BusTools/ARINC and CEI-LV
 *                       versions.
 * 12/03/99  ajh     23  Merged versions together (CEI-x20 v3.00).
 * 04/29/00  ajh     24  Added definitions for new CEI-520/620 interface
 *                       definitions (CEI-x20 v3.02).
 * 05/24/00  ajh     25  Removed hardware definitions (CEI-x20 v3.04).
 * 07/16/00  ajh     26  Updated to support CEI-x20 CSDB channels (CEI-x20 
 *                       v3.06).
 * 06/08/01  ajh     27  Corrected the definition of the labels
 *                       ARU_CH13_DISABLE_TIMETAG - ARU_CH16_DISABLE_TIMETAG 
 *                       (CEI-x20 v3.26).
 * 09/27/01  skb     28  Expanded explanation of the 'BaudRate' member in the
 *                       AR_CHANNEL_PARMS structure to include a description
 *                       description of variable baud rate usage on the 
 *                       CEI-820 (CEI-x20 v3.40).
 * 06/06/02  skb     29  Updated to support 32 transmitters and 32 receivers 
 *                       (CEI-x20 v3.60).
 * 03/25/03  skb     30  Added support for CEI-520A (CEI-x20 v3.80).
 * 04/23/04  skb     31  Added constants to support AR_EXECUTE_BIT (CEI-x20 
 *                       v3.90).
 * 04/11/06  skb     32  Updated to use platform-independent data types.
 *                       Added enhanced filtering constants (CEI-x20 v3.95).
 * 05/23/07  skb     33  Added limited support for 'await data' mode, multi-
 *                       thread mode, and multi-process mode (CEI-x20 v3.97).
 *
 *===========================================================================*/
#ifndef UTILDEFS_H
#define UTILDEFS_H


// define _UNIX_ for CEI-x20 VxWorks/Linux builds
#if (defined(CEIX20_TARGET_VXWORKS) || defined(CEIX20_TARGET_LINUX_X86)) && !defined(_UNIX_)
#define _UNIX_
#endif

// define VXWORKS for CEI-x20 VxWorks builds
#if defined(CEIX20_TARGET_VXWORKS) && !defined(VXWORKS)
#define VXWORKS
#endif


#include "cei_types.h"


// don't include WINDOWS stuff when building CEI-x20 firmware
#ifndef i960
#define WIN32_LEAN_AND_MEAN
#if !defined(__DOS__) && !defined(_UNIX_)
   #include <windows.h>
#endif
#if defined (_WINDOWS)  
   // Microsoft C protected mode Windows program
   #define _Windows
#endif  
#if defined(_WINDLL)
   #define _Windows
#endif 
#if defined (__WIN32__)    
   // for Borland C++ compiler
   #ifndef _WIN32
      #define _WIN32
   #endif
#endif 
#if !defined (_WIN32)
   #define EXPORT __export
   #define EXPORT32
#else
   #include <windows.h>
   #if defined(__BORLANDC__)
      #define EXPORT32
      #define EXPORT __export
   #else         
      #define EXPORT32  /* */ // __declspec(dllexport)
      #define EXPORT
      #define _Windows
   #endif

   #define UINT8 unsigned short >> 8
   #define INT8  short >> 8
#endif
#endif


/*---------------------------------------------------------------------------*
 *  Create the standard WINDOWS typedef's if compiling for _UNIX_.
 *---------------------------------------------------------------------------*/
#ifdef _UNIX_

#ifndef TARGET_DEFINES_H

  typedef CEI_UINT16   WORD;
  typedef CEI_UINT32   DWORD;
  typedef CEI_UINT32   HWND;
  typedef CEI_CHAR     *LPSTR;
  typedef int          *LPINT; 
  typedef WORD         *LPWORD;
  typedef CEI_INT32    *LPLONG;
  typedef DWORD        *LPDWORD;
  typedef void         *LPVOID;
  #ifndef HANDLE
  #define HANDLE       CEI_INT32
  #endif
  #define VOID         void
  #define CALLBACK
  #define NOMANGLE
  #define CCONV
  #define MAX_BTA      16
  #ifndef CEIX20_TARGET_VXWORKS
     typedef unsigned int UINT;
  #endif

#endif //TARGET_DEFINES_H

#define HUGE
#define FAR
#define far

#endif

/*---------------------------------------------------------------------------*
 *  CONSTANTS -- The following constants need to be exported in the interface.
 *---------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE                   0
#endif
#ifndef TRUE
#define TRUE                    1
#endif


/*----------------------------------------------------------------------------*
 *  Board Type equates. These are returned to callers of the API. The CEI-x20 
 *  API "OR"s the base board type with the options (IS_6WIRE, IS_CSDB, or 
 *  IS_717). The base board type can be extracted by "AND"ing with the mask
 *  API_BOARD_TYPE_MASK. Use AR_GET_BOARDTYPE to return the CEI-x20 board 
 *  type value.
 *----------------------------------------------------------------------------*/
#define MAX_BOARD_TYPES         18
#define MAX_IP_SLOTS            6

#define PA_100                  0
#define CEDEMO                  1    /* Driver only supports demo mode--no HW */
#define CEI_100                 2    /* CEI-100 board is selected HW          */
#define CEI_200                 3    /* CEI-200 board is selected HW          */
#define CEI_220                 4    /* CEI-220 board is selected HW          */
#define CEI_420                 5    /* CEI-420 board is selected HW          */
#define CEI_520                 6    /* CEI-520 board is selected HW          */
#define CEI_710                 7    /* CEI-710 board is selected HW          */
#define IP429HD                 8    /* IP429HD board is selected HW          */
#define VME_AIC                 9    /* VME-AIC board is selected HW          */
#define CEI_620                10    /* CEI-620 board is selected HW          */
#define CEI_820                11    /* CEI-820 board is selected HW          */
#define IP708                  12    /* IP-708 board is selected HW           */
#define CEI_420A               13    /* CEI-420A board is selected HW         */
#define CEI_420A_12            14    /* CEI-420A (12 MHz) is selected HW      */
#define CEI_420_70J            15    /* CEI-420-70J (12 MHz) reduced HW       */
#define CEI_715                16    /* CEI-715 board is selected HW          */
#define CEI_820TX              17    /* CEI-820TX board is selected HW        */
#define CEI_520A               18    /* CEI-520A board is selected HW         */

#define IS_6WIRE              0x1000 /* CEI-x20 board with 6-wire support     */
#define IS_717                0x2000 /* CEI-x20 board with -717/573 support   */
#define IS_CSDB               0x4000 /* CEI-x20 board with CSDB support       */

#define API_BOARD_TYPE_MASK   0x00FF /* Extract the base board type           */


/*----------------------------------------------------------------------------*
 *  CEI-520/620/820 Implemented Channel array bit definitions
 *----------------------------------------------------------------------------*/
#define CHAN_NOT_IMPLEMENTED    0x00 /* Channel is not implemented            */
#define CHAN_ARINC429           0x01 /* Channel is an ARINC-429 channel       */
#define CHAN_ARINC717           0x02 /* Channel is an ARINC-573/717 channel   */
#define CHAN_CSDB               0x04 /* Channel is a CSDB channel             */
#define CHAN_6WIRE              0x05 /* Channel is a 561 6-wire channel       */


/*----------------------------------------------------------------------------*
 *  ARINC configuration equates ==> AR_SET_ARINC_CONFIG specific.
 *----------------------------------------------------------------------------*
 *  This set of configuration equates should be used with calls to 
 *  AR_SET_ARINC_CONFIG when the target is a VME-AIC board.
 *----------------------------------------------------------------------------*/
#define A708_WORDS_PER_FRAME        107

#define RCB_SPEED_HIGH              0
#define RCB_SPEED_LOW               1
#define RCB_PARITY_ODD              0
#define RCB_PARITY_EVEN             1

#define SCB_RECV_MERGED             0
#define SCB_RECV_INDIVIDUAL         1
#define SCB_RECV_SELFTEST_IND       6
#define SCB_RECV_INDIVIDUAL_NOSDI   7

#define ARU_RX_BITRATE              1
#define ARU_TX_BITRATE              2
#define ARU_RX_PARITY               3
#define ARU_TX_PARITY               4
#define ARU_RECV_MODE               5
#define ARU_RESOLUTION              6


/*----------------------------------------------------------------------------*
 *                 V M E - A I C   Only!
 *  ARINC configuration equates ==> AR_SET_CONTROL specific (VME-AIC).
 *  These configuration equates should be used with calls to
 *   AR_SET_CONTROL to set the attribute specified in the comment field.
 *  Search for "717" to see the parameters for the CEI-x20 with the -717 option.
 *----------------------------------------------------------------------------*/
#define ARU_573_SPEED           1 /* Set the IP573 baud rate and subframe size*/
#define ARU_573_INTERNAL_WRAP   2 /* Set the IP573 internal wrap state        */
#define ARU_573_RECV_SYNC_1     3 /* Set the first IP573 receive sync word    */
#define ARU_573_RECV_SYNC_2     4 /* Set the second IP573 receive sync word   */
#define ARU_573_RECV_SYNC_3     5 /* Set the third IP573 receive sync word    */
#define ARU_573_RECV_SYNC_4     6 /* Set the forth IP573 receive sync word    */
#define ARU_708_INTERNAL_WRAP   7 /* Set the IP708 internal wrap state        */
#define ARU_708_XMIT_BUS        8 /* Set the IP708 xmit bus to A or B         */
#define ARU_708_RECV_BUS        9 /* Set the IP708 recv bus to A or B         */
#define ARU_708_MSG_GAP        10 /* Set the IP708 inter-message gap time     */

/* For the ARU_573_SPEED option, the following speed and size constants       */
/* should be OR'd together to form a single "value" parameter                 */
#define A573_SPEED_768              0
#define A573_SPEED_1536             0x10
#define A573_SPEED_3072             0x20
#define A573_SPEED_6144             0x30
#define A573_SPEED_12288            0x100
#define A573_SPEED_24576            0x110

#define A573_SUBFRAME_64            0
#define A573_SUBFRAME_128           0x40
#define A573_SUBFRAME_256           0x80
#define A573_SUBFRAME_512           0xC0

/* Constants for the ARU_573_INTERNAL_WRAP option */
#define A573_WRAP_DISABLED          0
#define A573_WRAP_ENABLED           1

/* Constants for the IP708 options */
#define A708_ENABLE                 1
#define A708_DISABLE                0
#define A708_SELECT_BUS_A           0
#define A708_SELECT_BUS_B           1
#define A708_XMIT_GAP_000           0
#define A708_XMIT_GAP_016           1
#define A708_XMIT_GAP_032           2
#define A708_XMIT_GAP_048           3
#define A708_XMIT_GAP_064           4
#define A708_XMIT_GAP_080           5
#define A708_XMIT_GAP_096           6
#define A708_XMIT_GAP_112           7


/*----------------------------------------------------------------------------*
 *  ARINC configuration equates ==> AR_SET_CONFIG SPECIFIC.  These
 *  configuration equates are included for historical purposes and are only to
 *  be used with AR_SET_CONFIG.  These should not be used with
 *  AR_SET_ARINC_CONFIG!
 *----------------------------------------------------------------------------*/
#define AR_ODD                  0  /* Used to set ODD parity                  */
#define AR_EVEN                 1  /* Used to set EVEN parity                 */
#define AR_HIGH                 0  /* Used to set HIGH speed (100 Kbaud)      */
#define AR_LOW                  1  /* Used to set LOW speed (12.5 Kbaud)      */
#define AR_WRAP_ON              0  /* Disables internal wrap-around mode      */
#define AR_WRAP_OFF             1  /* Enables internal wrap-around mode       */
#define AR_FALSE                0  /* General "False" flag                    */
#define AR_TRUE                 1  /* General "True" flag                     */
#define AR_ON                   7  /* General "Enable" flag                   */
#define AR_OFF                  8  /* General "Disable" flag                  */
#define AR_SHORT                9  /* Set low-bit error inj for entire board  */
#define AR_LONG                10  /* Clear low-bit error inj for entire board*/


/*----------------------------------------------------------------------------*
 *  Device built-in-test types.  For use with the routine AR_EXECUTE_BIT. 
 *  Refer to the AR_EXECUTE_BIT routine documentation in the User's Manual
 *  for detailed descriptions of these tests.
 *----------------------------------------------------------------------------*/
#define AR_BIT_BASIC_STARTUP    0  /* basic startup testing                   */
#define AR_BIT_PERIODIC         2  /* quick board check for active board      */
#define AR_BIT_INT_LOOPBACK     3  /* basic startup + internal loopback test  */
#define AR_BIT_EXT_LOOPBACK     4  /* basic startup + external loopback test  */


/*----------------------------------------------------------------------------*
 *  Enhanced filtering constants.  For use with the routines AR_PUTFILTER and
 *  AR_GETFILTER.
 *----------------------------------------------------------------------------*/
#define MAX_FILT                8192  /* label filt buf entries (per chan)    */
#define MAX_LABEL               256   /* label filt buf entries for labels    */
#define MAX_SDI                 4     /* label filt buf entries for SDI vals  */
#define MAX_ESSM                8     /* label filt buf entries for eSSM      */
#define FILTER_SEQUENTIAL       0x10  /* if CLEAR add label to seq rcv buf    */
#define FILTER_SNAPSHOT         0x20  /* if CLEAR add label to snapshot buf   */
#define FILTER_INTERRUPT        0x40  /* if SET add int queue entry & set IRQ */
#define FILTER_SIGNAL           0x80  /* if SET signal chan to xmit label     */
#define FILTER_CHANNEL          0x0F  /* filt buf xmtr to signal (mask)       */


/*----------------------------------------------------------------------------*
 *  The next set of configuration equates should be used with calls to 
 *  AR_SET_CONFIG when the target is a CEI300 board with an IP429HD. Parms for
 *  ar_set_config (CEI_INT16 board, CEI_INT16 item, CEI_UINT32 value) should be
 *  interpreted as (CEI_INT16 ip, CEI_INT16 item, CEI_UINT32 value) where:
 *  CEI_INT16 ip        -- IP number
 *  CEI_INT16 item      -- what to set -> valid options are:
 *                            RX1_SETUP, TX1_SETUP
 *                            RX2_SETUP, TX2_SETUP
 *                            RX3_SETUP, TX3_SETUP
 *                            RX4_SETUP, TX4_SETUP
 *                            RX5_SETUP, TX5_SETUP
 *                            RX6_SETUP, TX6_SETUP
 *                            RX7_SETUP, TX7_SETUP
 *                            RX8_SETUP, TX8_SETUP
 *
 *  CEI_UINT32 value    -- Configuration information, the 16-bit 
 *  data word to write to the specified register should be stored in the 
 *  least significant 16-bits of this 32-bit data word. The parameter is 
 *  a CEI_UINT32 for backwards compatability with other ARINC APIs.
 *  
 *----------------------------------------------------------------------------*
 *                                              see ipav_api.h
 *----------------------------------------------------------------------------*/
#define RX1_SETUP               12
#define RX2_SETUP               13     
#define RX3_SETUP               14     
#define RX4_SETUP               15     
#define RX5_SETUP               16     
#define RX6_SETUP               17
#define RX7_SETUP               18      
#define RX8_SETUP               19    

/*----------------------------------------------------------------------------*
 *                                              see ip429.h
 *----------------------------------------------------------------------------*/
#define IPAV_RXCFG_SPEED        0x01
#define IPAV_RXCFG_PARITY       0x02
#define IPAV_RXCFG_WRAP         0x04
#define IPAV_RXCFG_MODE         0x08
#define IPAV_RXCFG_SDI          0x10

/*----------------------------------------------------------------------------*
 *                                              see ipav_api.h
 *----------------------------------------------------------------------------*/
#define TX1_SETUP               32
#define TX2_SETUP               33 
#define TX3_SETUP               34 
#define TX4_SETUP               35 
#define TX5_SETUP               36 
#define TX6_SETUP               37 
#define TX7_SETUP               38 
#define TX8_SETUP               39     

/*----------------------------------------------------------------------------*
 *                                              see ip429.h
 *----------------------------------------------------------------------------*/
#define IPAV_TXCFG_SPEED        0x01
#define IPAV_TXCFG_PARITY       0x02
#define IPAV_TXCFG_EVEN         0x04
#define IPAV_TXCFG_BCLO         0x08
#define IPAV_TXCFG_BCHI         0x10
#define IPAV_TXCFG_GAP          0x20
#define IPAV_TXCFG_PARAM        0x40


/*----------------------------------------------------------------------------*
 *  Utility function codes for AR_SET_CONFIG chip configuration.  Definitions
 *   are applicable to the CEI-100/200/220/420/520/620/820/710 & IP429HD boards.
 *----------------------------------------------------------------------------*/
#define ARU_XMIT_RATE           1 /* AR_HIGH (100Kb) or AR_LOW (12.5Kb) all chan */
#define ARU_RECV_RATE           2 /* AR_HIGH or AR_LOW for 100 Kb or 12.5 Kb, all chan */
#define ARU_PARITY              3 /* Xmit parity all channels AR_EVEN, AR_ODD, AR_OFF(220 only) */
#define ARU_INTERNAL_WRAP       4 /* Internal wrap all channels AR_ON/AR_OFF  */
#define ARU_WORD_LENGTH         5 /* Transmit bit length AR_SHORT or AR_LONG  */

#define ARU_R1_SDI_FILTER       6 /* SDI receiver filtering AR_ON or AR_OFF   */
#define ARU_R2_SDI_FILTER       7
#define ARU_R3_SDI_FILTER       8
#define ARU_R4_SDI_FILTER       9
#define ARU_R5_SDI_FILTER       10
#define ARU_R6_SDI_FILTER       11
#define ARU_R7_SDI_FILTER       12
#define ARU_R8_SDI_FILTER       13

#define ARU_R1_SDI_VALUE        14 /* SDI receiver filter value               */
#define ARU_R2_SDI_VALUE        15
#define ARU_R3_SDI_VALUE        16
#define ARU_R4_SDI_VALUE        17
#define ARU_R5_SDI_VALUE        18
#define ARU_R6_SDI_VALUE        19
#define ARU_R7_SDI_VALUE        20
#define ARU_R8_SDI_VALUE        21

#define ARU_X1_RATE             22 /* AR_HIGH or AR_LOW for 100 Kb or 12.5 Kb, ch 1 */
#define ARU_X2_RATE             23
#define ARU_X3_RATE             24
#define ARU_X4_RATE             25
#define ARU_R12_RATE            26 /* AR_HIGH or AR_LOW for 100 Kb or 12.5 Kb, ch 1-2 */
#define ARU_R34_RATE            27
#define ARU_R56_RATE            28
#define ARU_R78_RATE            29

#define ARU_R12X1_PARITY        30 /* AR_EVEN or AR_ODD for transmit parity   */
#define ARU_R34X2_PARITY        31
#define ARU_R56X3_PARITY        32
#define ARU_R78X4_PARITY        33

#define ARU_XMIT                34 /* AR_HIGH or AR_LOW for all transmitters  */
#define ARU_RECV                35 /* AR_HIGH or AR_LOW for all transmitters  */


/*----------------------------------------------------------------------------*
 *  Enhanced API ar_set_preload_config constants
 *----------------------------------------------------------------------------*/
#define ARU_CONCURRENCY_MODE     1
#define ARU_INSTALL_INT_HANDLER  2
#define AR_CONC_NONE             0
#define AR_CONC_MULTITHRD        1
#define AR_CONC_MULTIPROC        2


/*----------------------------------------------------------------------------*
 *  PA-100 support ONLY.
 *----------------------------------------------------------------------------*/
#define ARU_BAUD_RATE         13
#define ARU_N_X_CHANNELS      37
#define ARU_XFER_MODE         34
#define AR_HALF_DUPLEX        0
#define AR_FULL_DUPLEX        1


/*----------------------------------------------------------------------------*
 *  CEI-220/420/520/620/820 support ONLY.  If these parameters are used with 
 *   a CEI-100/200 board, they will return an error and perform no operation.
 *----------------------------------------------------------------------------*/
#define ARU_RX_CH01_BIT_RATE   500   /* Configure receive ch one bit rate    */
#define ARU_RX_CH02_BIT_RATE   501   /* AR_HIGH or AR_LOW select 100Kbaud    */
#define ARU_RX_CH03_BIT_RATE   502   /*  and 12.5 Kbaud respectively.        */
#define ARU_RX_CH04_BIT_RATE   503  
#define ARU_RX_CH05_BIT_RATE   504
#define ARU_RX_CH06_BIT_RATE   505
#define ARU_RX_CH07_BIT_RATE   506
#define ARU_RX_CH08_BIT_RATE   507
#define ARU_RX_CH09_BIT_RATE   508
#define ARU_RX_CH10_BIT_RATE   509
#define ARU_RX_CH11_BIT_RATE   510
#define ARU_RX_CH12_BIT_RATE   511
#define ARU_RX_CH13_BIT_RATE   512
#define ARU_RX_CH14_BIT_RATE   513
#define ARU_RX_CH15_BIT_RATE   514
#define ARU_RX_CH16_BIT_RATE   515
#define ARU_RX_CH17_BIT_RATE   516
#define ARU_RX_CH18_BIT_RATE   517
#define ARU_RX_CH19_BIT_RATE   518
#define ARU_RX_CH20_BIT_RATE   519
#define ARU_RX_CH21_BIT_RATE   520
#define ARU_RX_CH22_BIT_RATE   521
#define ARU_RX_CH23_BIT_RATE   522
#define ARU_RX_CH24_BIT_RATE   523
#define ARU_RX_CH25_BIT_RATE   524
#define ARU_RX_CH26_BIT_RATE   525
#define ARU_RX_CH27_BIT_RATE   526
#define ARU_RX_CH28_BIT_RATE   527
#define ARU_RX_CH29_BIT_RATE   528
#define ARU_RX_CH30_BIT_RATE   529
#define ARU_RX_CH31_BIT_RATE   530
#define ARU_RX_CH32_BIT_RATE   531

#define ARU_TX_CH01_BIT_RATE   700   /* Configure transmit ch one bit rate   */
#define ARU_TX_CH02_BIT_RATE   701   /* AR_HIGH or AR_LOW select 100Kbaud    */
#define ARU_TX_CH03_BIT_RATE   702   /*  and 12.5 Kbaud respectively.        */
#define ARU_TX_CH04_BIT_RATE   703
#define ARU_TX_CH05_BIT_RATE   704
#define ARU_TX_CH06_BIT_RATE   705
#define ARU_TX_CH07_BIT_RATE   706
#define ARU_TX_CH08_BIT_RATE   707
#define ARU_TX_CH09_BIT_RATE   708
#define ARU_TX_CH10_BIT_RATE   709
#define ARU_TX_CH11_BIT_RATE   710
#define ARU_TX_CH12_BIT_RATE   711
#define ARU_TX_CH13_BIT_RATE   712
#define ARU_TX_CH14_BIT_RATE   713
#define ARU_TX_CH15_BIT_RATE   714
#define ARU_TX_CH16_BIT_RATE   715
#define ARU_TX_CH17_BIT_RATE   716
#define ARU_TX_CH18_BIT_RATE   717
#define ARU_TX_CH19_BIT_RATE   718
#define ARU_TX_CH20_BIT_RATE   719
#define ARU_TX_CH21_BIT_RATE   720
#define ARU_TX_CH22_BIT_RATE   721
#define ARU_TX_CH23_BIT_RATE   722
#define ARU_TX_CH24_BIT_RATE   723
#define ARU_TX_CH25_BIT_RATE   724
#define ARU_TX_CH26_BIT_RATE   725
#define ARU_TX_CH27_BIT_RATE   726
#define ARU_TX_CH28_BIT_RATE   727
#define ARU_TX_CH29_BIT_RATE   728
#define ARU_TX_CH30_BIT_RATE   729
#define ARU_TX_CH31_BIT_RATE   730
#define ARU_TX_CH32_BIT_RATE   731


// The CEI-220/420/520/620/820 receiver supports either no parity (AR_OFF),
//  or parity checking.
// Parity checking (AR_ON or AR_ODD) checks the parity of the received word.
// If the parity detected is ODD the most significant bit of the ARINC word
//   is CLEAR.  If EVEN parity is detected, the MSB is SET.  In either case
//   the received ARINC word is placed into the buffer.
#define ARU_RX_CH01_PARITY     900   /* Configure receive ch one parity      */
#define ARU_RX_CH02_PARITY     901   /* AR_ON/AR_ODD or AR_OFF               */
#define ARU_RX_CH03_PARITY     902   /* Defaults to AR_ODD mode (odd parity) */
#define ARU_RX_CH04_PARITY     903   /*  which is equivalent to AR_ON.       */
#define ARU_RX_CH05_PARITY     904   /* If parity enabled, MSB of received   */
#define ARU_RX_CH06_PARITY     905   /*  word is 1 if odd parity detected,   */
#define ARU_RX_CH07_PARITY     906   /*  and 0 if even parity was detected.  */
#define ARU_RX_CH08_PARITY     907   /* AR_ON and AR_ODD are equivalent.     */
#define ARU_RX_CH09_PARITY     908
#define ARU_RX_CH10_PARITY     909
#define ARU_RX_CH11_PARITY     910
#define ARU_RX_CH12_PARITY     911
#define ARU_RX_CH13_PARITY     912
#define ARU_RX_CH14_PARITY     913
#define ARU_RX_CH15_PARITY     914
#define ARU_RX_CH16_PARITY     915
#define ARU_RX_CH17_PARITY     916
#define ARU_RX_CH18_PARITY     917
#define ARU_RX_CH19_PARITY     918
#define ARU_RX_CH20_PARITY     919
#define ARU_RX_CH21_PARITY     920
#define ARU_RX_CH22_PARITY     921
#define ARU_RX_CH23_PARITY     922
#define ARU_RX_CH24_PARITY     923
#define ARU_RX_CH25_PARITY     924
#define ARU_RX_CH26_PARITY     925
#define ARU_RX_CH27_PARITY     926
#define ARU_RX_CH28_PARITY     927
#define ARU_RX_CH29_PARITY     928
#define ARU_RX_CH30_PARITY     929
#define ARU_RX_CH31_PARITY     930
#define ARU_RX_CH32_PARITY     931

#define ARU_TX_CH01_PARITY     1100  /* Configure transmit ch one parity    */
#define ARU_TX_CH02_PARITY     1101  /* AR_EVEN or AR_ODD or AR_OFF         */
#define ARU_TX_CH03_PARITY     1102  /* Defaults to AR_ODD mode (odd parity)*/
#define ARU_TX_CH04_PARITY     1103  /*  which is equivalent to AR_OFF.     */
#define ARU_TX_CH05_PARITY     1104
#define ARU_TX_CH06_PARITY     1105
#define ARU_TX_CH07_PARITY     1106
#define ARU_TX_CH08_PARITY     1107
#define ARU_TX_CH09_PARITY     1108
#define ARU_TX_CH10_PARITY     1109
#define ARU_TX_CH11_PARITY     1110
#define ARU_TX_CH12_PARITY     1111
#define ARU_TX_CH13_PARITY     1112
#define ARU_TX_CH14_PARITY     1113
#define ARU_TX_CH15_PARITY     1114
#define ARU_TX_CH16_PARITY     1115
#define ARU_TX_CH17_PARITY     1116
#define ARU_TX_CH18_PARITY     1117
#define ARU_TX_CH19_PARITY     1118
#define ARU_TX_CH20_PARITY     1119
#define ARU_TX_CH21_PARITY     1120
#define ARU_TX_CH22_PARITY     1121
#define ARU_TX_CH23_PARITY     1122
#define ARU_TX_CH24_PARITY     1123
#define ARU_TX_CH25_PARITY     1124
#define ARU_TX_CH26_PARITY     1125
#define ARU_TX_CH27_PARITY     1126
#define ARU_TX_CH28_PARITY     1127
#define ARU_TX_CH29_PARITY     1128
#define ARU_TX_CH30_PARITY     1129
#define ARU_TX_CH31_PARITY     1130
#define ARU_TX_CH32_PARITY     1131

#define ARU_RX_CH01_SDI_FILTER 1300  /* Enable SDI prefilter ch 1           */
#define ARU_RX_CH02_SDI_FILTER 1301  /* AR_ON or AR_OFF                     */
#define ARU_RX_CH03_SDI_FILTER 1302
#define ARU_RX_CH04_SDI_FILTER 1303
#define ARU_RX_CH05_SDI_FILTER 1304
#define ARU_RX_CH06_SDI_FILTER 1305
#define ARU_RX_CH07_SDI_FILTER 1306
#define ARU_RX_CH08_SDI_FILTER 1307
#define ARU_RX_CH09_SDI_FILTER 1308
#define ARU_RX_CH10_SDI_FILTER 1309
#define ARU_RX_CH11_SDI_FILTER 1310
#define ARU_RX_CH12_SDI_FILTER 1311
#define ARU_RX_CH13_SDI_FILTER 1312
#define ARU_RX_CH14_SDI_FILTER 1313
#define ARU_RX_CH15_SDI_FILTER 1314
#define ARU_RX_CH16_SDI_FILTER 1315
#define ARU_RX_CH17_SDI_FILTER 1316
#define ARU_RX_CH18_SDI_FILTER 1317
#define ARU_RX_CH19_SDI_FILTER 1318
#define ARU_RX_CH20_SDI_FILTER 1319
#define ARU_RX_CH21_SDI_FILTER 1320
#define ARU_RX_CH22_SDI_FILTER 1321
#define ARU_RX_CH23_SDI_FILTER 1322
#define ARU_RX_CH24_SDI_FILTER 1323
#define ARU_RX_CH25_SDI_FILTER 1324
#define ARU_RX_CH26_SDI_FILTER 1325
#define ARU_RX_CH27_SDI_FILTER 1326
#define ARU_RX_CH28_SDI_FILTER 1327
#define ARU_RX_CH29_SDI_FILTER 1328
#define ARU_RX_CH30_SDI_FILTER 1329
#define ARU_RX_CH31_SDI_FILTER 1330
#define ARU_RX_CH32_SDI_FILTER 1331


#define ARU_RX_CH01_SDI_VALUE  1500  /* SDI prefilter value for ch 1 (0-3)  */
#define ARU_RX_CH02_SDI_VALUE  1501
#define ARU_RX_CH03_SDI_VALUE  1502
#define ARU_RX_CH04_SDI_VALUE  1503
#define ARU_RX_CH05_SDI_VALUE  1504
#define ARU_RX_CH06_SDI_VALUE  1505
#define ARU_RX_CH07_SDI_VALUE  1506
#define ARU_RX_CH08_SDI_VALUE  1507
#define ARU_RX_CH09_SDI_VALUE  1508
#define ARU_RX_CH10_SDI_VALUE  1509
#define ARU_RX_CH11_SDI_VALUE  1510
#define ARU_RX_CH12_SDI_VALUE  1511
#define ARU_RX_CH13_SDI_VALUE  1512
#define ARU_RX_CH14_SDI_VALUE  1513
#define ARU_RX_CH15_SDI_VALUE  1514
#define ARU_RX_CH16_SDI_VALUE  1515
#define ARU_RX_CH17_SDI_VALUE  1516
#define ARU_RX_CH18_SDI_VALUE  1517
#define ARU_RX_CH19_SDI_VALUE  1518
#define ARU_RX_CH20_SDI_VALUE  1519
#define ARU_RX_CH21_SDI_VALUE  1520
#define ARU_RX_CH22_SDI_VALUE  1521
#define ARU_RX_CH23_SDI_VALUE  1522
#define ARU_RX_CH24_SDI_VALUE  1523
#define ARU_RX_CH25_SDI_VALUE  1524
#define ARU_RX_CH26_SDI_VALUE  1525
#define ARU_RX_CH27_SDI_VALUE  1526
#define ARU_RX_CH28_SDI_VALUE  1527
#define ARU_RX_CH29_SDI_VALUE  1528
#define ARU_RX_CH30_SDI_VALUE  1529
#define ARU_RX_CH31_SDI_VALUE  1530
#define ARU_RX_CH32_SDI_VALUE  1531

#define ARU_TX_CH01_SHUT_OFF   1700  /* Shut off the ARINC transmitter       */
#define ARU_TX_CH02_SHUT_OFF   1701  /* AR_ON or AR_OFF                      */
#define ARU_TX_CH03_SHUT_OFF   1702  /* AR_ON turns the transmitter OFF      */
#define ARU_TX_CH04_SHUT_OFF   1703
#define ARU_TX_CH05_SHUT_OFF   1704
#define ARU_TX_CH06_SHUT_OFF   1705
#define ARU_TX_CH07_SHUT_OFF   1706
#define ARU_TX_CH08_SHUT_OFF   1707
#define ARU_TX_CH09_SHUT_OFF   1708
#define ARU_TX_CH10_SHUT_OFF   1709
#define ARU_TX_CH11_SHUT_OFF   1710
#define ARU_TX_CH12_SHUT_OFF   1711
#define ARU_TX_CH13_SHUT_OFF   1712
#define ARU_TX_CH14_SHUT_OFF   1713
#define ARU_TX_CH15_SHUT_OFF   1714
#define ARU_TX_CH16_SHUT_OFF   1715
#define ARU_TX_CH17_SHUT_OFF   1716  
#define ARU_TX_CH18_SHUT_OFF   1717  
#define ARU_TX_CH19_SHUT_OFF   1718  
#define ARU_TX_CH20_SHUT_OFF   1719
#define ARU_TX_CH21_SHUT_OFF   1720
#define ARU_TX_CH22_SHUT_OFF   1721
#define ARU_TX_CH23_SHUT_OFF   1722
#define ARU_TX_CH24_SHUT_OFF   1723
#define ARU_TX_CH25_SHUT_OFF   1724
#define ARU_TX_CH26_SHUT_OFF   1725
#define ARU_TX_CH27_SHUT_OFF   1726
#define ARU_TX_CH28_SHUT_OFF   1727
#define ARU_TX_CH29_SHUT_OFF   1728
#define ARU_TX_CH30_SHUT_OFF   1729
#define ARU_TX_CH31_SHUT_OFF   1730
#define ARU_TX_CH32_SHUT_OFF   1731

#define ARU_CH01_SCHEDULED_MODE  1900 /* Set single channel to Scheduled mode */
#define ARU_CH02_SCHEDULED_MODE  1901 /* Use in call to ar_msg_control() to   */
#define ARU_CH03_SCHEDULED_MODE  1902 /* set channel to Scheduled xmit mode   */
#define ARU_CH04_SCHEDULED_MODE  1903
#define ARU_CH05_SCHEDULED_MODE  1904
#define ARU_CH06_SCHEDULED_MODE  1905
#define ARU_CH07_SCHEDULED_MODE  1906
#define ARU_CH08_SCHEDULED_MODE  1907
#define ARU_CH09_SCHEDULED_MODE  1908
#define ARU_CH10_SCHEDULED_MODE  1909
#define ARU_CH11_SCHEDULED_MODE  1910
#define ARU_CH12_SCHEDULED_MODE  1911
#define ARU_CH13_SCHEDULED_MODE  1912
#define ARU_CH14_SCHEDULED_MODE  1913
#define ARU_CH15_SCHEDULED_MODE  1914
#define ARU_CH16_SCHEDULED_MODE  1915
#define ARU_CH17_SCHEDULED_MODE  1916 
#define ARU_CH18_SCHEDULED_MODE  1917 
#define ARU_CH19_SCHEDULED_MODE  1918 
#define ARU_CH20_SCHEDULED_MODE  1919
#define ARU_CH21_SCHEDULED_MODE  1920
#define ARU_CH22_SCHEDULED_MODE  1921
#define ARU_CH23_SCHEDULED_MODE  1922
#define ARU_CH24_SCHEDULED_MODE  1923
#define ARU_CH25_SCHEDULED_MODE  1924
#define ARU_CH26_SCHEDULED_MODE  1925
#define ARU_CH27_SCHEDULED_MODE  1926
#define ARU_CH28_SCHEDULED_MODE  1927
#define ARU_CH29_SCHEDULED_MODE  1928
#define ARU_CH30_SCHEDULED_MODE  1929
#define ARU_CH31_SCHEDULED_MODE  1930
#define ARU_CH32_SCHEDULED_MODE  1931

#define ARU_CH01_BURST_MODE    2100  /* Set single channel to Burst mode      */
#define ARU_CH02_BURST_MODE    2101  /* Use in call to ar_msg_control() to    */
#define ARU_CH03_BURST_MODE    2102  /*  set channel to Burst transmit mode.  */
#define ARU_CH04_BURST_MODE    2103
#define ARU_CH05_BURST_MODE    2104
#define ARU_CH06_BURST_MODE    2105
#define ARU_CH07_BURST_MODE    2106
#define ARU_CH08_BURST_MODE    2107
#define ARU_CH09_BURST_MODE    2108
#define ARU_CH10_BURST_MODE    2109
#define ARU_CH11_BURST_MODE    2110
#define ARU_CH12_BURST_MODE    2111
#define ARU_CH13_BURST_MODE    2112
#define ARU_CH14_BURST_MODE    2113
#define ARU_CH15_BURST_MODE    2114
#define ARU_CH16_BURST_MODE    2115
#define ARU_CH17_BURST_MODE    2116 
#define ARU_CH18_BURST_MODE    2117 
#define ARU_CH19_BURST_MODE    2118 
#define ARU_CH20_BURST_MODE    2119
#define ARU_CH21_BURST_MODE    2120
#define ARU_CH22_BURST_MODE    2121
#define ARU_CH23_BURST_MODE    2122
#define ARU_CH24_BURST_MODE    2123
#define ARU_CH25_BURST_MODE    2124
#define ARU_CH26_BURST_MODE    2125
#define ARU_CH27_BURST_MODE    2126
#define ARU_CH28_BURST_MODE    2127
#define ARU_CH29_BURST_MODE    2128
#define ARU_CH30_BURST_MODE    2129
#define ARU_CH31_BURST_MODE    2130
#define ARU_CH32_BURST_MODE    2131


#define ARU_CH01_ENABLE_TIMETAG  2300  /* Enable Time Tags for a single chan  */
#define ARU_CH02_ENABLE_TIMETAG  2301  /* Use with ar_timetag_control().      */
#define ARU_CH03_ENABLE_TIMETAG  2302
#define ARU_CH04_ENABLE_TIMETAG  2303
#define ARU_CH05_ENABLE_TIMETAG  2304
#define ARU_CH06_ENABLE_TIMETAG  2305
#define ARU_CH07_ENABLE_TIMETAG  2306
#define ARU_CH08_ENABLE_TIMETAG  2307
#define ARU_CH09_ENABLE_TIMETAG  2308
#define ARU_CH10_ENABLE_TIMETAG  2309
#define ARU_CH11_ENABLE_TIMETAG  2310
#define ARU_CH12_ENABLE_TIMETAG  2311
#define ARU_CH13_ENABLE_TIMETAG  2312
#define ARU_CH14_ENABLE_TIMETAG  2313
#define ARU_CH15_ENABLE_TIMETAG  2314
#define ARU_CH16_ENABLE_TIMETAG  2315
#define ARU_CH17_ENABLE_TIMETAG  2316  
#define ARU_CH18_ENABLE_TIMETAG  2317  
#define ARU_CH19_ENABLE_TIMETAG  2318
#define ARU_CH20_ENABLE_TIMETAG  2319
#define ARU_CH21_ENABLE_TIMETAG  2320
#define ARU_CH22_ENABLE_TIMETAG  2321
#define ARU_CH23_ENABLE_TIMETAG  2322
#define ARU_CH24_ENABLE_TIMETAG  2323
#define ARU_CH25_ENABLE_TIMETAG  2324
#define ARU_CH26_ENABLE_TIMETAG  2325
#define ARU_CH27_ENABLE_TIMETAG  2326
#define ARU_CH28_ENABLE_TIMETAG  2327
#define ARU_CH29_ENABLE_TIMETAG  2328
#define ARU_CH30_ENABLE_TIMETAG  2329
#define ARU_CH31_ENABLE_TIMETAG  2330
#define ARU_CH32_ENABLE_TIMETAG  2331

#define ARU_CH01_DISABLE_TIMETAG 2500  /* Disable Time Tags for a single chan */
#define ARU_CH02_DISABLE_TIMETAG 2501  /* Use with ar_timetag_control().      */
#define ARU_CH03_DISABLE_TIMETAG 2502
#define ARU_CH04_DISABLE_TIMETAG 2503
#define ARU_CH05_DISABLE_TIMETAG 2504
#define ARU_CH06_DISABLE_TIMETAG 2505
#define ARU_CH07_DISABLE_TIMETAG 2506
#define ARU_CH08_DISABLE_TIMETAG 2507
#define ARU_CH09_DISABLE_TIMETAG 2508
#define ARU_CH10_DISABLE_TIMETAG 2509
#define ARU_CH11_DISABLE_TIMETAG 2510
#define ARU_CH12_DISABLE_TIMETAG 2511
#define ARU_CH13_DISABLE_TIMETAG 2512
#define ARU_CH14_DISABLE_TIMETAG 2513
#define ARU_CH15_DISABLE_TIMETAG 2514
#define ARU_CH16_DISABLE_TIMETAG 2515   
#define ARU_CH17_DISABLE_TIMETAG 2516  
#define ARU_CH18_DISABLE_TIMETAG 2517  
#define ARU_CH19_DISABLE_TIMETAG 2518
#define ARU_CH20_DISABLE_TIMETAG 2519
#define ARU_CH21_DISABLE_TIMETAG 2520
#define ARU_CH22_DISABLE_TIMETAG 2521
#define ARU_CH23_DISABLE_TIMETAG 2522
#define ARU_CH24_DISABLE_TIMETAG 2523
#define ARU_CH25_DISABLE_TIMETAG 2524
#define ARU_CH26_DISABLE_TIMETAG 2525
#define ARU_CH27_DISABLE_TIMETAG 2526
#define ARU_CH28_DISABLE_TIMETAG 2527
#define ARU_CH29_DISABLE_TIMETAG 2528
#define ARU_CH30_DISABLE_TIMETAG 2529
#define ARU_CH31_DISABLE_TIMETAG 2530
#define ARU_CH32_DISABLE_TIMETAG 2531  


#define ARU_CH01_BUFFERED      2700  /* Set a single channel to Buffered    */
#define ARU_CH02_BUFFERED      2701  /*  Receive Mode.                      */
#define ARU_CH03_BUFFERED      2702  /* Use with ar_set_storage_mode()      */
#define ARU_CH04_BUFFERED      2703
#define ARU_CH05_BUFFERED      2704
#define ARU_CH06_BUFFERED      2705
#define ARU_CH07_BUFFERED      2706
#define ARU_CH08_BUFFERED      2707
#define ARU_CH09_BUFFERED      2708
#define ARU_CH10_BUFFERED      2709
#define ARU_CH11_BUFFERED      2710
#define ARU_CH12_BUFFERED      2711
#define ARU_CH13_BUFFERED      2712
#define ARU_CH14_BUFFERED      2713
#define ARU_CH15_BUFFERED      2714
#define ARU_CH16_BUFFERED      2715
#define ARU_CH17_BUFFERED      2716  
#define ARU_CH18_BUFFERED      2717  
#define ARU_CH19_BUFFERED      2718  
#define ARU_CH20_BUFFERED      2719
#define ARU_CH21_BUFFERED      2720
#define ARU_CH22_BUFFERED      2721
#define ARU_CH23_BUFFERED      2722
#define ARU_CH24_BUFFERED      2723
#define ARU_CH25_BUFFERED      2724
#define ARU_CH26_BUFFERED      2725
#define ARU_CH27_BUFFERED      2726
#define ARU_CH28_BUFFERED      2727
#define ARU_CH29_BUFFERED      2728
#define ARU_CH30_BUFFERED      2729
#define ARU_CH31_BUFFERED      2730
#define ARU_CH32_BUFFERED      2731


#define ARU_CH01_DEDICATED     2900  /* Set a single channel to Dedicated    */
#define ARU_CH02_DEDICATED     2901  /*  Receive Mode.                       */
#define ARU_CH03_DEDICATED     2902  /* Use in call to ar_set_storage_mode() */
#define ARU_CH04_DEDICATED     2903
#define ARU_CH05_DEDICATED     2904
#define ARU_CH06_DEDICATED     2905
#define ARU_CH07_DEDICATED     2906
#define ARU_CH08_DEDICATED     2907
#define ARU_CH09_DEDICATED     2908
#define ARU_CH10_DEDICATED     2909
#define ARU_CH11_DEDICATED     2910
#define ARU_CH12_DEDICATED     2911
#define ARU_CH13_DEDICATED     2912
#define ARU_CH14_DEDICATED     2913
#define ARU_CH15_DEDICATED     2914
#define ARU_CH16_DEDICATED     2915
#define ARU_CH17_DEDICATED     2916  
#define ARU_CH18_DEDICATED     2917  
#define ARU_CH19_DEDICATED     2918  
#define ARU_CH20_DEDICATED     2919
#define ARU_CH21_DEDICATED     2920
#define ARU_CH22_DEDICATED     2921
#define ARU_CH23_DEDICATED     2922
#define ARU_CH24_DEDICATED     2923
#define ARU_CH25_DEDICATED     2924
#define ARU_CH26_DEDICATED     2925
#define ARU_CH27_DEDICATED     2926
#define ARU_CH28_DEDICATED     2927
#define ARU_CH29_DEDICATED     2928
#define ARU_CH30_DEDICATED     2929
#define ARU_CH31_DEDICATED     2930
#define ARU_CH32_DEDICATED     2931

#define ARU_CH01_MERGED        3100  /* Set a single channel to Merged        */
#define ARU_CH02_MERGED        3101  /*  Receive Mode.                        */
#define ARU_CH03_MERGED        3102  /* Use in call to ar_set_storage_mode()  */
#define ARU_CH04_MERGED        3103  /* Beware of limitations in buffer size! */
#define ARU_CH05_MERGED        3104
#define ARU_CH06_MERGED        3105
#define ARU_CH07_MERGED        3106
#define ARU_CH08_MERGED        3107
#define ARU_CH09_MERGED        3108
#define ARU_CH10_MERGED        3109
#define ARU_CH11_MERGED        3110
#define ARU_CH12_MERGED        3111
#define ARU_CH13_MERGED        3112
#define ARU_CH14_MERGED        3113
#define ARU_CH15_MERGED        3114
#define ARU_CH16_MERGED        3115
#define ARU_CH17_MERGED        3116  
#define ARU_CH18_MERGED        3117  
#define ARU_CH19_MERGED        3118  
#define ARU_CH20_MERGED        3119  
#define ARU_CH21_MERGED        3120
#define ARU_CH22_MERGED        3121
#define ARU_CH23_MERGED        3122
#define ARU_CH24_MERGED        3123
#define ARU_CH25_MERGED        3124
#define ARU_CH26_MERGED        3125
#define ARU_CH27_MERGED        3126
#define ARU_CH28_MERGED        3127
#define ARU_CH29_MERGED        3128
#define ARU_CH30_MERGED        3129
#define ARU_CH31_MERGED        3130
#define ARU_CH32_MERGED        3131

#define ARU_TX_CH01_HB_INJ      3300  /* Enable Extra Bit error injection     */
#define ARU_TX_CH02_HB_INJ      3301  /* AR_ON or AR_OFF                      */
#define ARU_TX_CH03_HB_INJ      3302  /* This function when enabled increases */
#define ARU_TX_CH04_HB_INJ      3303  /*  the normal number of bits send per  */
#define ARU_TX_CH05_HB_INJ      3304  /*  label from 32 to 33.                */
#define ARU_TX_CH06_HB_INJ      3305
#define ARU_TX_CH07_HB_INJ      3306
#define ARU_TX_CH08_HB_INJ      3307
#define ARU_TX_CH09_HB_INJ      3308
#define ARU_TX_CH10_HB_INJ      3309
#define ARU_TX_CH11_HB_INJ      3310
#define ARU_TX_CH12_HB_INJ      3311
#define ARU_TX_CH13_HB_INJ      3312
#define ARU_TX_CH14_HB_INJ      3313
#define ARU_TX_CH15_HB_INJ      3314
#define ARU_TX_CH16_HB_INJ      3315
#define ARU_TX_CH17_HB_INJ      3316
#define ARU_TX_CH18_HB_INJ      3317
#define ARU_TX_CH19_HB_INJ      3318
#define ARU_TX_CH20_HB_INJ      3319
#define ARU_TX_CH21_HB_INJ      3320
#define ARU_TX_CH22_HB_INJ      3321
#define ARU_TX_CH23_HB_INJ      3322
#define ARU_TX_CH24_HB_INJ      3323
#define ARU_TX_CH25_HB_INJ      3324
#define ARU_TX_CH26_HB_INJ      3325
#define ARU_TX_CH27_HB_INJ      3326
#define ARU_TX_CH28_HB_INJ      3327
#define ARU_TX_CH29_HB_INJ      3328
#define ARU_TX_CH30_HB_INJ      3329
#define ARU_TX_CH31_HB_INJ      3330
#define ARU_TX_CH32_HB_INJ      3331


#define ARU_TX_CH01_LB_INJ      3500  /* Enable Too Few Bits error injection  */
#define ARU_TX_CH02_LB_INJ      3501  /* AR_ON or AR_OFF                      */
#define ARU_TX_CH03_LB_INJ      3502  /* This function when enabled reduces   */
#define ARU_TX_CH04_LB_INJ      3503  /*  the normal number of bits send per  */
#define ARU_TX_CH05_LB_INJ      3504  /*  label from 32 to 31.                */
#define ARU_TX_CH06_LB_INJ      3505
#define ARU_TX_CH07_LB_INJ      3506
#define ARU_TX_CH08_LB_INJ      3507
#define ARU_TX_CH09_LB_INJ      3508
#define ARU_TX_CH10_LB_INJ      3509
#define ARU_TX_CH11_LB_INJ      3510
#define ARU_TX_CH12_LB_INJ      3511
#define ARU_TX_CH13_LB_INJ      3512
#define ARU_TX_CH14_LB_INJ      3513
#define ARU_TX_CH15_LB_INJ      3514
#define ARU_TX_CH16_LB_INJ      3515
#define ARU_TX_CH17_LB_INJ      3516
#define ARU_TX_CH18_LB_INJ      3517
#define ARU_TX_CH19_LB_INJ      3518
#define ARU_TX_CH20_LB_INJ      3519
#define ARU_TX_CH21_LB_INJ      3520
#define ARU_TX_CH22_LB_INJ      3521
#define ARU_TX_CH23_LB_INJ      3522
#define ARU_TX_CH24_LB_INJ      3523
#define ARU_TX_CH25_LB_INJ      3524
#define ARU_TX_CH26_LB_INJ      3525
#define ARU_TX_CH27_LB_INJ      3526
#define ARU_TX_CH28_LB_INJ      3527
#define ARU_TX_CH29_LB_INJ      3528
#define ARU_TX_CH30_LB_INJ      3529
#define ARU_TX_CH31_LB_INJ      3530
#define ARU_TX_CH32_LB_INJ      3531

#define ARU_TX_CH01_GAP_INJ     3700  /* Enable Short Gap error injection     */
#define ARU_TX_CH02_GAP_INJ     3701  /* AR_ON or AR_OFF                      */
#define ARU_TX_CH03_GAP_INJ     3702  /* This function when enabled reduces   */
#define ARU_TX_CH04_GAP_INJ     3703  /*  the normal 4 bit time gap between   */
#define ARU_TX_CH05_GAP_INJ     3704  /*  labels to 2 bit times.              */
#define ARU_TX_CH06_GAP_INJ     3705
#define ARU_TX_CH07_GAP_INJ     3706
#define ARU_TX_CH08_GAP_INJ     3707
#define ARU_TX_CH09_GAP_INJ     3708
#define ARU_TX_CH10_GAP_INJ     3709
#define ARU_TX_CH11_GAP_INJ     3710
#define ARU_TX_CH12_GAP_INJ     3711
#define ARU_TX_CH13_GAP_INJ     3712
#define ARU_TX_CH14_GAP_INJ     3713
#define ARU_TX_CH15_GAP_INJ     3714
#define ARU_TX_CH16_GAP_INJ     3715
#define ARU_TX_CH17_GAP_INJ     3716
#define ARU_TX_CH18_GAP_INJ     3717
#define ARU_TX_CH19_GAP_INJ     3718
#define ARU_TX_CH20_GAP_INJ     3719
#define ARU_TX_CH21_GAP_INJ     3720
#define ARU_TX_CH22_GAP_INJ     3721
#define ARU_TX_CH23_GAP_INJ     3722
#define ARU_TX_CH24_GAP_INJ     3723
#define ARU_TX_CH25_GAP_INJ     3724
#define ARU_TX_CH26_GAP_INJ     3725
#define ARU_TX_CH27_GAP_INJ     3726
#define ARU_TX_CH28_GAP_INJ     3727
#define ARU_TX_CH29_GAP_INJ     3728
#define ARU_TX_CH30_GAP_INJ     3729
#define ARU_TX_CH31_GAP_INJ     3730
#define ARU_TX_CH32_GAP_INJ     3731

#define ARU_DAC_VALUE_01        3900  /* Raw value for DAC #1                 */
#define ARU_DAC_VALUE_02        3901
#define ARU_DAC_VALUE_03        3902
#define ARU_DAC_VALUE_04        3903
#define ARU_DAC_VALUE_05        3904
#define ARU_DAC_VALUE_06        3905
#define ARU_DAC_VALUE_07        3906
#define ARU_DAC_VALUE_08        3907
#define ARU_DAC_VALUE_09        3908
#define ARU_DAC_VALUE_10        3909
#define ARU_DAC_VALUE_11        3910
#define ARU_DAC_VALUE_12        3911
#define ARU_DAC_VALUE_13        3912
#define ARU_DAC_VALUE_14        3913
#define ARU_DAC_VALUE_15        3914
#define ARU_DAC_VALUE_16        3915
#define ARU_DAC_VALUE_17        3916
#define ARU_DAC_VALUE_18        3917
#define ARU_DAC_VALUE_19        3918
#define ARU_DAC_VALUE_20        3919
#define ARU_DAC_VALUE_21        3920
#define ARU_DAC_VALUE_22        3921
#define ARU_DAC_VALUE_23        3922
#define ARU_DAC_VALUE_24        3923
#define ARU_DAC_VALUE_25        3924
#define ARU_DAC_VALUE_26        3925
#define ARU_DAC_VALUE_27        3926
#define ARU_DAC_VALUE_28        3927
#define ARU_DAC_VALUE_29        3928
#define ARU_DAC_VALUE_30        3929
#define ARU_DAC_VALUE_31        3930
#define ARU_DAC_VALUE_32        3931

// To enable parametric variation of the output voltage levels of a CEI-520/620
//  call ar_set_config() with the following as "item" and either AR_ON or AR_OFF
//  to enable or disable parametric outputs.  When not using parametrics specify
//  AR_OFF to insure that the output slew rates are per the ARINC specification.
#define ARU_OUTPUT_LEVEL_ADJ_01 4100  /* Adjustable ARINC output levels       */
#define ARU_OUTPUT_LEVEL_ADJ_02 4101  
#define ARU_OUTPUT_LEVEL_ADJ_03 4102  
#define ARU_OUTPUT_LEVEL_ADJ_04 4103  
#define ARU_OUTPUT_LEVEL_ADJ_05 4104  
#define ARU_OUTPUT_LEVEL_ADJ_06 4105  
#define ARU_OUTPUT_LEVEL_ADJ_07 4106  
#define ARU_OUTPUT_LEVEL_ADJ_08 4107  
#define ARU_OUTPUT_LEVEL_ADJ_09 4108  
#define ARU_OUTPUT_LEVEL_ADJ_10 4109  
#define ARU_OUTPUT_LEVEL_ADJ_11 4110  
#define ARU_OUTPUT_LEVEL_ADJ_12 4111  
#define ARU_OUTPUT_LEVEL_ADJ_13 4112  
#define ARU_OUTPUT_LEVEL_ADJ_14 4113  
#define ARU_OUTPUT_LEVEL_ADJ_15 4114  
#define ARU_OUTPUT_LEVEL_ADJ_16 4115  
#define ARU_OUTPUT_LEVEL_ADJ_17 4116  
#define ARU_OUTPUT_LEVEL_ADJ_18 4117  
#define ARU_OUTPUT_LEVEL_ADJ_19 4118  
#define ARU_OUTPUT_LEVEL_ADJ_20 4119  
#define ARU_OUTPUT_LEVEL_ADJ_21 4120  
#define ARU_OUTPUT_LEVEL_ADJ_22 4121  
#define ARU_OUTPUT_LEVEL_ADJ_23 4122  
#define ARU_OUTPUT_LEVEL_ADJ_24 4123  
#define ARU_OUTPUT_LEVEL_ADJ_25 4124  
#define ARU_OUTPUT_LEVEL_ADJ_26 4125  
#define ARU_OUTPUT_LEVEL_ADJ_27 4126  
#define ARU_OUTPUT_LEVEL_ADJ_28 4127  
#define ARU_OUTPUT_LEVEL_ADJ_29 4128  
#define ARU_OUTPUT_LEVEL_ADJ_30 4129  
#define ARU_OUTPUT_LEVEL_ADJ_31 4130  
#define ARU_OUTPUT_LEVEL_ADJ_32 4131  


/*----------------------------------------------------------------------------*
 *  CEI-520/620/820 support only.  If these parameters are used with a 
 *  CEI-100/200/220/420, they will return an error and perform no operation.
 *
 *  When 'await data' mode is enabled for a particular receiver, calls to
 *  AR_GETWORD or AR_GETWORDT put the calling thread to sleep if no data
 *  word is available in the channel's sequential receive buffer.  The sleeping
 *  thread is awakened when either (a) waiting is cancelled with a call to 
 *  AR_CANCEL_DATA_WAIT, or (b) a word is received on the channel in question.
 *  If no data word is available in the channel's sequential receive buffer
 *  when a sleeping thread is awakened, the call to AR_GETWORD or AR_GETWORDT 
 *  returns ARS_NODATA.  Note that AR_GETWORD and AR_GETWORDT return failure 
 *  if a thread is already waiting to receive a word on the channel in 
 *  question (only one waiting thread per receive channel is supported).
 *
 *  The only routines that should be used to read data on a channel that is 
 *  in 'await data' mode are AR_GETWORD or AR_GETWORDT.  Other read routines 
 *  (AR_GETNEXT, AR_GETNEXTT, AR_GETBLOCK) must not be used unless 'await
 *  data' mode is disabled.  Only ARINC 429 and 561 receivers support 'await
 *  data' mode.
 *
 *  If 'await data' mode is enabled for a particular receiver and a thread is 
 *  currently asleep (in AR_GETWORD or AR_GETWORDT) waiting to receive a word
 *  on the channel in question, then disabling 'await data' mode on that 
 *  channel also cancels the pending wait.
 *
 *  Note that 'await data' mode cannot be enabled unless internal interrupt 
 *  handling has also been enabled.  Refer to AR_SET_PRELOAD_CONFIG 
 *  documentation for a discussion of internal interrupt handling limitations.
 *----------------------------------------------------------------------------*/
#define ARU_RX_CH01_AWAIT_DATA 4300   /* Set receive ch one 'await data' mode */
#define ARU_RX_CH02_AWAIT_DATA 4301   /*  AR_ON enables, AR_OFF disables      */
#define ARU_RX_CH03_AWAIT_DATA 4302   
#define ARU_RX_CH04_AWAIT_DATA 4303
#define ARU_RX_CH05_AWAIT_DATA 4304
#define ARU_RX_CH06_AWAIT_DATA 4305
#define ARU_RX_CH07_AWAIT_DATA 4306
#define ARU_RX_CH08_AWAIT_DATA 4307 
#define ARU_RX_CH09_AWAIT_DATA 4308
#define ARU_RX_CH10_AWAIT_DATA 4309
#define ARU_RX_CH11_AWAIT_DATA 4310
#define ARU_RX_CH12_AWAIT_DATA 4311
#define ARU_RX_CH13_AWAIT_DATA 4312
#define ARU_RX_CH14_AWAIT_DATA 4313
#define ARU_RX_CH15_AWAIT_DATA 4314
#define ARU_RX_CH16_AWAIT_DATA 4315
#define ARU_RX_CH17_AWAIT_DATA 4316
#define ARU_RX_CH18_AWAIT_DATA 4317
#define ARU_RX_CH19_AWAIT_DATA 4318
#define ARU_RX_CH20_AWAIT_DATA 4319
#define ARU_RX_CH21_AWAIT_DATA 4320
#define ARU_RX_CH22_AWAIT_DATA 4321
#define ARU_RX_CH23_AWAIT_DATA 4322
#define ARU_RX_CH24_AWAIT_DATA 4323
#define ARU_RX_CH25_AWAIT_DATA 4324
#define ARU_RX_CH26_AWAIT_DATA 4325
#define ARU_RX_CH27_AWAIT_DATA 4326
#define ARU_RX_CH28_AWAIT_DATA 4327
#define ARU_RX_CH29_AWAIT_DATA 4328
#define ARU_RX_CH30_AWAIT_DATA 4329
#define ARU_RX_CH31_AWAIT_DATA 4330
#define ARU_RX_CH32_AWAIT_DATA 4331


/*----------------------------------------------------------------------------*
 *  CEI-520/620/820 support only.  If these parameters are used with a 
 *  CEI-100/200/220/420, they will return an error and perform no operation.
 *
 *  Used in conjunction with AR_GET_CONFIG to determine if there is currently
 *  a pending 'await data' mode wait operation on the given receiver.
 *----------------------------------------------------------------------------*/
#define ARU_RX_CH01_WAIT_PENDING 4500 /* AR_TRUE if pending 'await mode' wait */
#define ARU_RX_CH02_WAIT_PENDING 4501 /* operation, AR_FALSE otherwise        */
#define ARU_RX_CH03_WAIT_PENDING 4502
#define ARU_RX_CH04_WAIT_PENDING 4503
#define ARU_RX_CH05_WAIT_PENDING 4504
#define ARU_RX_CH06_WAIT_PENDING 4505
#define ARU_RX_CH07_WAIT_PENDING 4506
#define ARU_RX_CH08_WAIT_PENDING 4507
#define ARU_RX_CH09_WAIT_PENDING 4508
#define ARU_RX_CH10_WAIT_PENDING 4509
#define ARU_RX_CH11_WAIT_PENDING 4510
#define ARU_RX_CH12_WAIT_PENDING 4511
#define ARU_RX_CH13_WAIT_PENDING 4512
#define ARU_RX_CH14_WAIT_PENDING 4513
#define ARU_RX_CH15_WAIT_PENDING 4514
#define ARU_RX_CH16_WAIT_PENDING 4515
#define ARU_RX_CH17_WAIT_PENDING 4516
#define ARU_RX_CH18_WAIT_PENDING 4517
#define ARU_RX_CH19_WAIT_PENDING 4518
#define ARU_RX_CH20_WAIT_PENDING 4519
#define ARU_RX_CH21_WAIT_PENDING 4520
#define ARU_RX_CH22_WAIT_PENDING 4521
#define ARU_RX_CH23_WAIT_PENDING 4522
#define ARU_RX_CH24_WAIT_PENDING 4523
#define ARU_RX_CH25_WAIT_PENDING 4524
#define ARU_RX_CH26_WAIT_PENDING 4525
#define ARU_RX_CH27_WAIT_PENDING 4526
#define ARU_RX_CH28_WAIT_PENDING 4527
#define ARU_RX_CH29_WAIT_PENDING 4528
#define ARU_RX_CH30_WAIT_PENDING 4529
#define ARU_RX_CH31_WAIT_PENDING 4530
#define ARU_RX_CH32_WAIT_PENDING 4531


/*----------------------------------------------------------------------------*
 *  CEI-220 support; Only available if the PARAMETRIC option was purchased.
 *   Not available on the CEI-100/200 boards.
 *----------------------------------------------------------------------------*/
#define ARU_RX1_4_THRESH_VALUE   160  /* Thresh = 10 * (2.5 - Vdac)           */
#define ARU_RX5_8_THRESH_VALUE   161  /* Vdac = 5 * n / 256 Volts             */
#define ARU_RX9_10_THRESH_VALUE  162  /* Thresh = 0x72 = 2.73 Volts default   */
#define ARU_RX11_12_THRESH_VALUE 163
#define ARU_INPUT_THRESH_VALUE   164  /* Thresh = 20 * n / 256 Volts = 1.56V  */
#define ARU_TRANSMIT_VOLT_VALUE  165  /* Level  = 10 * n / 256 Volts = 9.96V  */
                                      /* See below for generic DAC commands.  */

#define ARU_PARAMETRIC_SUPPORT   170  /* H/W supports variable parametrics?   */
                                      /* This includes Rcvr Thresh Adj, Xmt   */
                                      /*  Output Adj and Extra/Too Few bits   */
                                      /* Returns AR_ON or AR_OFF              */

/*----------------------------------------------------------------------------*
 *  CEI-x20 discrete input/output support
 *----------------------------------------------------------------------------*/                                    
#define ARU_DISCRETE_VALUES      175  /* Read/Write Discrete Inputs/Outputs   */
                                      /* Discretes are active high.           */
#define ARU_DISCRETE_OUTPUTS     176  /* Read/Write discrete output values    */
#define ARU_DISCRETE_INPUTS      177  /* Read discrete input values           */




/*---------------------------------------------------------------------------*
 *  Old 16-channel-max item codes (superceded by codes allowing 200 rx/tx)
 *---------------------------------------------------------------------------*/
#define ARU_OLD_RX_CH01_BIT_RATE     40  /* Configure receiver bit rate      */
#define ARU_OLD_RX_CH16_BIT_RATE     55
#define ARU_OLD_TX_CH01_BIT_RATE     60  /* Configure transmitter bit rate   */
#define ARU_OLD_TX_CH16_BIT_RATE     75
#define ARU_OLD_RX_CH01_PARITY       80  /* Configure receiver parity        */
#define ARU_OLD_RX_CH16_PARITY       95
#define ARU_OLD_TX_CH01_PARITY       100 /* Configure transmitter parity     */
#define ARU_OLD_TX_CH16_PARITY       115
#define ARU_OLD_RX_CH01_SDI_FILTER   120 /* Enable SDI prefilter             */
#define ARU_OLD_RX_CH16_SDI_FILTER   135
#define ARU_OLD_RX_CH01_SDI_VALUE    140 /* SDI prefilter value              */ 
#define ARU_OLD_RX_CH16_SDI_VALUE    155
#define ARU_OLD_TX_CH01_SHUT_OFF     240 /* Shut off the ARINC transmitter   */
#define ARU_OLD_TX_CH16_SHUT_OFF     255
#define ARU_OLD_CH01_SCHEDULED_MODE  260 /* Set channel to scheduled mode    */
#define ARU_OLD_CH16_SCHEDULED_MODE  275
#define ARU_OLD_CH01_BURST_MODE      280 /* Set channel to burst mode        */
#define ARU_OLD_CH16_BURST_MODE      295
#define ARU_OLD_CH01_ENABLE_TIMETAG  300 /* Enable Time Tags for a channel   */
#define ARU_OLD_CH16_ENABLE_TIMETAG  315
#define ARU_OLD_CH01_DISABLE_TIMETAG 320 /* Disable Time Tags for a channel  */
#define ARU_OLD_CH16_DISABLE_TIMETAG 335
#define ARU_OLD_CH01_BUFFERED        340 /* Set a channel to buffered mode   */
#define ARU_OLD_CH16_BUFFERED        355
#define ARU_OLD_CH01_DEDICATED       360 /* Set a channel to dedicated mode  */
#define ARU_OLD_CH16_DEDICATED       375
#define ARU_OLD_CH01_MERGED          380 /* Set a channel to merged mode     */
#define ARU_OLD_CH16_MERGED          395
#define ARU_OLD_TX_CH01_HB_INJ       180 /* Enable Extra Bit error inj       */
#define ARU_OLD_TX_CH16_HB_INJ       195
#define ARU_OLD_TX_CH01_LB_INJ       200 /* Enable Too Few Bits error inj    */ 
#define ARU_OLD_TX_CH16_LB_INJ       215
#define ARU_OLD_TX_CH01_GAP_INJ      220 /* Enable Short Gap error inj       */
#define ARU_OLD_TX_CH16_GAP_INJ      235
#define ARU_OLD_DAC_VALUE_01         400 /* Raw value for DAC #1             */
#define ARU_OLD_DAC_VALUE_16         415 
#define ARU_OLD_OUTPUT_LEVEL_ADJ_01  420 /* Adjustable ARINC output levels   */
#define ARU_OLD_OUTPUT_LEVEL_ADJ_16  435 


/*----------------------------------------------------------------------------*
 *  CEI-x20 Only:
 *  See top of file for the equates for the (VME-AIC)...
 *
 *  These configuration equates are specific to ar_setchparms().
 *  ar_setchparms(board, channel, TransRcv, pAR_CHANNEL_PARMS def);
 *----------------------------------------------------------------------------*/
#define CEIX20_DEFAULT_SEG      0xD000 /* default CEI-220/420 segment address */

/* TransRcv definitions:                                                      */
#define ARU_RECEIVER            0 /* Apply parameter to receiver              */
#define ARU_TRANSMITTER         1 /* Apply parameter to transmitter           */
#define ARU_DISCRETES           2 /* Apply parameter to discretes             */

/* BaudRate definitions:                                                      */
/* For the ARU_RECEIVER or ARU_TRANSMITTER option, use the following speed    */
/*   constants.                                                               */
#define ARU_717_SPEED_384       0x000    /* Receiver subframe = 32 words      */
#define ARU_717_SPEED_768       0x100    /* Receiver subframe = 64 words      */
#define ARU_717_SPEED_1536      0x200    /* Receiver subframe = 128 words     */
#define ARU_717_SPEED_3072      0x300    /* Receiver subframe = 256 words     */
#define ARU_717_SPEED_6144      0x400    /* Receiver subframe = 512 words     */
#define ARU_717_SPEED_12288     0x500    /* Receiver subframe = 1024 words    */
#define ARU_717_SPEED_24576     0x600    /* Receiver subframe = 2048 words    */
#define ARU_717_SPEED_49152     0x700    /* Receiver subframe = 4096 words    */

// CSDB Baud Rates updated for CEI-x20 10-17-2000.V3.21.ajh
#define ARU_CSDB_SPEED_4800     0xCF00   /* CSDB 4800 baud                    */
#define ARU_CSDB_SPEED_9600     0x6700   /* CSDB 9600 baud                    */
#define ARU_CSDB_SPEED_12500    0x4F00   /* CSDB 12500 baud (same as AR_LOW)  */
#define ARU_CSDB_SPEED_19200    0x3300   /* CSDB 19200 baud                   */
#define ARU_CSDB_SPEED_38400    0x1900   /* CSDB 38400 baud                   */
#define ARU_CSDB_SPEED_50000    0x1300   /* CSDB 50000 baud (same as AR_HIGH) */
#define ARU_CSDB_SPEED_76800    0x0C00   /* CSDB 76800 baud                   */

/* Constants for the ARU_RECEIVER Receiver def.AutoSync option                */
#define ARU_717_AUTOSYNC        0x0000   /* Autosync mode                     */
#define ARU_717_RAW             0x2000   /* RAW mode                          */

/* Constants for the Receiver or Transmitter Encoding Mode option             */
#define ARU_717_H_BI_PHASE      0x2000   /* Harvard Bi-Phase Encoding (HBP)   */
#define ARU_717_BIPOLAR         0x1000   /* Bipolar R-T-Z Encoding    (BPRZ)  */

/* Constants for the -717 OperateMode mode definition                         */
#define ARU_FRAME               0x4000   /* Operate in FRAME mode             */

/* Constants for the -429 OperateMode mode definition                         */
#define ARU_SCHEDULED           0x5000   /* Operate in SCHEDULED mode         */

/* Constants for the Parity definition                                        */
#define AR_RAW                  0x2000   /* Parity is turned OFF              */

/* Constants for special channel operations                                   */
#define AR_CEI520J_573_DISABLE  0x0001   /* disable 573 channel on 520/520A-J */


/*----------------------------------------------------------------------------*
 *  Utility function codes
 *----------------------------------------------------------------------------*/
#define ARU_ENABLE_INT          1     /* Enable interrupts to the host        */
#define ARU_DISABLE_INT         2     /* Disable interrupts to the host       */
#define ARU_ENABLE_TIMETAG      1     /* Enable receiver time tagging         */
#define ARU_DISABLE_TIMETAG     0     /* Disable receiver time tagging        */
#define ARU_ALL_LABELS          0x1FF /* Enable/disable ALL label filtering   */
#define ARU_FILTER_OFF          0     /* Filter off--pass specified label(s)  */
#define ARU_FILTER_ON           1     /* Filter on--delete specified label(s) */
#define ARU_BUFFERED            0     /* Set receiver to BUFFERED mode        */
#define ARU_DEDICATED           1     /* Set receiver to DEDICATED mode       */
#define ARU_MERGED              2     /* Set receiver to MERGED mode          */


/*----------------------------------------------------------------------------*
 *  Utility return status codes.  These error codes may be converted into a
 *  printable ASCII string by calling ar_get_error().
 *----------------------------------------------------------------------------*/
#define ARS_FAILURE             -1

#define ARS_NODATA               0    /* No data in receive buffer            */
#define ARS_NORMAL               1    /* Function normal/successful completion*/
#define ARS_GOTDATA              4    /* Receiver returned data               */

#define BTD_ERR_NOWINRT         50    /* WinRT driver not loaded/started      */
#define BTD_ERR_BADREGISTER     51    /* WinRT parameters don't match registry*/
#define BTD_ERR_BADOPEN         52    /* WinRT device open failed             */
#define BTD_NO_SUPPORT          53    /* Bus/Carrier/OS not supported by API  */

#define ARS_INVHARCMD         1002    /* Invalid configuration command        */
#define ARS_INVHARVAL         1003    /* Invalid configuration value          */
#define ARS_XMITOVRFLO        1004    /* Transmit buffer full, no data inserted*/
#define ARS_INVBOARD          1005    /* Invalid board argument               */
#define ARS_NOSYNC            1006    /* Timeout synchronizing with the ARINC */
#define ARS_BADLOAD           1007    /* Could not successfully load firmware */
#define ARS_BRDNOTLOAD        1008    /* Board micro-code load is not running */
                                      /*  or board has not been initialized.  */
#define ARS_SYNCTIMEOUT       1009    /* Error initializing timer             */
#define ARS_SYNCOVERRUN       1010    /* Host CPU can't keep up with data rate*/
#define ARS_BADINIT           1011    /* Can't initialize timer on slave      */
#define ARS_MEMADERR          1012    /* Memory address mapping error         */
#define ARS_MEMWRERR          1013    /* Dual-Port memory write/read-back error*/
#define ARS_INVSTRING         1014    /* Invalid string                       */
#define ARS_INVEQID           1015    /* Invalid equipment ID                 */
#define ARS_CHECKSUM          1016    /* Checksum error loading slave         */
#define ARS_NORESPONSE        1017    /* No response on receiver              */
#define ARS_UNSUPTYPE         1018    /* Translation not available for type   */
#define ARS_INVARG            1019    /* Invalid argument to function         */
#define ARS_INVLABEL          1020    /* Invalid label                        */
#define ARS_DRIVERFAIL        1021    /* Driver load failed                   */
#define ARS_WINRTFAIL         1022    /* WinRT Device Driver Error            */
#define ARS_CHAN_TIMEOUT      1023    /* Time out while commanding CEI-x20 brd*/
#define ARS_NO_HW_SUPRT       1024    /* Function not supported by hardware   */
#define ARS_BAD_DAC_VAL       1025    /* Specified DAC value bad (<0 or >255) */
#define ARS_BAD_FPGA          1026    /* Bad num of transmitters or receivers */
#define ARS_BAD_STATIC        1027    /* Internal memory read/write failure   */
#define ARS_LAST_ERROR        1028    /* Returns addn'l info about prev error */
#define ARS_HW_CONSISTENCY    1029    /* Hardware config is not consistent    */
#define ARS_HW_DETECT         1030    /* Failed to detect CEI-xxx hardware    */
#define ARS_WRAP_DATA_FAIL    1031    /* Wrap Test invalid data failure       */ 
#define ARS_WRAP_RATE_FAIL    1032    /* Wrap Test timetag/baud rate mismatch */
#define ARS_WRAP_FLUSH_FAIL   1035    /* Wrap Test initial flush failure      */
#define ARS_WRAP_DROP_FAIL    1036    /* Wrap Test missing data failure       */
#define ARS_INT_ISR           1037    /* Failed to install/uninstall ISR      */
#define ARS_BOARD_MUTEX       1038    /* Failed to acquire/release board lock */
#define ARS_CONFIG_MUTEX      1039    /* Failed to acquire/release cfg lock   */
#define ARS_NOT_SUPPORTED     1040    /* Requested feature not supported      */
#define ARS_NO_OS_SUPPORT     1041    /* No oper sys support for req feature  */
#define ARS_NOT_EXCLUSIVE     1042    /* Cannot grant exclusive load request  */
#define ARS_NO_INT_SUPPORT    1043    /* Interrupt handling not supported     */
#define ARS_NO_INT_ENABLED    1044    /* Interrupt handling not enabled       */
#define ARS_ISR_ALREADY_INST  1045    /* Another proc already installed ISR   */
#define ARS_THREAD_WAITING    1046    /* Already thread waiting for rcv data  */
#define ARS_WAIT_FAILED       1047    /* Failed to wait for rcv data          */
#define ARS_SIGNAL_FAILED     1048    /* Failed to signal waiting thread      */
#define ARS_CHANNEL_TYPE      1049    /* Invalid channel type for req func    */


#define INTFC_BASE_ERR_CODES        1900
#define INTFC_MAX_ERR_CODES         4

#define INTFC_ERR_NO_LIB            1900
#define INTFC_ERR_NO_FCN            1901
#define INTFC_ERR_NO_INSTALL        1902
#define INTFC_ERR_BOARD_UNSUPORTED  1903


/*---------------------------------------------------------------------------*
 * Added to support the new CEI-520/620/820 functions.
 * This structure defines the channel parameters that are to be set by a call
 * to the new channel-based ar_setchparms() function.
 *---------------------------------------------------------------------------*/
typedef struct ar_channel_parms 
{
   CEI_INT32 BaudRate;     // One of the supported baud rates:
                           //  For ARINC 573/717 channels, baud rate/sub frame size:
                           //     ARU_717_SPEED_384    subframe size = 32 words
                           //     ARU_717_SPEED_768    subframe size = 64 words
                           //     ARU_717_SPEED_1536   subframe size = 128 words
                           //     ARU_717_SPEED_3072   subframe size = 256 words
                           //     ARU_717_SPEED_6144   subframe size = 512 words
                           //     ARU_717_SPEED_12288  subframe size = 1024 words
                           //     ARU_717_SPEED_24576  subframe size = 2048 words
                           //     ARU_717_SPEED_49152  subframe size = 4096 words
                           //  For CSDB channels:
                           //     ARU_CSDB_SPEED_4800    4800 baud
                           //     ARU_CSDB_SPEED_9600    9600 baud
                           //     ARU_CSDB_SPEED_12500  12500 baud (same as AR_LOW)
                           //     ARU_CSDB_SPEED_19200  19200 baud
                           //     ARU_CSDB_SPEED_38400  38400 baud
                           //     ARU_CSDB_SPEED_50000  50000 baud (same as AR_HIGH)
                           //     ARU_CSDB_SPEED_76800  76800 baud
                           //  For all ARINC channels:
                           //     AR_HIGH  Set HIGH speed (100 Kbaud(50KB if CSDB))
                           //     AR_LOW   Set LOW speed (12.5 Kbaud)
                           //     In addition to the standard AR_HIGH and AR_LOW baud rate
                           //     flags, the CEI-820 also allows the user to specify any 
                           //     desired transmit/receive baud rate between 5 KHz and 
                           //     200 KHz. Simply specify the desired baud rate (in Hz). 
                           //     For example, to set the baud rate of a particular 
                           //     transmit/receive channel to 98 KHz, set 'BaudRate' to 
                           //     98000.  
   CEI_INT32 ChannelType;  // Channel type (returned by function call):
                           //  CHAN_NOT_IMPLEMENTED    Channel is not implemented
                           //  CHAN_ARINC429           Channel is an ARINC-429 channel
                           //  CHAN_ARINC717           Channel is an ARINC-573/717 channel
                           //  CHAN_CSDB               Channel is a CSDB channel
                           // When using the CEI22032.dll interface, this value must
                           //  be correctly set by the caller before calling ar_setchparms!
   CEI_INT32 InternalWrap; // Internal wrap-around enable (receiver only)
                           //     AR_WRAP_ON  Enables internal wrap-around mode
                           //     AR_WRAP_OFF Disables internal wrap-around mode
   CEI_INT32 IntCount;     // Enable interrupt every "IntCount" labels rcv/xmt.
                           //     Buffer full/empty interrupts enabled.
                           //     If zero no interrupts are generated.
   CEI_INT32 Parity;       // AR_EVEN, AR_ODD or AR_RAW (same as AR_OFF, disable parity)
   CEI_INT32 AutoSync;     // Autosync or Raw mode        (-717 receiver only)
                           //     ARU_717_AUTOSYNC or ARU_717_RAW
   CEI_INT32 HWEncoding;   // Hardware encoding for ARINC 573/717 channels:
                           //     ARU_717_H_BI_PHASE   = Harvard Bi-Phase Encoding (HBP)
                           //     ARU_717_BIPOLAR      = Bipolar R-t-Z Encoding    (BPRZ)
                           //     ARU_717_H_BI_PHASE | ARU_717_BIPOLAR = both xmitters.V3.26.ajh
                           //     AR_OFF or NULL       = Disable Transmitter
                           //     AR_ON                = Enable Transmitter
   CEI_INT32 Sync1;        // ARINC 573/717 Sync word #1 - Ignored if not 573/717
   CEI_INT32 Sync2;        // ARINC 573/717 Sync word #2
   CEI_INT32 Sync3;        // ARINC 573/717 Sync word #3
   CEI_INT32 Sync4;        // ARINC 573/717 Sync word #4
   CEI_INT32 NumSubFrames; // Number of subframes allocated (ARINC 573/717 channel only)
   CEI_INT32 OperateMode;  // Operational mode to enable:
                           //     ARU_BUFFERED or ARU_MERGED
                           //     or ARU_FRAME   (ARINC 573/717 channel only)
                           //     ARU_DEDICATED  (receive non-enhanced interface only)
                           //     ARU_SCHEDULED  (transmit non-enhanced interface only)
                           // Note that FRAME mode is not supported on 573/717 channels 
                           // running at baud rates above 12288.  All baud rates are 
                           // supported by 573/717 channels running in BUFFERED mode.
   CEI_INT32 TimeTagMode;  // Receiver channel time tag mode:
                           //   ARU_ENABLE_TIMETAG      Enable receiver time tagging
                           //   ARU_DISABLE_TIMETAG     Disable receiver time tagging
                           // Ignored if OperateMode is ARU_FRAME or ARU_MERGED
   CEI_UINT32 Parametrics; // Parametric enable/DAC voltage settings:
                           // If this is a set parameters for ARU_TRANSMITTER:
                           //  Value is 0x01-0xFF; API programs the transmit DAC
                           //    with the supplied value.  If value != 0xFF then the
                           //    PARAMETRIC ENABLE bit is set in the channel control word.
                           //  If specified as 0x00 the DAC value is not changed and the
                           //    channel is programmed to disable variable output voltage.
                           //  If this is a set parameters for ARU_RECEIVER:
                           //   Value is 0xPPNN, where PP is the receiver positive
                           //    threshold value and NN is the receiver negative threshold.
                           //  If specified as 0x0000 the DAC voltages are not modified.
                           // Note that in either case multiple channels might be sharing
                           //  the associated DAC and will all be affected by the change
                           //  in the DAC value(s) supplied by this argument.  See the
                           //  manual for further details.
   CEI_INT32 Reserved2;    // Reserved for future error injection use
} AR_CHANNEL_PARMS, *pAR_CHANNEL_PARMS;



/*---------------------------------------------------------------------------*
 *                   F U N C T I O N   P R O T O T Y P E S
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
   /*------------------------------------------------------------------------*
    *  handle .cpp files
    *------------------------------------------------------------------------*/
   extern "C" {
#endif

#ifdef CEIX20_TARGET_VXWORKS
   #define DLL_EXPORTED      /* */
   #define EXPORT32
#else
#if defined (_Windows)
   #if defined (_WIN32) || defined(__WIN32__) || defined(_CVI_)
      #define DLL_EXPORTED _stdcall
   #else
      #define DLL_EXPORTED FAR PASCAL EXPORT
   #endif
#else
   #define DLL_EXPORTED      /* */
   #define EXPORT32
#endif
#endif


/*---------------------------------------------------------------------------*
 *  ARINC API function prototypes
 *---------------------------------------------------------------------------*/
#ifndef i960   /* Don't include windows stuff when building CEI-220 firmware */


// make sure BusTools/1553-API header hasn't already been included
#ifndef _BUSAPI_H_

// these types are not used by the CEI-x20 API (included to support other products)
#define U16BIT      CEI_UINT16
#define BT_U16BIT   CEI_UINT16
#define BT_U32BIT   CEI_UINT32
#define BT_UINT     unsigned int

#endif


EXPORT32 void DLL_EXPORTED ar_clr_rx_count
(
   CEI_INT16 board,
   CEI_INT16 channel
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_define_frame
( 
   CEI_INT16 board, 
   CEI_INT16 channel, 
   CEI_INT16 rate,
   CEI_UINT16 frame_data[A708_WORDS_PER_FRAME] 
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_define_msg 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_INT16 rate, 
   CEI_UINT16 start,
   CEI_INT32 data
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_define_msg_ex
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_INT16 rate, 
   CEI_UINT32 start,
   CEI_INT32 data
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_execute_bit 
(
   CEI_INT16 cardnum,
   CEI_INT16 test_type
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_get_config
(
   CEI_INT16 board,
   CEI_INT16 item
);

EXPORT32 void DLL_EXPORTED ar_get_errorLV
(
   CEI_CHAR *string,
   CEI_INT16 status
);

#ifdef CEI_LV_BUILD

EXPORT32 void DLL_EXPORTED ar_get_error
(
   CEI_CHAR *string,
   CEI_INT16 status
);

#else

EXPORT32 CEI_CHAR * DLL_EXPORTED ar_get_error
(
   CEI_INT16 status
);

#endif

EXPORT32 CEI_INT16 DLL_EXPORTED ar_get_frame 
( 
   CEI_INT16 board, 
   CEI_INT16 channel, 
   CEI_UINT32 *timetag,
   CEI_UINT16 frame_data[A708_WORDS_PER_FRAME] 
);

EXPORT32 void DLL_EXPORTED ar_get_latest 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_UINT16 label, 
   void *data,
   CEI_CHAR *seq_num
);

EXPORT32 CEI_UINT32 DLL_EXPORTED ar_get_rx_count 
(
   CEI_INT16 board,
   CEI_INT16 channel
);

EXPORT32 CEI_UINT16 DLL_EXPORTED ar_get_timercnt
(
   CEI_INT16 board
);

EXPORT32 CEI_UINT32 DLL_EXPORTED ar_get_timercntl
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_getnext 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   void* dest
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_getnextt 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   void* data, 
   void* timetag
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_getword 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_INT32* dest
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_getwordt 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   void* dest, 
   void* timetag
);

// not defined for the 100/200/x20
EXPORT32 CEI_INT16 DLL_EXPORTED ar_get573 
( 
   CEI_INT16 board,
   CEI_INT16 ip, 
   void* destination 
);


// not defined for the 100/200/x20
EXPORT32 CEI_INT16 DLL_EXPORTED ar_get_serial 
( 
   CEI_INT16 board,
   CEI_INT16 ip, 
   CEI_INT16 channel, 
   CEI_UCHAR *data 
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_go 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_init_dual_port 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_init_slave 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_int_control 
(
   CEI_INT16 board, 
   CEI_INT16 channel, 
   CEI_INT16 flag
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_int_set     
(
   CEI_INT16 board, 
   CEI_INT32 int_num
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_label_filter 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_UINT16 label,
   CEI_INT16 action
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_preload_config
(
   CEI_INT16 cardnum,
   CEI_INT16 item,
   CEI_UINT32 value
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_cancel_data_wait
(
   CEI_INT16 cardnum,
   CEI_INT16 channel
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_loadslv 
(
   CEI_INT16 board,
   CEI_UINT32 base_seg,
   CEI_INT32 base_port,
   CEI_UINT16 ram_size
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_loadslv_init_discrete 
(
   CEI_INT16 board,
   CEI_UINT32 base_seg,
   CEI_INT32 base_port,
   CEI_UINT16 ram_size,
   CEI_UINT32 disc_init
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_modify_msg 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_INT16 msg_num,
   CEI_INT16 rate, 
   CEI_INT32 data
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_msg_control 
(
   CEI_INT16 board, 
   CEI_INT16 control
);

EXPORT32 void DLL_EXPORTED ar_mod_msgctl 
( 
   CEI_INT16 board,
   CEI_INT16 msg_num, 
   CEI_INT16 control 
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_modify_frame 
( 
   CEI_INT16 board,
   CEI_INT16 channel, 
   CEI_INT16 msg_num, 
   CEI_INT16 rate, 
   CEI_UINT16 frame_data[A708_WORDS_PER_FRAME] 
);

// not defined for the 100/200/x20
EXPORT32 CEI_INT16 DLL_EXPORTED ar_put573 
( 
   CEI_INT16 board,
   CEI_INT16 ip, 
   CEI_UINT16 data 
);

// not defined for the 100/200/x20
EXPORT32 CEI_INT16 DLL_EXPORTED ar_put_serial 
( 
   CEI_INT16 board,
   CEI_INT16 ip, 
   CEI_INT16 channel, 
   CEI_UCHAR data 
);

/*----------------------------------------------------------------------------*
 *  Some utility functions have two calling mechanisms:
 *----------------------------------------------------------------------------*/
#if !defined(OLD_UTILITY_STYLE) || defined (_Windows)

EXPORT32 CEI_INT16 DLL_EXPORTED ar_putword 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_INT32 xmit_data
);


EXPORT32 CEI_INT16 DLL_EXPORTED ar_putword2x16 
(
   CEI_INT16 board, 
   CEI_INT16 channel, 
   CEI_INT16 lsw,
   CEI_INT16 msw
);

#else

EXPORT32 CEI_INT16 DLL_EXPORTED ar_putword 
(
   CEI_INT16 board, 
   CEI_INT16 channel,
   CEI_INT16 lsw, 
   CEI_INT16 msw
);

#endif

EXPORT32 void DLL_EXPORTED ar_reformat 
(
   void* lsw, 
   void* msw
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_reset 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_reset_int 
(
   CEI_INT16 board
);

EXPORT32 void DLL_EXPORTED ar_reset_timercnt 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_selftest 
(
   CEI_INT16 board, 
   CEI_INT16 count
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_arinc_config 
( 
   CEI_INT16 board,
   CEI_INT16 channel, 
   CEI_INT16 item, 
   CEI_UINT32 value
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_config 
(
   CEI_INT16 board, 
   CEI_INT16 item,
   CEI_UINT32 value
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_control
(
   CEI_UINT32 board, 
   CEI_UINT32 channel,
   CEI_INT32 item, 
   CEI_UINT32 value
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_dump_dp
(
   CEI_INT16 board, 
   CEI_CHAR *file_name, 
   CEI_CHAR *msg
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_off_board 
( 
   CEI_INT16 board, 
   CEI_INT16 channel, 
   CEI_UINT32 read_address, 
   CEI_UINT32 write_address, 
   CEI_UINT32 tag 
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_raw_mode 
(
   CEI_INT16 board, 
   CEI_INT16 direction,
   CEI_INT16 channel,
   CEI_INT16 control
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_storage_mode 
(
   CEI_INT16 board, 
   CEI_INT16 flag
);

EXPORT32 void DLL_EXPORTED ar_set_timerrate 
(
   CEI_INT16 board, 
   CEI_INT16 rate
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_timetag_control 
(
   CEI_INT16 board, 
   CEI_INT16 flag
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_xmit_sync 
(
   CEI_INT16 board, 
   CEI_INT16 channel
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_close 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_force_version 
(
   CEI_INT16 board, 
   CEI_INT16 number
);

EXPORT32 void DLL_EXPORTED ar_formatarinclabel 
(
   CEI_UINT32 label_data,
   CEI_CHAR *labelstr
);

EXPORT32 void DLL_EXPORTED ar_formatbinarylabel 
(
   CEI_UINT32 label_data,
   CEI_INT32 trunc_label,
   CEI_CHAR *labelstr
);

EXPORT32 CEI_CHAR * DLL_EXPORTED ar_get_boardname
(
   CEI_UINT32 board, 
   CEI_CHAR *boardName
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_get_boardnameLV 
(
   CEI_UINT32 cardnum,
   CEI_CHAR *boardName
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_get_boardtype 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_get_board_status
( 
   CEI_INT16 board,
   CEI_INT16 *system_status,
   CEI_UINT32 *ip_status,
   CEI_UINT32 *error_status
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_get_harris 
(
   CEI_INT16 board, 
   CEI_INT16 item
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_get_label_filter 
(
   CEI_INT16 board, 
   CEI_UINT16 label
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_get_raw_mode 
(
   CEI_INT16 board, 
   CEI_INT16 direction,
   CEI_INT16 channel
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_int_slave 
(
   CEI_INT16 board
);

EXPORT32 CEI_UINT32 DLL_EXPORTED ar_map_dualportram
(
   CEI_INT16 board,
   CEI_UINT32 real_seg,
   CEI_INT16 *err_flag,
   CEI_UINT16 ram_size
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_num_rchans 
(
   CEI_INT16 board
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_num_xchans 
(
   CEI_INT16 board
);

EXPORT32 void DLL_EXPORTED ar_recreate_parity 
(
   CEI_UCHAR *arinc_word
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_set_harris 
(
   CEI_INT16 board, 
   CEI_INT16 item,
   CEI_UINT32 value
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_setuplabelmode 
(
   CEI_INT16 board_num
);
 
EXPORT32 void DLL_EXPORTED ar_unformat 
(
   CEI_INT16 *lsword, 
   CEI_INT16 *msword
);

EXPORT32 void DLL_EXPORTED ar_version 
(
   CEI_CHAR *version
);

EXPORT32 CEI_INT32 DLL_EXPORTED LoadDll
(
   CEI_INT16 SystemType
);

EXPORT32 CEI_INT32 DLL_EXPORTED UnloadDll
(
   void
);

EXPORT32 CEI_INT16 DLL_EXPORTED RegQueryBoardType 
(
   CEI_INT16 *board_type_index
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_getblock
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_INT32 MaxWords,
   CEI_INT32 Offset717,
   CEI_INT32 *ActualCount,
   CEI_INT32 *ARINCdata,
   CEI_INT32 *TimeTag
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_getchparms
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_INT32 TransRcv,
   pAR_CHANNEL_PARMS def
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_getfilter
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_CHAR *Filter
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_putblock
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_INT32 AvailWords,
   CEI_INT32 Offset717,
   CEI_INT32 *ARINCdata,
   CEI_INT32 *ActualCount
);
   
EXPORT32 CEI_INT32 DLL_EXPORTED ar_putfilter
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_CHAR *Filter
);
   
EXPORT32 CEI_INT32 DLL_EXPORTED ar_setchparms
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_INT32 TransRcv,
   pAR_CHANNEL_PARMS def
);

EXPORT32 CEI_INT32 DLL_EXPORTED ar_setinterrupts
(
   CEI_UINT32 cardnum,
   CEI_UINT32 channel,
   CEI_INT32 TransRcv,
   CEI_UINT32 Count,
   CEI_UINT32 Mask
);

EXPORT32 void DLL_EXPORTED ar_sleep
(                    
   CEI_UINT32 sleep_ms
);

// for PA-100 support
EXPORT32 CEI_INT16 DLL_EXPORTED pa_deinit_adapter 
(
   int port
);

// for PA-100 support
EXPORT32 CEI_INT16 DLL_EXPORTED pa_initex_adapter 
(
   int port, 
   int comm_port
);

// for PA-100 support
EXPORT32 CEI_INT16 DLL_EXPORTED pa_timetag_control 
(
   int port, 
   int mode
);


/*---------------------------------------------------------------------------*
 *  The following function definitions are conditionally compiled iff
 *  _Windows is defined.
 *---------------------------------------------------------------------------*/
#if defined(_Windows)

EXPORT32 CEI_INT16 DLL_EXPORTED ar_loadfakedata 
(
   CEI_INT16 board_num
);

EXPORT32 CEI_INT16 DLL_EXPORTED ar_setuplabelmode 
(
   CEI_INT16 board_num
);

#endif
#ifdef __cplusplus
}
#endif

#endif

#endif
