/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "netif/tunif.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "lwip/debug.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/mem.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"

static char hostname[24];

#define LWIP_UNIX_LINUX 1
#if LWIP_IPV4 /* @todo: IPv6 */
#if !NO_SYS

#define IFNAME0 'e'
#define IFNAME1 'n'

#ifndef TUNIF_DEBUG
#define TUNIF_DEBUG LWIP_DBG_OFF
#endif

//#define IFCONFIG_CALL "/sbin/ifconfig tun0 inet %d.%d.%d.%d %d.%d.%d.%d"
#define IFCONFIG_CALL "/sbin/ip addr add %d.%d.%d.%d/24 dev tun0"
#define ROUTE_CALL "/sbin/ip route add default via %d.%d.%d.%d"

struct tunif {
  /* Add whatever per-interface state that is needed here. */
  int fd;
};

/* Forward declarations. */
static void  tunif_input(struct netif *netif);
static err_t tunif_output(struct netif *netif, struct pbuf *p,
                          const ip4_addr_t *ipaddr);

static void tunif_thread(void *data);

/*-----------------------------------------------------------------------------------*/
static void
low_level_init(struct netif *netif)
{
  struct tunif *tunif;
  char buf[sizeof(IFCONFIG_CALL) + 50];

  tunif = (struct tunif *)netif->state;

  /* Obtain MAC address from network interface. */

  /* Do whatever else is needed to initialize interface. */

  tunif->fd = open("/dev/net/tun", O_RDWR);
  printf("tunif_init: fd %d\n", tunif->fd);
  if (tunif->fd == -1) {
    perror("tunif_init");
    exit(1);
  }
#ifdef LWIP_UNIX_LINUX
  {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    //if (preconfigured_tapif) {
    //  strncpy(ifr.ifr_name, preconfigured_tapif, sizeof(ifr.ifr_name));
    //} else {
    //  strncpy(ifr.ifr_name, DEVTAP_DEFAULT_IF, sizeof(ifr.ifr_name));
    //} 
    strncpy(ifr.ifr_name, "/dev/tun0", sizeof(ifr.ifr_name));
    ifr.ifr_name[sizeof(ifr.ifr_name)-1] = 0; /* ensure \0 termination */
    ifr.ifr_name[0]=0;

    ifr.ifr_flags = IFF_TUN|IFF_NO_PI;
    if (ioctl(tunif->fd, TUNSETIFF, (void *) &ifr) < 0) {
      perror("tunif_init: tun0 ioctl TUNSETIFF");
      exit(1);
    }
  }
#endif /* LWIP_UNIX_LINUX */


  sprintf(buf, IFCONFIG_CALL,
           ip4_addr1(netif_ip4_addr(netif)),
           ip4_addr2(netif_ip4_addr(netif)),
           ip4_addr3(netif_ip4_addr(netif)),
           ip4_addr4(netif_ip4_addr(netif)));


  printf ("tunif_init: system(\"%s\");\n", buf);
  system(buf);

  sprintf(buf, ROUTE_CALL,
      ip4_addr1(netif_ip4_gw(netif)),
      ip4_addr2(netif_ip4_gw(netif)),
      ip4_addr3(netif_ip4_gw(netif)),
      ip4_addr4(netif_ip4_gw(netif)));
  printf ("tunif_init: system(\"%s\");\n", buf);
  system(buf);

  sys_thread_new("tunif_thread", tunif_thread, netif, DEFAULT_THREAD_STACKSIZE*2, DEFAULT_THREAD_PRIO);

}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  char buf[1540];
  //int rnd_val;
  struct tunif *tunif;

  tunif = (struct tunif *)netif->state;
  ssize_t written;

  /* initiate transfer(); */

#if 0
  rnd_val = rand();
  if (((double)rnd_val/(double)RAND_MAX) < 0.4) {
    printf("drop\n");
    return ERR_OK;
  }
#endif
  pbuf_copy_partial(p, buf, p->tot_len, 0);
  if (p->tot_len<64) {
      p->tot_len=64;
  }

  /* signal that packet should be sent(); */
  written=write(tunif->fd, p->payload, p->tot_len);
  if (written == -1) {
    printf("write %d,%d\n",tunif->fd,p->tot_len);
    perror("tunif: write");
    if (errno==EINVAL) {
        printf("EINVAL\n");
    }
  }
  return ERR_OK;
}
/*
 *   // write length + packet
uint16_t plength = htons(p->tot_len);
written = write(tunif->fd, (char *)&plength, sizeof(plength));
if (written == -1) {
    printf("write %d,%d\n",tunif->fd,p->tot_len);
  perror("tunif: 1st write");
}

*/
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */
/*-----------------------------------------------------------------------------------*/
static struct pbuf *
low_level_input(struct tunif *tunif)
{
  struct pbuf *p;
  ssize_t len;
  char buf[1500];

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = read(tunif->fd, buf, sizeof(buf));
  if((len <= 0) || (len > 0xffff)) {
    return NULL;
  }

  /*  if (((double)rand()/(double)RAND_MAX) < 0.1) {
    printf("drop\n");
    return NULL;
    }*/

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_LINK, (u16_t)len, PBUF_POOL);

  if (p != NULL) {
    pbuf_take(p, buf, (u16_t)len);
    /* acknowledge that packet has been read(); */
  } else {
    /* drop packet(); */
  }

  return p;
}
/*-----------------------------------------------------------------------------------*/
static void 
tunif_thread(void *arg)
{
  struct netif *netif;
  struct tunif *tunif;
  fd_set fdset;
  int ret;

  netif = (struct netif *)arg;
  tunif = (struct tunif *)netif->state;

  while (1) {
    FD_ZERO(&fdset);
    FD_SET(tunif->fd, &fdset);

    /* Wait for a packet to arrive. */
    ret = select(tunif->fd + 1, &fdset, NULL, NULL, NULL);

    if (ret == 1) {
      /* Handle incoming packet. */
      tunif_input(netif);
    } else if (ret == -1) {
      perror("tunif_thread: select");
    }
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tunif_output():
 *
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actuall transmission of the packet.
 *
 */
/*-----------------------------------------------------------------------------------*/
static err_t
tunif_output(struct netif *netif, struct pbuf *p,
             const ip4_addr_t *ipaddr)
{
  struct tunif *tunif;
  LWIP_UNUSED_ARG(ipaddr);

  tunif = (struct tunif *)netif->state;

  return low_level_output(netif, p);

}
/*-----------------------------------------------------------------------------------*/
/*
 * tunif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
tunif_input(struct netif *netif)
{
  struct tunif *tunif;
  struct pbuf *p;


  tunif = (struct tunif *)netif->state;

  p = low_level_input(tunif);

  if (p == NULL) {
    LWIP_DEBUGF(TUNIF_DEBUG, ("tunif_input: low_level_input returned NULL\n"));
    return;
  }

#if 0
/* CS: ip_lookup() was removed */
  if (ip_lookup(p->payload, netif)) {
#endif
    netif->input(p, netif);
#if 0
  }
#endif
}
/*-----------------------------------------------------------------------------------*/
/*
 * tunif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
tunif_init(struct netif *netif)
{
  struct tunif *tunif;

  tunif = (struct tunif *)mem_malloc(sizeof(struct tunif));
  if (!tunif) {
    return ERR_MEM;
  }
  netif->state = tunif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = tunif_output;
  netif->linkoutput = low_level_output;

  low_level_init(netif);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/

#endif /* !NO_SYS */
#endif /* LWIP_IPV4 */


/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
#if ESP_LWIP
wlanif_input(struct netif *netif, void *buffer, u16_t len, void* eb)
#else
wlanif_input(struct netif *netif, void *buffer, uint16 len)
#endif
{
  struct pbuf *p;
  
#if ESP_LWIP
    if(buffer== NULL)
    	goto _exit;
    if(netif == NULL)
    	goto _exit;
#endif

#if ESP_LWIP
  p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
  if (p == NULL){
#if ESP_PERF
      g_rx_alloc_pbuf_fail_cnt++;
#endif
      return;
  }
  p->payload = buffer;
  p->eb = eb;
#else
  p = pbuf_alloc(PBUF_IP, len, PBUF_POOL);
  if (p == NULL) {
    return;
  }
  memcpy(p->payload, buffer, len);
#endif


  /* full packet send to tcpip_thread to process */
  if (netif->input(p, netif) != ERR_OK) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
  }
  
_exit:
;	  
}


/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
wlanif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));


#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */

#if ESP_LWIP
//TO_DO
/*
  if ((struct netif *)wifi_get_netif(STATION_IF) == netif) {
      if (default_hostname == 1) {
          wifi_station_set_default_hostname(netif->hwaddr);
      }
      netif->hostname = hostname;
  } else {
      netif->hostname = NULL;
  }
*/
  sprintf(hostname, "ESP_%02X%02X%02X", netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
  netif->hostname = hostname;
  
#else
  sprintf(hostname, "ESP_%02X%02X%02X", netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
  netif->hostname = hostname;
#endif
  
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  //NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

