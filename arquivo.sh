arquivoResultados=resultados # arquivo onde os tempos serão armazenados
nomeExecutavel=a.out 	     # nome do executável
numeroTestes=5 		        # quantidade de testes a serem realizadas
arquivoHosts=hosts 	        # nome do arquivo de hosts
parametros='1024 768 18000'  # parâmetros (linhas, colunas, iterações)



echo resultados >$arquivoResultados
function executar() {
	awk 'NR <= '$2' { print }' < "$arquivoHosts" > tmphostfile
	echo número de processos: $1
	echo número de processos: $1.>> $arquivoResultados
	for((n=1;n<=$numeroTestes;n++)) do 
		echo executando com $1 processos e $2 nodos
		{ time mpirun -hostfile tmphostfile -np $1 $nomeExecutavel $parametros > /dev/null ; } 2>> $arquivoResultados
	done
}

executar 64 16;
executar 32 8;
executar 16 4;
executar 8 2;
executar 4 1;
executar 2 1;

