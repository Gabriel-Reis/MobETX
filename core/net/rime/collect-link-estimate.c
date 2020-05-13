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

/**
 * \addtogroup rimelinkestimate
 * @{
 */

#include "net/rime/collect.h"
#include "net/rime/collect-link-estimate.h"

#define INITIAL_LINK_ESTIMATE 16

#define COLLECT_LINK_ESTIMATE_ALPHA ((3 * (COLLECT_LINK_ESTIMATE_UNIT)) / 8)

#define MAX_ESTIMATES 255

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#include <stdio.h>
#include <stdlib.h>

int mem_b = 0;
struct MetricStructHead *Metrics = NULL;


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

    int emm = estimated_mobility_metric();
    le->etx_accumulator = (((uint32_t)tx * COLLECT_LINK_ESTIMATE_UNIT) *
     COLLECT_LINK_ESTIMATE_ALPHA +
     le->etx_accumulator * (COLLECT_LINK_ESTIMATE_UNIT -
      COLLECT_LINK_ESTIMATE_ALPHA)) /
    COLLECT_LINK_ESTIMATE_UNIT;

    int mobetx = emm+le->etx_accumulator;
    
    //id: rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]
    PRINTF("ETX ===  %u EMM == %d TIME==%d MOBETX==%u\n",le->etx_accumulator, emm, clock_seconds(), mobetx);
    //le->etx_accumulator;
    //collect_print_stats();
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
/*---------------------------------------------------------------------------*/
void
read_estimated_mobility_metric()
{
  FILE *arq;  
  // int count = 0;
  arq = fopen("../../../core/net/emm.txt","r");/* /home/user/contiki/core/net */
  if(arq == NULL) {
    PRINTF("EMM.txt ERRO AO LER ARQUIVO\n");
  }
  else{
    PRINTF("EMM.txt lido com sucesso\n");
    //Adicionar variáveis para características lidas do arquivo txt
    int id,m1,m2,time,i;
    char string[50];

    for(i=0;i<10;i++)
      fgets(string, 50, arq);

    while(fscanf(arq, "%d %d %d %d",&id,&m1,&m2,&time)!=EOF){
      if(id == ( linkaddr_node_addr.u8[0]+linkaddr_node_addr.u8[1]*10) ){
        struct MetricStruct *NewMetric=(struct MetricStruct*)malloc(sizeof(struct MetricStruct));
      //Calculate te EMM in the next line
        NewMetric->emm = m1+m2;
        NewMetric->time = time;
        NewMetric->next = NULL;
        insert_metric(NewMetric);
      }
    }
  //print_metric();
    count_metric();
    mem_b = 1;
  }
  fclose(arq);
}
/*---------------------------------------------------------------------------*/
int
estimated_mobility_metric()
{
  if(mem_b == 0)
    read_estimated_mobility_metric();

  struct MetricStruct *aux;
  if(Metrics->lastUsed == NULL)
    aux = Metrics->head;
  else
    aux = Metrics->lastUsed; 

  while(aux->next != NULL && aux->next->time <= clock_seconds())
    aux = aux->next;

  Metrics->lastUsed = aux;
  return aux->emm;
}
/*---------------------------------------------------------------------------*/
void
insert_metric(struct MetricStruct *NewMetric)
{
  if(Metrics != NULL){
    Metrics->last->next = NewMetric;
    Metrics->last = NewMetric;
  }
  else{
    Metrics = (struct MetricStructHead*)malloc(sizeof(struct MetricStructHead));
    Metrics->head = NewMetric;
    Metrics->last = NewMetric;
    Metrics->lastUsed = NULL;
  }
}
/*---------------------------------------------------------------------------*/
void
print_metric(){
  if(Metrics != NULL){
    int count = 0;
    struct MetricStruct *aux;
    PRINTF("EMM.txt :");
    for(aux = Metrics->head;aux != NULL ;aux = aux->next){
      PRINTF("-%.2f-",aux->emm);
    }
    PRINTF("\n");
  }
  else
    PRINTF("EMM.txt VECTOR NULL \n");
}

void
count_metric(){
  if(Metrics != NULL){
    int count = 0;
    struct MetricStruct *aux;
    for(aux = Metrics->head;aux != NULL ;aux = aux->next){
      count++;
    }
    PRINTF("EMM.txt TOTAL: %d \n",count);
  }
  else
    PRINTF("EMM.txt: VECTOR NULL \n");
}