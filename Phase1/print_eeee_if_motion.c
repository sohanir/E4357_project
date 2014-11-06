#include "mbed.h"
Ethernet eth;             // Ethernet object
Serial pc(USBTX, USBRX);  // tx, rx for host terminal coms
DigitalIn switch_ip1(p5); //Connected to 
char buf[0xFF];           // create a large buffer to store data
char data1[] = {0xDE, 0xAD, 0xBE, 0xEF};
char data2[] = {0xEE, 0xEE, 0xEE, 0xEE};
int main() {
  pc.printf("Ethernet data read and display\n\r");
  while (1) {
    int size = eth.receive();              // get size of incoming data packet
    if (size > 0) {                        // if packet received
      eth.read(buf, size);                 // read packet to data buffer
      pc.printf("size = %d data = ",size); // print to screen
      for (int i=0;i<size;i++) {           // loop for each data byte
        pc.printf("%d ",buf[i]);         // print data to screen
      }
      pc.printf("\n\r");
    } 
    if (switch_ip1==1) {
        eth.write(data1,0x04); // Write the package
    } else {
        eth.write(data2, 0x04);
    }
    eth.send();           // Send the package
    wait(0.2);            // wait 200 ms
  }
}
