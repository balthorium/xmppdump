#include <iostream>
#include <pcap.h>
#include <string.h>
#include <string>
#include <sstream>
#include <memory>
#include "xmppdump.h"
#include "xmlpp.h"

using namespace std;

// converts ip address and port to string 
static void addr2str(uint8_t ip[], uint16_t port, string &address)
{
    std::ostringstream ssaddr;
    ssaddr << static_cast<int>(ip[0]) << "." 
        << static_cast<int>(ip[1]) << "." 
        << static_cast<int>(ip[2]) << "." 
        << static_cast<int>(ip[3]) << ":" 
        << ntohs(port);

    address = ssaddr.str();
}

// callback invoked by libpcap for every packet captured
static void on_packet(
        u_char *, 
        const struct pcap_pkthdr *header, 
        const u_char *packet)
{
    // get a pointer to the IP header, cast to ip_hdr struct
    struct ip_hdr *ip = (struct ip_hdr *) 
            (packet + sizeof(dlt_linux_sll_hdr));

    // get a pointer to the TCP header, cast to tcp_hdr struct
    struct tcp_hdr *tcp = (struct tcp_hdr *) 
            ((size_t)ip + (size_t)(ip->hdr_len*4));

    // stringify the ip:port for tcp source and destination
    string src, dst;
    addr2str(ip->src, tcp->src_port, src);
    addr2str(ip->dst, tcp->dst_port, dst);

    // stringify and collect tcp flags
    ostringstream tcpflags;
    if (tcp->flags & TCP_FLAG_SYN) tcpflags << "S";
    if (tcp->flags & TCP_FLAG_FIN) tcpflags << "F";
    if (tcp->flags & TCP_FLAG_RST) tcpflags << "R";
    if (tcp->flags & TCP_FLAG_URG) tcpflags << "U";
    if (tcp->flags & TCP_FLAG_PSH) tcpflags << "P";
    if (tcp->flags & TCP_FLAG_ACK) tcpflags << "A";

    // get pointer to the tcp packet payload, and data length
    const char *msg = (const char *) ((size_t)tcp + (size_t)(tcp->hdr_len*4));
    uint16_t msg_len = ntohs(ip->pkt_len) - ip->hdr_len*4 - tcp->hdr_len*4;

    // QAD check to see if this is readable
    // TODO: make UTF-8 friendly
    bool readable = true;
    for (int i = 0; i < msg_len; ++i)
    {
        if (!isascii(msg[i]))
        {
            readable = false;
            break;
        }
    }

    // if data is readable ascii, try to parse into formatted xml
    XmlPrettyPrinter pp;
    int flength = msg_len;
    if (readable)
    {
        if (pp.parse(msg, msg_len))
        {
            flength = pp.getPrettyStream().str().length();
        }
    }

    // dump tcp header information to standard out
    printf("[tcp from=\"%s\" to=\"%s\" flags=\"%s\" seqno=\"%u\" ackno=\"%u\" "
            "time=\"%llu\" length=\"%d\" flength=\"%d\" readable=\"%s\"]\n", 
            src.c_str(), dst.c_str(), tcpflags.str().c_str(), 
            ntohl(tcp->seqnum), ntohl(tcp->acknum),
            ((uint64_t)header->ts.tv_sec * 1000) + 
            ((uint64_t)header->ts.tv_usec / 1000), msg_len, flength,
            (readable) ? "true" : "false");

    // if the payload was readable ascii, dump it to standard out
    if (readable)
    {
        if (flength != msg_len)
        {
            cout << pp.getPrettyStream().str() << "\n";
        }
        else
        {
            printf("%.*s\n", msg_len, msg);
        }
    }
    
    printf("\n");
    fflush(stdout);
 }


// Main xmppdump application.
// xmppdump monitors local ip traffic on ports given on the command line.
// If the data content of packets sent and received on those ports are
// text (currently only ascii is recognized), then it will write the packet 
// data contents to the stdout.  
 int main(int argc, char *argv[])
 {
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;

    if (argc == 1)
    {
        cout << "usage: " << argv[0] << " port [port...]" << endl << endl;  
        cout << "\tport  A port number to include in the capture.  All\n"
                "\t      ascii-text traffic to and from this port will be\n"
                "\t      captured and written to stdout.  One or more\n"
                "\t      ports may be specified on the command line.\n\n" 
                "Note that this command must be run as root." << endl << endl;
        return -1;
    }

    // construct capture filter string 
    ostringstream ossfilter;
    for (int i = 1; i < argc; ++i)
    {
        ossfilter << "src port " << argv[i] << " or dst port " << argv[i];
        if (i + 1 < argc) ossfilter << " or ";
    }
    auto_ptr<char> filter(new char[ossfilter.str().length() + 1]);
    memset(filter.get(), 0, ossfilter.str().length() + 1);
    ossfilter.str().copy(filter.get(), ossfilter.str().length(), 0);

    // TODO: I hardcode a bunch of assumptions here, and particularly
    // egregious is the use of the "any" device, only available on Linux 2.2+.
    // Should make these override-able at the command line.   
 
    // open the session in promiscuous mode (one second read-timeout)
    handle = pcap_open_live("any", BUFSIZ, 0, 1000, errbuf);
    if (handle == NULL) 
    {
        fprintf(stderr, "Could not open device \"any\": %s\n", errbuf);
        return 1;
    }

    // we listen on the "any" device, so link hdrs should be DLT_LINUX_SLL 
    if (pcap_datalink(handle) != DLT_LINUX_SLL)
    {
        fprintf(stderr, 
            "Datalink is not of type DLT_LINUX_SLL, bailing out.\n");
        return 1;
    }

    // compile and apply the filter
    if (pcap_compile(handle, &fp, filter.get(), 0, 0) == -1) 
    {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", 
                filter.get(), pcap_geterr(handle));
        return 1;
    }
    if (pcap_setfilter(handle, &fp) == -1) 
    {
        fprintf(stderr, "Couldn't install filter %s: %s\n", 
                ossfilter.str().c_str(), pcap_geterr(handle));
        return 1;
    }

    // jack packets 
    if(pcap_loop(handle, 0, &on_packet, NULL) == -1) 
    {
        fprintf(stderr, "Coulding start pcap loop: %s\n", 
                pcap_geterr(handle));
    }

    pcap_close(handle);
    return 0;
} 

