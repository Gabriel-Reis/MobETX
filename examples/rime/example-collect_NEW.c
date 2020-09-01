/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Example of how the collect primitive works.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include "net/rime/collect.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"


/*HERE*/ 
#include "../examples/rime/collect-common.h"
#include "collect-view.h"
#include "rpl.h" 
#include "net/ipv6/uip-ds6-route.h"
#include "net/ip/uip-udp-packet.h"
#include "net/rpl/rpl.h"
#include "dev/serial-line.h"
#include "apps/shell/shell-collect-view.h"
#if CONTIKI_TARGET_Z1
#include "dev/uart0.h"
#else
#include "dev/uart1.h"
#endif



#include "net/netstack.h"

#include <stdio.h>

static struct collect_conn tc;

/*---------------------------------------------------------------------------*/
PROCESS(example_collect_process, "Test collect process");
AUTOSTART_PROCESSES(&example_collect_process);
/*---------------------------------------------------------------------------*/
static void
recv(const linkaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  printf("Sink got message from %d.%d, seqno %d, hops %d: len %d '%s'\n",
    originator->u8[0], originator->u8[1],
    seqno, hops,
    packetbuf_datalen(),
    (char *)packetbuf_dataptr());
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_collect_process, ev, data)
{
  static struct etimer periodic;
  static struct etimer et;
  static struct etimer stats;
  int minutes = 15;

  PROCESS_BEGIN();

    #if PLATFORM_HAS_BUTTON
  SENSORS_ACTIVATE(button_sensor);
  #endif
  /*HERE*/ collect_common_net_init();
  collect_open(&tc, 130, COLLECT_ROUTER, &callbacks);

  if(linkaddr_node_addr.u8[0] == 1 &&
   linkaddr_node_addr.u8[1] == 0) {
   printf("I am sink\n");
 collect_set_sink(&tc, 1);
}

  /* Allow some time for the network to settle. */
etimer_set(&et, 120 * CLOCK_SECOND);
PROCESS_WAIT_UNTIL(etimer_expired(&et));

while(1) {

    /* Send a packet every 30 seconds. */
  if(etimer_expired(&periodic)) {
    etimer_set(&periodic, CLOCK_SECOND * 30);
    etimer_set(&et, random_rand() % (CLOCK_SECOND * 30));
  }

  if(etimer_expired(&stats)) {
    etimer_set(&stats, CLOCK_SECOND*60*minutes);
    collect_print_stats();
  }

  PROCESS_WAIT_EVENT();

  if(etimer_expired(&et)) {
    static linkaddr_t oldparent;
    const linkaddr_t *parent;

    printf("Sending\n");
    packetbuf_clear();
    packetbuf_set_datalen(sprintf(packetbuf_dataptr(),
      "%s", "Hello") + 1);
    collect_send(&tc, 15);

    parent = collect_parent(&tc);
    if(!linkaddr_cmp(parent, &oldparent)) {
      if(!linkaddr_cmp(&oldparent, &linkaddr_null)) {
        printf("#L %d 0\n", oldparent.u8[0]);
      }
      if(!linkaddr_cmp(parent, &linkaddr_null)) {
        printf("#L %d 1\n", parent->u8[0]);
      }
      linkaddr_copy(&oldparent, parent);
    }
  }

}

PROCESS_END();
}

// int id0 = linkaddr_node_addr.u8[0];
// int id1 = linkaddr_node_addr.u8[1];
#if 1
/*---------------------------------------------------------------------------*/
  void collect_common_set_sink(void){}
/*---------------------------------------------------------------------------*/
  void collect_common_net_print(void){ /* PRINTF("I am sink!\n"); */ }
/*---------------------------------------------------------------------------*/
void collect_common_send(void) {  /* Server never sends */ }
/*---------------------------------------------------------------------------*/
  void collect_common_net_init(void) {
    uart1_set_input(serial_line_input_byte);
    serial_line_init();
  }

#else

void collect_common_net_init(void)
{
  #if CONTIKI_TARGET_Z1
  uart0_set_input(serial_line_input_byte);
  #else
  uart1_set_input(serial_line_input_byte);
  #endif
  serial_line_init();

  // PRINTF("I am sink!\n");
}
void collect_common_send(void) {
  static uint8_t seqno;
  struct {
    uint8_t seqno;
    uint8_t for_alignment;
    struct collect_view_data_msg msg;
  } msg;
/* struct collect_neighbor *n; */
  uint16_t parent_etx;
  uint16_t rtmetric;
  uint16_t num_neighbors;
  uint16_t beacon_interval;
  rpl_parent_t *preferred_parent;
  linkaddr_t parent;
  rpl_dag_t *dag;

 //  if(client_conn == NULL) {
 // Not setup yet 
 //    return;
 //  }
  memset(&msg, 0, sizeof(msg));
  seqno++;
  if(seqno == 0) {
/* Wrap to 128 to identify restarts */
    seqno = 128;
  }
  msg.seqno = seqno;

  linkaddr_copy(&parent, &linkaddr_null);
  parent_etx = 0;

/* Let’s suppose we have only one instance */
  dag = rpl_get_any_dag();
  if(dag != NULL) {
    preferred_parent = dag->preferred_parent;
    if(preferred_parent != NULL) {
      uip_ds6_nbr_t *nbr;
      nbr = uip_ds6_nbr_lookup(rpl_get_parent_ipaddr(preferred_parent));
      if(nbr != NULL) {
/* Use parts of the IPv6 address as the parent address, in reversed byte order. */
        parent.u8[LINKADDR_SIZE - 1] = nbr->ipaddr.u8[sizeof(uip_ipaddr_t) -2];
        parent.u8[LINKADDR_SIZE - 1] = nbr->ipaddr.u8[sizeof(uip_ipaddr_t) -1];
        parent_etx = rpl_get_parent_rank((uip_lladdr_t *) uip_ds6_nbr_get_ll(nbr)) / 2;
      }
    }
    rtmetric = dag->rank;
    beacon_interval = (uint16_t) ((2L << dag->instance->dio_intcurrent) / 1000);
    num_neighbors = uip_ds6_nbr_num();
  } else {
    rtmetric = 0;
    beacon_interval = 0;
    num_neighbors = 0;
  }

/* num_neighbors = collect_neighbor_list_num(&tc.neighbor_list); */
  collect_view_construct_message(&msg.msg, &parent,
    parent_etx, rtmetric,
    num_neighbors, beacon_interval);

  uip_udp_packet_sendto(client_conn, &msg, sizeof(msg),
    &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}

void
collect_common_set_sink(void)
{
/* A udp client can never become sink */
}
/*—————————————————————————*/

void
collect_common_net_print(void)
{
  rpl_dag_t *dag;
uip_ds6_route_t *r;

/* Let’s suppose we have only one instance */
  dag = rpl_get_any_dag();
if(dag->preferred_parent != NULL) {
printf("Preferred parent: ");
PRINT6ADDR(rpl_get_parent_ipaddr(dag->preferred_parent));
printf("\n");
}
for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {
PRINT6ADDR(&r->ipaddr);
}
printf("—\n");
}
/*—————————————————————————*/
static void
tcpip_handler(void)
{
  // if(uip_newdata()) {
   // Ignore incoming data 
 }
}
/*—————————————————————————*/
#endif