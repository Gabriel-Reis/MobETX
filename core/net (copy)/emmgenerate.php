<?php

# gera numero do nó com valores aleatórios para métricas e o tempo em ordem crescentes
$array = array(); //Inicializa Array
$time=1; //Time inicial
$NumberOfParans = 2; //Quantidade de parametros gerados
$NodeInitial = 2; //Nó inicial (1 não necessita de emm)
$NumberOfNodes = 10; //Quantidades de nós no array
$TimeOfSimulation = 3; //Em horas
$TimeOfSimulation *= 3600; //Transforma em segundos
$MaxVarOfTime= 5; //Valor máximo para variar entre linhas
$MaxVarOfParans= 20; //Valor máximo para parametros
$formater = 2; //Acrescenta zeors a esquerda para impressão
$print = false; //Define se imprime na tela o resultado
$header = "";
$printer = "";

//gera valores
for (;$time<$TimeOfSimulation; $time+=mt_rand(1,$MaxVarOfTime)) {
	$new = array();
	$new[] = mt_rand($NodeInitial, $NumberOfNodes); //NODE ID
	for($i=0; $i<$NumberOfParans;$i++){ //Params
		$new[] = mt_rand(1,$MaxVarOfParans);
	}
	$new[] = $time;
	array_push($array, $new);
}

//Gera Cabeçalho
$header .= "############### CABEÇALHO ###############\n";
$header .= "# Número de nós: \t\t\t\t\t".$NumberOfNodes."\t#\n";
$header .= "# Nó inicial: \t\t\t\t\t\t".$NodeInitial."\t#\n";
$header .= "# Tempo de simulação: \t\t\t\t".($TimeOfSimulation/3600)."h\t#\n";
$header .= "# Variação máxima de tempo: \t\t".$MaxVarOfTime."s\t#\n";
$header .= "# Quantidade de paramtros gerados: \t".$NumberOfParans."\t#\n";
$header .= "# Valor máximo para parametros: \t".$MaxVarOfParans."\t#\n";
$header .= "# Gerado em: \t\t".date('d/m/Y H:i:s')."\t#\n";
$header .= "# ID\tPARAM1\t\tPARAM2\t\tTIME\t#\n";
$header .= "#########################################\n";

//Gera String de dados
foreach ($array as $key => $value) {
	$space = 1;//Impede espaço no fim da linha
	foreach ($value as $keyIn => $valueIn) {
		$printer .= str_pad($valueIn, $formater, "0", STR_PAD_LEFT);
		if($space < 2+$NumberOfParans)
			$printer.=" ";
		$space++;
	}
	$printer .= "\n";
}

//Imprime ou não na tela
if($print)
	echo str_replace("\n", "<br>", $printer);
else
	echo "DONE!";

//imprime em emm.txt
$name = 'emm.txt';
$file = fopen($name, 'w');
fwrite($file, $header);
fwrite($file, rtrim($printer,"\n"));
fclose($file);

?>
