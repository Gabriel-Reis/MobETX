#include <stdio.h>
#include <stdlib.h>

int main (){
  // int count = 0;
	char arqEntrada_path[30] =  "grm100.csv";
	char arqSaidaDat_path[30] = "grm100.dat";
	char arqSaidaMov_path[30] = "grm100.movements";
	int NodeLimit = 10;
	
	FILE *arqEntrada;  
	FILE *arqSaidaDat;  
	FILE *arqSaidaMov;  

	//csv entrace
  	//<time> <id> <x coord> <y coord>
	arqEntrada = fopen(arqEntrada_path,"r");
  	
  	// cooja use
  	// id time x y
	arqSaidaDat = fopen(arqSaidaDat_path,"w");
  	
  	// Statistcs bonnmotion
  	// tempo x y (Nó por linha)
	arqSaidaMov = fopen(arqSaidaMov_path,"w");
	
	if(arqEntrada == NULL) {
		printf("Erro ao ler .csv\n");
	}
	else{
		printf("Aquivo lido com sucesso\n");

		int id,ctrl=0;
		double time,x,y;

		while(fscanf(arqEntrada, "%lf %d %lf %lf",&time,&id,&x,&y)!=EOF){
			if(id > NodeLimit)
				break;
			//escreve .DAT
			fprintf(arqSaidaDat, "%d" , id);
			fprintf(arqSaidaDat, "%s" , " ");
			fprintf(arqSaidaDat, "%.0lf", time);
			fprintf(arqSaidaDat, "%s" , " ");
			fprintf(arqSaidaDat, "%lf", x);
			fprintf(arqSaidaDat, "%s" , " ");
			fprintf(arqSaidaDat, "%lf", y);
			fprintf(arqSaidaDat, "%s" , "\n");
			//escreve .mov
			if(ctrl != id)
				fprintf(arqSaidaMov, "%s" , "\n");
			fprintf(arqSaidaMov, "%lf", time);
			fprintf(arqSaidaMov, "%s" , " ");
			fprintf(arqSaidaMov, "%lf", x);
			fprintf(arqSaidaMov, "%s" , " ");
			fprintf(arqSaidaMov, "%lf", y);
			fprintf(arqSaidaMov, "%s" , " ");
			ctrl = id;
		}
	}
	fclose(arqEntrada);
	fclose(arqSaidaDat);
	fclose(arqSaidaMov);
	return 0;
}