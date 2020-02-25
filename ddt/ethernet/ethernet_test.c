#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<netinet/in.h>
#include<net/if.h>
#include<unistd.h>
#include<arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netdb.h>
#include <linux/if.h>
#define BUFFER_SIZE 4096

#include "test.h"
#include "safe_macros.h"

char *TCID = "ethernets_test";
int TST_TOTAL = 3;

int getgatewayandiface(void)
{
    int route_attribute_len = 0,received_bytes = 0, msg_len = 0; 
    int     sock = -1,msgseq = 0;
    struct  nlmsghdr *nlh, *nlmsg;
    struct  rtmsg *route_entry;
    struct  rtattr *route_attribute;// This struct contain route attributes (route type)
    char    gateway_address[INET_ADDRSTRLEN], interface[IF_NAMESIZE];
    char    msgbuf[BUFFER_SIZE], buffer[BUFFER_SIZE];
    char    *ptr = buffer;
    struct timeval tv;

    if ((sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0) {
        tst_brkm(TCONF,NULL,"socket %d failed for gateway",sock);
        return EXIT_FAILURE;
    }

    memset(msgbuf, 0, sizeof(msgbuf));
    memset(gateway_address, 0, sizeof(gateway_address));
    memset(interface, 0, sizeof(interface));
    memset(buffer, 0, sizeof(buffer));

    /* point the header and the msg structure pointers into the buffer */
    nlmsg = (struct nlmsghdr *)msgbuf;

    /* Fill in the nlmsg header*/
    nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nlmsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
    nlmsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlmsg->nlmsg_seq = msgseq++; // Sequence of the message packet.
    nlmsg->nlmsg_pid = getpid(); // PID of process sending the request.

    //1 Sec Timeout to avoid stall
    tv.tv_sec = 1;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    // send msg 
    if (send(sock, nlmsg, nlmsg->nlmsg_len, 0) < 0) {
        tst_brkm(TCONF,NULL,"send failed for gateway");
       }

    // receive response 
    do
    {
        received_bytes = recv(sock, ptr, sizeof(buffer) - msg_len, 0);
        if (received_bytes < 0) {
            tst_brkm(TCONF,NULL,"Error in recv for gateway");
           
        }

        nlh = (struct nlmsghdr *) ptr;

        // Check if the header is valid 
        if((NLMSG_OK(nlmsg, received_bytes) == 0) ||
           (nlmsg->nlmsg_type == NLMSG_ERROR))
        {
            tst_brkm(TCONF,NULL,"Error in received packet for gateway");
            return EXIT_FAILURE;
        }

        // If we received all data break 
        if (nlh->nlmsg_type == NLMSG_DONE)
            break;
        else {
            ptr += received_bytes;
            msg_len += received_bytes;
        }

        //Break if its not a multi part message
        if ((nlmsg->nlmsg_flags & NLM_F_MULTI) == 0)
            break;
    }
    while ((nlmsg->nlmsg_seq != msgseq) || (nlmsg->nlmsg_pid != getpid()));

    // parse response //
    for ( ; NLMSG_OK(nlh, received_bytes); nlh = NLMSG_NEXT(nlh, received_bytes))
    {
        // Get the route data
        route_entry = (struct rtmsg *) NLMSG_DATA(nlh);

        // We are just interested in main routing table
        if (route_entry->rtm_table != RT_TABLE_MAIN)
            continue; 

        route_attribute = (struct rtattr *) RTM_RTA(route_entry);
        route_attribute_len = RTM_PAYLOAD(nlh);

        // Loop through all attributes 
        for ( ; RTA_OK(route_attribute, route_attribute_len);
              route_attribute = RTA_NEXT(route_attribute, route_attribute_len))
        {
            switch(route_attribute->rta_type) {
            case RTA_OIF:
                if_indextoname(*(int *)RTA_DATA(route_attribute), interface);
                break;
            case RTA_GATEWAY:
                inet_ntop(AF_INET, RTA_DATA(route_attribute),
                          gateway_address, sizeof(gateway_address));
                break;
            default:
                break;
            }
        }

        if ((*gateway_address) && (*interface)) {
            tst_resm(TPASS, "Gateway %s for interface %s\n", gateway_address, interface);
            break;
        }
    }

    close(sock);

    return 0;
}
int getip_addr(char *argv[])
{
	int n;
	struct ifreq ifr;
	n=socket(AF_INET,SOCK_DGRAM,0);
	if(n==-1)
	{
	tst_brkm(TCONF,NULL,"socket failed for ipaddr");
	}
	ifr.ifr_addr.sa_family=AF_INET;
	strncpy(ifr.ifr_name,argv[1],IFNAMSIZ -1);
	ioctl(n,SIOCGIFADDR,&ifr);
	close(n);
	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	tst_resm(TPASS,"ip address  is %s - %s\n", argv[1], inet_ntoa(ipaddr->sin_addr));
	return 0;
}	
int getmac_addr(char *argv[])
{
    struct ifreq s;
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(fd==-1)
    {
		tst_brkm(TCONF,NULL,"socket failed for mac address");
		}

   strcpy(s.ifr_name, argv[1]);
	 
    if (ioctl(fd, SIOCGIFHWADDR, &s)==0) {
    int i;
    for (i = 0; i < 6; ++i)
    tst_resm(TPASS,"mac address %02x", (unsigned char) s.ifr_addr.sa_data[i]);
        
    return 0;
    }
    else
	 {
   tst_resm(TFAIL,"ioctl failed for mac address");
   }
return 0;
}
int gethost_name(void) 
{ 
    char hostbuffer[256]; 
    char *IPbuffer; 
    struct hostent *host_entry; 
      
    // To retrieve hostname 
    gethostname(hostbuffer, sizeof(hostbuffer)); 
    //checkHostName(hostname); 
  
    // To retrieve host information 
    host_entry = gethostbyname(hostbuffer); 
    //checkHostEntry(host_entry); 
  
    // To convert an Internet network 
    // address into ASCII string 
    IPbuffer = inet_ntoa(*((struct in_addr*) 
                           host_entry->h_addr_list[0])); 
  
    tst_resm(TPASS,"Hostname: %s\n", hostbuffer); 
    tst_resm(TPASS,"Host IP: %s", IPbuffer); 
  
    return 0; 
} 
int main(int argc, char *argv[])
{
gethost_name() ;
getip_addr(argv);
getmac_addr(argv);
getgatewayandiface();

return 0;
}

