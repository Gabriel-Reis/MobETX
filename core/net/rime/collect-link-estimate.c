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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "net/rime/collect.h"
#include "net/rime/collect-link-estimate.h"


// HERE #include "collect-common.h"
// HERE #include "collect-view.h"
// HERE #include "dev/serial-line.h"
// HERE #include "apps/shell/shell-collect-view.h"


#define INITIAL_LINK_ESTIMATE 16

#define COLLECT_LINK_ESTIMATE_ALPHA ((3 * (COLLECT_LINK_ESTIMATE_UNIT)) / 8)

#define MAX_ESTIMATES 255

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

//Controle de uso da EMM
int memCtrl = 1;
//Peso da formula EMM
float alpha = 0.5;
float beta = 0.5;
float MaxSpeed = 4.1;
//Controle de leitura da EMM
int mem_b = 0;
double emmAux = 0.0;
struct MetricStructHead *Metrics = NULL;
//teste
double start = 0;


/*---------------------------------------------------------------------------*/
void
collect_link_estimate_new(struct collect_link_estimate *le)
{
  if(le == NULL) {
    return;
  }
  le->num_estimates = 0;
  le->etx_accumulator = COLLECT_LINK_ESTIMATE_UNIT;
  le->emm = 0;
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

    if(memCtrl){
      PRINTF("EMM: UPDATE EMM VALUE");
      le->emm = estimated_mobility_metric();
      // if(linkaddr_node_addr.u8[0]+linkaddr_node_addr.u8[1]*10 == 5)
      //   PRINTF("EMM VALUE le->emm: %f \n",le->emm);  
    }

    le->etx_accumulator = ((((uint32_t)tx * COLLECT_LINK_ESTIMATE_UNIT) *
     COLLECT_LINK_ESTIMATE_ALPHA +
     le->etx_accumulator * (COLLECT_LINK_ESTIMATE_UNIT -
      COLLECT_LINK_ESTIMATE_ALPHA)) /
    COLLECT_LINK_ESTIMATE_UNIT);

      // int mobetx = emm+le->etx_accumulator;
      // PRINTF("DEBUG: ETX ===  %u EMM == %lf TIME==%d MOBETX==%u\n",le->etx_accumulator, emm, clock_seconds(), mobetx);
    // }else{
    //   le->etx_accumulator = (((uint32_t)tx * COLLECT_LINK_ESTIMATE_UNIT) *
    //    COLLECT_LINK_ESTIMATE_ALPHA +
    //    le->etx_accumulator * (COLLECT_LINK_ESTIMATE_UNIT -
    //     COLLECT_LINK_ESTIMATE_ALPHA)) /
    //   COLLECT_LINK_ESTIMATE_UNIT;
    // }

    // id: rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]
    // le->etx_accumulator;
    collect_print_stats();
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

  if(memCtrl){
    // if(linkaddr_node_addr.u8[0]+linkaddr_node_addr.u8[1]*10 == 5){
    //   PRINTF("ETX VALUE return: %" PRIu32,le->etx_accumulator);
    //   PRINTF(" --  EMM VALUE return: %lf",le->emm);
    //   PRINTF("\n");
    // }
      
    return le->etx_accumulator + le->emm;
  }
  else
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


double
return_estimated_mobility_metric(struct collect_link_estimate *le)
{
  if(le != NULL) {
    return le->emm;
  }
  return 2;
}
/*---------------------------------------------------------------------------*/
/** @} */
/*---------------------------------------------------------------------------*/
void
read_estimated_mobility_metric()
{
  // PRINTF("DEBUG INFO: read_estimated_mobility_metric\n");
  FILE *arq;  
  // int count = 0;
  char arq_path[30] = "../../../core/net/emm.txt";
  arq = fopen(arq_path,"r");/* /home/user/contiki/core/net */
  if(arq == NULL) {
    PRINTF("DEBUG ERR: ERRO AO LER ARQUIVO EMM\n");   
  }
  else{
    //PRINTF("DEBUG: Lido com sucesso EMM\n");
    //Adicionar variáveis para características lidas do arquivo txt
    double linkDur,NodeSpeed;
    int id,time,LinkQnt;


    while(fscanf(arq, "%d %d %lf %lf %d",&id,&time,&linkDur,&NodeSpeed,&LinkQnt)!=EOF){
      if(id == ( linkaddr_node_addr.u8[0]+linkaddr_node_addr.u8[1]*10) ){
        struct MetricStruct *NewMetric=(struct MetricStruct*)malloc(sizeof(struct MetricStruct));
      //Calculate te EMM in the next line
        // NewMetric->emm = ((linkDur/4)/LinkQnt)+NodeSpeed;
        NewMetric->emm = (alpha*(1-((linkDur/LinkQnt)/time)))+(beta*(NodeSpeed/MaxSpeed));
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
// /*---------------------------------------------------------------------------*/
double
estimated_mobility_metric()
{
  if(memCtrl==0)
    return 0;
  PRINTF("EMM update EMM\n");
  if(mem_b == 0){
    read_estimated_mobility_metric();
    //PRINTF("DEBUG: if mem_b\n");
  }

  if(Metrics != NULL){
    //count_metric();
    struct MetricStruct *aux;
    if(Metrics->lastUsed == NULL)
      aux = Metrics->head;
    else
      aux = Metrics->lastUsed; 

    while(aux->next != NULL && aux->next->time <= clock_seconds()){
      aux = aux->next;
    }

    //PRINTF("EMM VALUE: %f \n",aux->emm);
    // if(start != aux->emm){
    //   PRINTF("EMM CHANGE: %f para %f",start,aux->emm);
    //   start = aux->emm;
    // }

    // if(linkaddr_node_addr.u8[0]+linkaddr_node_addr.u8[1]*10 == 5)
    //   PRINTF("EMM VALUE return: %f TIME: %d \n",aux->emm, aux->time);  

    Metrics->lastUsed = aux;
    return aux->emm;
  }
  else if (mem_b == 1){
    PRINTF("DEBUG INFO: NODE DONT HAVE EMM IN TXT\n");
    return 0;
  }
  else{
    PRINTF("DEBUG ERR: read_estimated_mobility_metric()\n");
    return 0;
  }
  return 0; 
}
// /*---------------------------------------------------------------------------*/
void
insert_metric(struct MetricStruct *NewMetric)
{
  //PRINTF("DEBUG: INSERT_METRIC\n");
  if(Metrics != NULL){
    //PRINTF("DEBUG: INSERT_METRIC - IF\n");
    Metrics->last->next = NewMetric;
    Metrics->last = NewMetric;
  }
  else{
    //PRINTF("DEBUG: INSERT_METRIC - ELSE\n");
    Metrics = (struct MetricStructHead*)malloc(sizeof(struct MetricStructHead));
    Metrics->head = NewMetric;
    Metrics->last = NewMetric;
    Metrics->lastUsed = NULL;
  }
}
// /*---------------------------------------------------------------------------*/

void printMetric(){
  PRINTF("print");
  if(Metrics != NULL){
    struct MetricStruct *aux;
    aux = Metrics->head;
    PRINTF("EMM t:%d EMM: %f--",aux->time,aux->emm);
    aux = aux->next;
    PRINTF("EMM t:%d EMM: %f--",aux->time,aux->emm);
    aux = aux->next;
    PRINTF("EMM t:%d EMM: %f--",aux->time,aux->emm);
  }
  
}

void
count_metric(){
  if(Metrics != NULL){
    // if(linkaddr_node_addr.u8[0]+linkaddr_node_addr.u8[1]*10 == 5)
      // printMetric();
    int count = 0;
    struct MetricStruct *aux;
    for(aux = Metrics->head;aux != NULL ;aux = aux->next){
      count++;
    }
    PRINTF("DEBUG INFO: EMM > TOTAL: %d \n",count);
    // printMetric();
  }
  else
    PRINTF("DEBUG INFO: EMM > VECTOR NULL \n");
}