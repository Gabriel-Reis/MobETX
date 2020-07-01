#include <stdio.h>
#include <stdlib.h>

int main (){
  // int count = 0;
	char arqEntrada_path[30] =  "grm100.csv"; //entrace
	char arqSaidaDat_path[30] = "grm10.dat"; //cooja use
	char arqSaidaMov_path[50] = "../bonnmotion-3.0.1/bin/grm10.movements"; //bonnmotion use
	char arqSaidaParans_path[50] = "../bonnmotion-3.0.1/bin/grm10.params"; //bonnmotion use
	
	//BONNMOTION PARANS
	char model[10] = "SWIM";
	int NodeLimit = 10;
	float ignore = 3600;
	float randomSeed = 1592456788185;
	float espx = 1500.0;					//atualiza pelo maior valor de x
	float espy = 1500.0; 				//atualiza pelo maior valor de y
	float duration = 500.0;  			//atualiza pelo maior valor de tempo
	char circular[7] = "false";
	int J = 2; //2D
	float nodeRadius = 0.1;
	float cellDistanceWeight = 0.0;
	float nodeSpeedMultiplier = 1.0;
	float waitingTimeExponent = 0.0;
	float waitingTimeUpperBound = 0.0;
	
	FILE *arqEntrada;  
	FILE *arqSaidaDat;  
	FILE *arqSaidaMov;  
	FILE *arqSaidaParans;  

	//csv entrace
  	//<time> <id> <x coord> <y coord>
	arqEntrada = fopen(arqEntrada_path,"r");

  	// cooja use
  	// id time x y
	arqSaidaDat = fopen(arqSaidaDat_path,"w");

  	// Statistcs bonnmotion
  		// tempo x y (Nó por linha)
	arqSaidaMov = fopen(arqSaidaMov_path,"w");
		// parans
	arqSaidaParans = fopen(arqSaidaParans_path,"w");


	if(arqEntrada == NULL) {
		printf("Erro ao ler .csv\n");
	}
	else{
		printf("Aquivo lido com sucesso\n");

		int id,ctrl=0;
		double time,aux=-1,x,y;

		while(fscanf(arqEntrada, "%lf %d %lf %lf",&time,&id,&x,&y)!=EOF){
			if(id > NodeLimit)
				break;
			if (x > espx)
				espx = x;
			if (y > espy)
				espy = y;
			if (time > duration)
				duration = time;
			if(time > aux){
			//escreve .DAT
				fprintf(arqSaidaDat, "%d" , id);
				fprintf(arqSaidaDat, "%s" , " ");
				fprintf(arqSaidaDat, "%.2lf", time);
				fprintf(arqSaidaDat, "%s" , " ");
				fprintf(arqSaidaDat, "%.0lf", x);
				fprintf(arqSaidaDat, "%s" , " ");
				fprintf(arqSaidaDat, "%.0lf", y);
				fprintf(arqSaidaDat, "%s" , "\n");
			//escreve .mov
				if(ctrl != id)
					fprintf(arqSaidaMov, "%s" , "\n");
				fprintf(arqSaidaMov, "%.0lf", time);
				fprintf(arqSaidaMov, "%s" , " ");
				fprintf(arqSaidaMov, "%.0lf", x);
				fprintf(arqSaidaMov, "%s" , " ");
				fprintf(arqSaidaMov, "%.0lf", y);
				fprintf(arqSaidaMov, "%s" , " ");
				ctrl = id;
				aux = time;
			}
		}
		fprintf(arqSaidaParans, "%s" , "model=");
		fprintf(arqSaidaParans, "%s\n" , model);
		fprintf(arqSaidaParans, "%s" , "ignore=");
		fprintf(arqSaidaParans, "%.1lf\n" , ignore);
		fprintf(arqSaidaParans, "%s" , "randomSeed=");
		fprintf(arqSaidaParans, "%.0f\n" , randomSeed);
		fprintf(arqSaidaParans, "%s" , "x=");
		fprintf(arqSaidaParans, "%.1lf\n" , espx);
		fprintf(arqSaidaParans, "%s" , "y=");
		fprintf(arqSaidaParans, "%.1lf\n" , espy);
		fprintf(arqSaidaParans, "%s" , "duration=");
		fprintf(arqSaidaParans, "%.1lf\n" , duration);
		fprintf(arqSaidaParans, "%s" , "nn=");
		fprintf(arqSaidaParans, "%d\n" , NodeLimit);
		fprintf(arqSaidaParans, "%s" , "circular=");
		fprintf(arqSaidaParans, "%s\n" , circular);
		fprintf(arqSaidaParans, "%s" , "J=");
		fprintf(arqSaidaParans, "%dD\n" , J);
		fprintf(arqSaidaParans, "%s" , "nodeRadius=");
		fprintf(arqSaidaParans, "%.1lf\n" , nodeRadius);
		fprintf(arqSaidaParans, "%s" , "cellDistanceWeight=");
		fprintf(arqSaidaParans, "%.1lf\n" , cellDistanceWeight);
		fprintf(arqSaidaParans, "%s" , "nodeSpeedMultiplier=");
		fprintf(arqSaidaParans, "%.1lf\n" , nodeSpeedMultiplier);
		fprintf(arqSaidaParans, "%s" , "waitingTimeExponent=");
		fprintf(arqSaidaParans, "%.1lf\n" , waitingTimeExponent);
		fprintf(arqSaidaParans, "%s" , "waitingTimeUpperBound=");
		fprintf(arqSaidaParans, "%.1lf\n" , waitingTimeUpperBound);
	}
	fclose(arqEntrada);
	fclose(arqSaidaDat);
	fclose(arqSaidaMov);
	fclose(arqSaidaParans);
	return 0;
}