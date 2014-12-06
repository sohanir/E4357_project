#include "mbed.h"
#include "EthernetInterface.h"
#include "Websocket.h"
#define SAMPLING_FREQUENCY 5 //CHECK EVERY 2 SECONDS
#define MOTION_COUNT_THRESHOLD 5 // More than 5 readings of motion

Ticker flash;
DigitalOut led(LED1);

//void flashLED(void){led = !led;}
 DigitalIn switch_ip1(p6);
 static unsigned int motion_count = 0;
 int num_readings = 0;
 void send_to_socket(unsigned int);
 
int main() 
{

    //flash.attach(&flashLED, 1.0f);
    EthernetInterface ethernet;
    ethernet.init();    // connect with DHCP
    int ret_val = ethernet.connect();
    
    if (0 == ret_val) {
        printf("IP Address: %s\n", ethernet.getIPAddress());
    } else {
        error("ethernet failed to connect: %d.\n", ret_val);
    }    
    while(1) {

        num_readings =0;
        while(num_readings < 100) {
            if (switch_ip1 == 1) {
            motion_count++;
            } else {
                if (motion_count > 0 ) {
                    motion_count --;
                }
            }
            num_readings++;
        }
        send_to_socket(motion_count);
        wait(SAMPLING_FREQUENCY);
    }
    ethernet.disconnect();
    
}
void send_to_socket(unsigned int motion_count) {

    // view @ http://sockets.mbed.org/demo/viewer
    Websocket ws("ws://sockets.mbed.org/ws/e4357_project/rw");
    ws.connect();
    char str[100];
    UDPSocket sock;
    int n, ret = sock.init();
    printf("Opened UDP socket %d\n\r",ret);
    Endpoint nist;
    nist.set_address("nist-time-server.eoni.com", 37);  
    char outbuf[4];
    time_t timeRes;
    char localtime[40] = {0}, *p = localtime;
    ret = sock.sendTo(nist, "\0", 1);
    printf("sock.sendTo() returned %d\n\r", ret);

    n = sock.receiveFrom(nist, outbuf, sizeof(outbuf));
    printf("sock.receiveFrom() returned %d\n\r", n);
    
    timeRes = ntohl(*((unsigned int*)outbuf)) - 2208988800;
    printf("Received %d bytes from server %s on port %d: %u seconds since 1/01/1900 00:00 GMT\n\r", n, nist.get_address(), nist.get_port(), timeRes);
    p = ctime(&timeRes);
    

    
        // string with a message
        if (motion_count > MOTION_COUNT_THRESHOLD) {       
            sprintf(str, "%d Someone's there! @ %s", motion_count, p);
        } else {
            sprintf(str, "%d No motion at %s",motion_count, p);
        }

        ws.send(str);
        // clear the buffer and wait a sec...
        memset(str, 0, 100);
        wait(0.5f);

        // websocket server should echo whatever we sent it
        if (ws.read(str)) {
            printf("rcv'd: %s\n", str);
        }
    ws.close(); 
}    