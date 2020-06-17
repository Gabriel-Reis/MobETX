The Contiki Operating System
============================

Contiki is an open source operating system that runs on tiny low-power
microcontrollers and makes it possible to develop applications that
make efficient use of the hardware while providing standardized
low-power wireless communication for a range of hardware platforms.

Contiki is used in numerous commercial and non-commercial systems,
such as city sound monitoring, street lights, networked electrical
power meters, industrial monitoring, radiation monitoring,
construction site monitoring, alarm systems, remote house monitoring,
and so on.

============================

Examples
Bonnmotion SWIM:
	./bm -f NOMEAQUI SWIM -n 10 -x 100 -y 100 -r 0.1 -m 1 -c 0.5 -e 0.45 -u 4 -d 1000
Bonnmotion Statistics:
	./bm Statistics -f NOMEAQUI -r 0.1
Contiki:
	ant run

Paths:
	Contiki:
		/contiki/tools/cooja
	Bonnmotion:
		/contiki/bonnmotion-3.0.1/bin