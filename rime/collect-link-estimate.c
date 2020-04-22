/**
 * \addtogroup rimelinkestimate
 * @{
 */
/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
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
 *         Implementation of Collect link estimate based on ETX
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "net/rime/collect.h"
#include "net/rime/collect-link-estimate.h"

#define INITIAL_LINK_ESTIMATE 16

#define COLLECT_LINK_ESTIMATE_ALPHA ((3 * (COLLECT_LINK_ESTIMATE_UNIT)) / 8)

#define MAX_ESTIMATES 255

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


int mem[10][2];
int mem_b = 0;


/*---------------------------------------------------------------------------*/
void
collect_link_estimate_new(struct collect_link_estimate *le)
{
  if(le == NULL) {
    return;
  }
  le->num_estimates = 0;
  le->etx_accumulator = COLLECT_LINK_ESTIMATE_UNIT;
}
/*---------------------------------------------------------------------------*/
void
collect_link_estimate_update_tx(struct collect_link_estimate *le, uint8_t tx)
{
  if(le == NULL) {
    return;
  }
  if(tx == 0) {
    /*    printf("ERROR tx == 0\n");*/
    return;
  }
  if(le != NULL) {
    if(le->num_estimates == 0) {
      le->etx_accumulator = tx * COLLECT_LINK_ESTIMATE_UNIT;
    }

    if(le->num_estimates < MAX_ESTIMATES) {
      le->num_estimates++;
    }

    int emm = estimated_mobility_metric()* COLLECT_LINK_ESTIMATE_UNIT;
    le->etx_accumulator = ((((uint32_t)tx * COLLECT_LINK_ESTIMATE_UNIT) *
                           COLLECT_LINK_ESTIMATE_ALPHA +
                           le->etx_accumulator * (COLLECT_LINK_ESTIMATE_UNIT -
                                                  COLLECT_LINK_ESTIMATE_ALPHA)) /
      COLLECT_LINK_ESTIMATE_UNIT);

    //PRINTF("ETX %d.%d ===  %u",rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],le->etx_accumulator);
    int mobetx = emm+le->etx_accumulator;
    PRINTF("ETX %d.%d ===  %u EMM == %d MOBETX==%u\n",rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],le->etx_accumulator, emm, mobetx);
    le->etx_accumulator+=emm;

  }
}
/*---------------------------------------------------------------------------*/
void
collect_link_estimate_update_tx_fail(struct collect_link_estimate *le,
                                     uint8_t tx)
{
  if(le == NULL) {
    return;
  }
  collect_link_estimate_update_tx(le, tx * 2);
}
/*---------------------------------------------------------------------------*/
void
collect_link_estimate_update_rx(struct collect_link_estimate *n)
{

}
/*---------------------------------------------------------------------------*/
uint16_t
collect_link_estimate(struct collect_link_estimate *le)
{
  if(le == NULL) {
    return 0;
  }
  if(le->num_estimates == 0) {
    return INITIAL_LINK_ESTIMATE * COLLECT_LINK_ESTIMATE_UNIT;
  }

  return le->etx_accumulator;
}
/*---------------------------------------------------------------------------*/
int
collect_link_estimate_num_estimates(struct collect_link_estimate *le)
{
  if(le != NULL) {
    return le->num_estimates;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/** @} */

void
read_estimated_mobility_metric()
{
  FILE *arq;  
  arq = fopen("../emm.txt","r");          /*  /home/user/contiki/tools/cooja */
  if(arq == NULL) {
    PRINTF("emm.txt ERRO AO LER ARQUIVO\n");
  }
  else{
    int i = 0;
    while(!feof(arq)){
      //IF PARA SOMENTE SEUS DADOS
      fscanf(arq, "%d %d",&mem[i][0],&mem[i][1]);
      i++;
    }
    mem_b = 1;
  }
  fclose(arq);
}

int
estimated_mobility_metric()
{
  if(mem_b == 0)
    read_estimated_mobility_metric();
  int id = rimeaddr_node_addr.u8[0];
  return (mem[id-1][0]+mem[id-1][1]);
}