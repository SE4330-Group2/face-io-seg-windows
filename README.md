face-io-seg-windows
===================

- These must be done in C and must be console based.
- The prototype is not directly tied to WOW and Emergency.
     Keep it general, use arrays.
- You can use non POSIX functions but
    - Must make every effort to minimize use
    - Must use a Posix-type function if VS has it
    - Wherever a non-Posix function is used, it must be clearly marked with:
    ```
*****  Non-Posix ******
    ```
- Assume I/O seg, PSS in same program - so only direct calls
- Only handle discretes
- Must read and use config file:
     - Although it is a prototype, no hard-coding items
     - The demo's must work if I add multiple 
          TX, RX Discretes to the config file
     - Use arrays to handle the discretes
     - See parallel arrays I used in face_io_api.c  
- For Discrete channels, logical channel #'s from 
     - 1-16 are hardware RX (Input) channels 1-16
     - 17-32 are hardware TX (Output) channels 1-16
- Right now, the attached hardware is set up to use
     - Discrete Input Channel 1
     - Discrete Output channel 1
- See the test bed main in `face_io_api.c` for examples of how to test your sides prior to when we put these together.
- The I/O set side must appropriately drive hardware,
     no matter how many discretes are configured.  For
     example, if we have an XML file with 3 outputs, 
     then if we say to turn 2 off and 1 on, if we move
     the wire for the LED, we will see it appropriately
     light or not.   We could add some more LEDs
- The PSS seg must have a way for the user to 
     - prompt for and read an output channel number 
         and a value to set it to
     - Output the values of the input channels on user request
