#  MOBETX
## Fórmula do MOBETX (ATUAL)
"
	VisitTime + LinkDuration
"

#  Fórmula ETX (Contiki)
## Fórmula que o contiki usa para calcular a ETX
"
	((tx * COLLECT_LINK_ESTIMATE_UNIT) * COLLECT_LINK_ESTIMATE_ALPHA + le->etx_accumulator * (COLLECT_LINK_ESTIMATE_UNIT - COLLECT_LINK_ESTIMATE_ALPHA))
	/ COLLECT_LINK_ESTIMATE_UNIT;
	----->>>>>
	( (Nº transmissões * 8) * alpha + etx_accumulator * (8-alpha) )  /  8
	----->>>>>
	((Nº transmissões * 8) * 3 + etx_accumulator * (8-3))  / 8
	----->>>>>
	( (Nº transmissões * 24) + etx_accumulator * 5) / 8
	----->>>>>
	( ( [1;255] * 24) + etx_accumulator * 5) / 8
	----->>>>>
	( [24;6120] + etx_accumulator * 5) / 8

	tx = The number of times the packet was transmitted before it was given up on
	unit = 8 
		defined on collect-link-estimate.h (59)
	alpha = 3
		defined on collect-link-estimate.c (50)
"


#  new rtmetric 	
## Resultado atual do ETX ou (ValorInicial * Unidade)
"
	tc->rtmetric (Collect.c 448)->
	rtmetric_compute (Collect.c 235) ->   Descobre melhor vizinho ->  
	collect_neighbor_rtmetric_link_estimate (Collect-neighbor.c) ->  Métrica dovizinho + -> 
	collect_link_estimate (Collect-link-estimate.c) -> 
		Se nunca tentou estimer (base * Unit)  
		OU ACUMULADOR 
"


#  #a
## ETX do PAI

#  uint32_t
## Is a numeric type that guarantees 32 bits, the value is unsigned, meaning that the range of values goes from 0 to 2^32-1.