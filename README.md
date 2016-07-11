# HobbyBootloader
This is the beginnings of an RS232 based bootloader for PIC32MX devices, intended to be used with my HobbyOS project.

The Bootloader FSM looks like this:
![FSM](http://i.imgur.com/ipu5VY8.png)

## Bootloader RS232 Protocol

### Get Version
This command will return a version code.  
Send: 'V'  
Response: 4 byte version  

### Get Error
This command will return an error code corresponding to the application verificaiton failure, if any.  
Send: 'R' 
Response: 8 character string (not null terminated).  

### Erase Page
This command will erase a page of memory starting at the address passed.  
Send: 'E' + 4 byte address  
Response: 'e' when erase is complete if a valid address, 'f' otherwise  

### Program Row
This command will program a row starting at the address specified.  
Send: 'P' + 4 byte address  
Response: 'p'  
Then, send 512 bytes + 4 byte CRC32  
Response: 'q' if the CRC failed, 'r' otherwise  

### Boot
This command will stop communication, and attempt to boot the applicaiton.  
Send: 'B'  
Response: 'b'  

If a 500 ms timeout occurs while waiting for data, a 't' will be sent.  
If a command is not recognized, a '?' will be sent.
