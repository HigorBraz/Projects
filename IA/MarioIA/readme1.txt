nome: Higor Braz da Silva
ra: 11201810691
turno: diurno


O algoritmo usado é o reinforcement learning.
basicamente preenchi uma matriz estado x acao onde tabela[estado][acao] guarda o valor daquela acao naquele estado. 
Para calcular os valores foi usado uma recompensa.

segui a ideia decrita nesse artigo: https://www.learndatasci.com/tutorials/reinforcement-q-learning-scratch-python-openai-gym/?fbclid=IwAR0Q0jTMT8_ZlngO9L-wNy1PSewkzRsZASuAZnPsGQlrvWO-sRc9me7PWP8

Infelizmente não consegui fazer ele treinar todos os estados possiveis do melhor agente (meu computador não é tão bom então ele trava muito quando a tabela esta muito grande)
 então as vezes no play.py ele vai pular pra um estado que não treinou ou que os custos das açoes estao em 0 e ai empata, dai ele faz uma ação aleatoria que acaba por levar
 a outros estados que nao foram treinados. Portanto o play do melhor agente pode acabar não sendo bom, sendo necessario ele jogar mais algumas vezes, por isso eu deixo ele
 executando ate terminar a fase e não apenas uma vez. Peço, por gentileza, a compreensão, muito obrigado. (porem o melhor agente esta bem treinado entao em 80% dos casos ele termina a fase).

O agente treinado esta guardado em Q1.
O novo agente sera guardado em Q2.

as bibliotecas usadas são rominfo e utils (disponibilizadas pelo professor)

para treinar o agente: 
py train.py best - treina o agente ja treinado
py train.py train - treina um agente novo

para jogar:
py play.py best - joga o agente ja treinado
py play.py train - joga o agente novo

link do video: https://www.youtube.com/watch?v=uaLiOP8y9Yk&ab_channel=HigorBraz

ps: não consegui gravar a execução dos algoritmos :/