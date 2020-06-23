packetsReceived = new Array();
packetsSent = new Array();
sinkID = 101;
nodeCount = 101;
totalPRR_UP = 0;
totalPRR_DOWN = 0;

collectBeacon = new Array();
matrixbeacon = new Array();
downwardsDefaultRoute = new Array();
temporaryRoutes = new Array();
nodeIP = new Array();
ipRecv = 0
sinkRecv = 0;
downRecv = 0;
totalBeacons = 0;
totalSent = 0;
dropAwayHL = 0;
dropTmpTableNextHopAway = 0;
dropNextHopIPtreeMoved = 0;
dropDueCTP = 0;
dropDueQueueTimedOut = 0;

logMSGDATALatency = "";

lastCollectStats = new Array();

//TIMEOUT(5400000, printOutput()); // CRWP
//TIMEOUT(1296000000, printOutput()); // GRM-MIT
TIMEOUT(950400000, printOutput()); // GRM-Cambridge
//TIMEOUT(259200000, printOutput()); // GRM-Inf06


printOutput = function(){
	// HEADER
	log.log("sent, beacons, sinkRecv, downRecv, pprUp, pprDown, ipRecv, dropAwayHL, dropTmpTableNextHopAway, dropNextHopIPtreeMoved, dropDueCTP, dropDueQueueTimedOut, tempRoutes, defaultRoutes, lastCollectStats\n");
	
	log.log(totalSent +", ");
	log.log(totalBeacons +", ");
	log.log(sinkRecv +", ");
	log.log(downRecv +", ");
	log.log(sinkRecv/totalSent +", ");
	log.log(downRecv/sinkRecv +", ");
	log.log(ipRecv +", ");
	log.log(dropAwayHL +", ");
	log.log(dropTmpTableNextHopAway +", ");
	log.log(dropNextHopIPtreeMoved +", ");
	log.log(dropDueCTP +", ");
	log.log(dropDueQueueTimedOut +", ");
	log.log("NA, ");
	log.log("NA, ");
	log.log("NA\n");
	doloop(temporaryRoutes, downwardsDefaultRoute, lastCollectStats);
	//log.log(logMSGDATALatency);
	log.testOK();
	/*log.log("temporaryRoutes:\n");
	doloop(temporaryRoutes);
	log.log("downwardsDefaultRoute:\n");
	doloop(downwardsDefaultRoute);
	log.log("TOTAL sent "+ totalSent +"\n");
	log.log("TOTAL beacons "+ totalBeacons +"\n");
	log.log("TOTAL sinkRecv "+ sinkRecv +"\n");
	log.log("TOTAL downRecv "+ downRecv +"\n");
	log.log("PRR_DOWN "+ downRecv/sinkRecv +"\n");
	log.log("TOTAL ipRecv "+ ipRecv +"\n");*/
}

doloop = function(vet1, vet2, vet3){
	for(i = 1; i <= nodeCount; i++){
		log.log("NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, ");
		log.log(vet1[i] + ", "+ vet2[i] + ", " + vet3[i] + "\n");
	}
}

for(i = 0; i <= nodeCount; i++) {
	packetsReceived[i] = 0;
	packetsSent[i] = 0;
	collectBeacon[i] = 0;
	matrixbeacon[i] = 0;
	downwardsDefaultRoute[i] = 0;
	temporaryRoutes[i] = 0;
	nodeIP[i] = 0;
    ipRecv = 0;
}

while(1) {
	YIELD();

	//log.log(time + ":" + id + ":" + msg + "\n");

	if(msg.contains("IP RECEIVED")){
		nodeIP[msg.split(" ")[3]] = id;
        ipRecv++;
		//log.log(msg.split(" ")[3]);
		//log.log("\n");
	}
	if(msg.contains("DATA")){
		//logMSGDATALatency += id + " " + time + " " + msg + "\n";
		log.log(id + " " + time + " " + msg + "\n");
	}
	if(msg.contains("MATRIX SINK DATA")){
		sinkRecv++;
	}
	if(msg.contains("MATRIX NODE DATA")){
		packetsReceived[id]++;
		downRecv++;
	}
	if(msg.contains("MATRIX NODE SEND DATA")){
		packetsSent[id]++;
		totalSent++;
	}
	if(msg.contains("dest is away from HL")){
		dropAwayHL++;
	}
	if(msg.contains("node in temporary table, but either dest or nexthop is away from HL.")){
		dropTmpTableNextHopAway++;
	}
	if(msg.contains("nexthop in the IPtree moved")){
		dropNextHopIPtreeMoved++;
	}
	if(msg.contains("drop due to CTP")){
		dropDueCTP++;
	}
	if(msg.contains("removing queued packet due to timeout")){
		dropDueQueueTimedOut++;
	}
	if(msg.contains("sending neighbor advertisement with val") || 
		msg.contains("beacon")){
		collectBeacon[id]++;
		totalBeacons++;
	}
	if(msg.contains("matrix_route_tmp_num")){
		if(temporaryRoutes[id] < msg.split(" ")[1]){
			temporaryRoutes[id] = msg.split(" ")[1];
		}
	}
	if(msg.contains("matrix_route:")){
		if(downwardsDefaultRoute[id] < msg.split(" ")[1]){
			downwardsDefaultRoute[id] = msg.split(" ")[1];
		}
	}
	if(msg.contains("collect stats")){
		tmp = msg;
		tmp = tmp.replace("collect stats ", "");

		lastCollectStats[id] = tmp;
	}


	/*
	msgArray = msg.split(' ');
	if(msgArray[0].equals("DATA")) {
		if(msgArray.length == 9) {
			// Received packet
			senderID = parseInt(msgArray[8]);
			packetsReceived[senderID]++;

			log.log("SenderID " + senderID + " PRR " + packetsReceived[senderID] / packetsSent[senderID] + "\n");
			totalReceived = totalSent = 0;
			for(i = serverID + 1; i &lt;= nodeCount; i++) {
				totalReceived += packetsReceived[i];
				totalSent += packetsSent[i];
			}
			totalPRR = totalReceived / totalSent;
			log.log("Total PRR " + totalPRR + " recv " + totalReceived + " sent " + totalSent + "\n");
		} else if(msgArray.length == 6) {
			// Sent packet
			packetsSent[id]++;
		}
	}*/
}	