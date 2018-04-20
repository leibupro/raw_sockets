/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pl@vqe.ch> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   P. Leibundgut
 * ----------------------------------------------------------------------------
 *
 * File:      mitm.c
 *
 *
 * Purpose:   This is an exemplary application of how
 *            a "man in the middle" can be setup between
 *            two ethernet interfaces. It uses
 *            raw sockets to listen on the lowest possible
 *            network layer, which is the link layer (L2).
 *
 *
 * Remarks:   - Program was wriiten and tested on
 *              debian and arch distributions with
 *              a kernel version >= 4.9.
 *
 *
 * Date:      04/2018
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/param.h>

#include <arpa/inet.h>

#include <net/if.h>

#include <netpacket/packet.h>

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define PROTO_PROFINET   0x8892
#define ETH_DEVICE_FOO    "foo"
#define ETH_DEVICE_BAR    "bar"

/* to satisfy the jumbo frame studs, 
 * we take a buf size > 9600 bytes ... */
#define RX_BUF_SIZE       10240  /* unit is bytes */
#define TX_BUF_SIZE       10240  /* unit is bytes */


typedef struct
{
    int foo;
    int bar;
}
sd_pair_t;


/* socket file descriptors */
static sd_pair_t sfds = { -1, -1 };

static volatile sig_atomic_t running = 1;


/* function prototypes */
static void srvr_runnr( void );
static void setup_sockets( void );
static int open_and_bind_raw_socket( char* if_name, unsigned short proto );
static void free_bufs( void** foo_rx, void** foo_tx,
                       void** bar_rx, void** bar_tx );
static int forward( int sfd_rx, int sfd_tx, fd_set* rfds, 
                    void* rx_buf, size_t rx_buf_len, 
                    void* tx_buf );
static void signal_handler( int signum );


static void signal_handler( int signum )
{
    if( signum == SIGINT )
    {
        ( void )fprintf( stdout, "Received SIGINT, terminating ...\n" );
    }
    running = 0;
}


static void free_bufs( void** foo_rx, void** foo_tx,
                       void** bar_rx, void** bar_tx )
{
    free( *foo_rx );
    free( *foo_tx );
    free( *bar_rx );
    free( *bar_tx );
    *foo_rx = NULL;
    *foo_tx = NULL;
    *bar_rx = NULL;
    *bar_tx = NULL;
}


static int forward( int sfd_rx, int sfd_tx, fd_set* rfds, 
                    void* rx_buf, size_t rx_buf_len, 
                    void* tx_buf )
{
    ssize_t rx_bytes;

    if( FD_ISSET( sfd_rx, rfds ) )
    {
        if( ( rx_bytes = read( sfd_rx, rx_buf, rx_buf_len ) ) == -1 )
        {
            perror( "Failed to receive data." );
            return -1;
        }
        ( void )fprintf( stdout, "Received %zd bytes of data.\n", rx_bytes );

        ( void )memmove( tx_buf,
                         ( const void* )rx_buf,
                         ( size_t )rx_bytes );

        /* modify the frame content, mitms just want to have fun ... */
        *( ( ( unsigned char* )tx_buf ) + 14U ) = 0xFFU;

        if( write( sfd_tx, ( const void* )tx_buf, ( size_t )rx_bytes ) == -1 )
        {
            perror( "Failed to send data." );
            return -1;
        }
    }
    return 0;
}


static void srvr_runnr( void )
{
    fd_set rfds;
    int nfds;
    unsigned char* foo_rx_buf = NULL;
    unsigned char* foo_tx_buf = NULL;
    unsigned char* bar_rx_buf = NULL;
    unsigned char* bar_tx_buf = NULL;

    nfds = ( MAX( sfds.foo, sfds.bar ) + 1 );

    if( !( foo_rx_buf = ( unsigned char* )calloc( RX_BUF_SIZE, sizeof( unsigned char ) ) ) ||
        !( foo_tx_buf = ( unsigned char* )calloc( TX_BUF_SIZE, sizeof( unsigned char ) ) ) ||
        !( bar_rx_buf = ( unsigned char* )calloc( RX_BUF_SIZE, sizeof( unsigned char ) ) ) ||
        !( bar_tx_buf = ( unsigned char* )calloc( TX_BUF_SIZE, sizeof( unsigned char ) ) ) )
    {
        perror( "Failed to allocate memory for buffers." );
        exit( EXIT_FAILURE );
    }

    while( running )
    {
        FD_ZERO( &rfds );
        FD_SET( sfds.foo, &rfds );
        FD_SET( sfds.bar, &rfds );

        if( select( nfds, &rfds, NULL, NULL, NULL ) < 0 )
        {
            if( errno != EINTR )
            {
                perror( "select failed." );
                goto exit_fail;
            }
            else
            {
                ( void )fprintf( stdout, "Select was interrupted,\n"
                                         "stopping the mitm srvr ...\n");
                break;
            }
        }

        if( ( forward( sfds.foo, sfds.bar, &rfds,
                       ( void* )foo_rx_buf, RX_BUF_SIZE,
                       ( void* )bar_tx_buf ) == -1 )
            ||
            ( forward( sfds.bar, sfds.foo, &rfds,
                       ( void* )bar_rx_buf, RX_BUF_SIZE,
                       ( void* )foo_tx_buf ) == -1 )
          )
        {
            goto exit_fail;
        }
    }
    ( void )close( sfds.foo );
    ( void )close( sfds.bar );
    free_bufs( ( void** )&foo_rx_buf, ( void** )&foo_tx_buf,
               ( void** )&bar_rx_buf, ( void** )&bar_tx_buf );
    return;

    exit_fail:
        ( void )close( sfds.foo );
        ( void )close( sfds.bar );
        free_bufs( ( void** )&foo_rx_buf, ( void** )&foo_tx_buf,
                   ( void** )&bar_rx_buf, ( void** )&bar_tx_buf );
        exit( EXIT_FAILURE );
}


static int open_and_bind_raw_socket( char* if_name, unsigned short proto )
{
    int sfd;
    struct sockaddr_ll addr;

    if( ( ( sfd = socket( AF_PACKET, SOCK_RAW, htons( proto ) ) ) == -1 ) )
    {
        perror( "socket creation failed:" );
        exit( EXIT_FAILURE );
    }

    ( void )memset( &addr, 0, sizeof( struct sockaddr_ll ) );
    addr.sll_family   = AF_PACKET;
    addr.sll_protocol = htons( proto );
    addr.sll_ifindex  = if_nametoindex( if_name );

    if( bind( sfd, ( struct sockaddr* )&addr, sizeof( struct sockaddr_ll ) ) < 0 )
    {
        perror( "socket bind failed:" );
        ( void )close( sfd );
        exit( EXIT_FAILURE );
    }

    return sfd;
}


static void setup_sockets( void )
{
    sfds.foo = open_and_bind_raw_socket( ETH_DEVICE_FOO, PROTO_PROFINET );
    sfds.bar = open_and_bind_raw_socket( ETH_DEVICE_BAR, PROTO_PROFINET );
}


int main( void )
{
    /* adding a signal handler */
    if( signal( SIGINT, &signal_handler ) == SIG_ERR )
    {
        ( void )fprintf( stderr, "Unable to catch SIGINT ...\n" );
        return EXIT_FAILURE;
    }

    setup_sockets();
    srvr_runnr();

    ( void )fprintf( stdout, "That's all, folks. Exiting ...\n" );

    return EXIT_SUCCESS;
}

