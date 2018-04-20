#!/usr/bin/env python2

from socket import socket, AF_PACKET, SOCK_RAW

TX_IF = str( "alice" )
RX_IF = str( "bob" )

TX_IF_L2 = [ 0x02, 0xDE, 0xAD, 0xBE, 0xEF, 0x01 ]
RX_IF_L2 = [ 0x02, 0xDE, 0xAD, 0xBE, 0xEF, 0x02 ]

src_mac     = TX_IF_L2
dst_mac     = RX_IF_L2
rtc_payload = [ 0x88, 0x92, 0xf7, 0xff, 0x80, 0x00, \
                0x80, 0x80, 0x00, 0x00, 0x00, 0x00, \
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                0x00, 0x00 ]


# convert byte list to byte string
def pack( byte_sequence ):
    return b"".join( map( chr, byte_sequence ) )


def main():
    # open socket
    s = socket( AF_PACKET, SOCK_RAW )
    
    # bind socket to sending interface
    s.bind( ( TX_IF, 0 ) )
    
    # send frame
    s.send( pack( dst_mac ) + pack( src_mac ) + pack( rtc_payload ) )
    
    # close socket
    s.close()


if __name__ == "__main__":
    main()
    