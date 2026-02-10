<!-- LTeX: language=pt-BR -->

# PAGINADOR DE MEMÓRIA -- RELATÓRIO

1. Termo de compromisso

    Ao entregar este documento preenchido, os membros do grupo afirmam que todo o código desenvolvido para este trabalho é de autoria própria. Exceto pelo material listado no item 3 deste relatório, os membros do grupo afirmam não ter copiado material da Internet nem ter obtido código de terceiros.

2. Membros do grupo e alocação de esforço

    Preencha as linhas abaixo com o nome e o email dos integrantes do grupo.  Substitua marcadores `XX` pela contribuição de cada membro do grupo no desenvolvimento do trabalho (os valores devem somar 100%).

    * Lucas Affonso Pires <lap110303@gmail.com> 33.4%
    * Lucas Ferreira Pedras <lucaspedras@dcc.ufmg.br> 33.3%
    * João Fernando Menezes Mauro <jfmenezes@ufmg.br> 33.3%

3. Referências bibliográficas

- Definição das structs foi retirada de: https://gitlab.dcc.ufmg.br/cunha-dcc605/mempager-assignment/-/blob/master/src/pager.c

4. Detalhes de implementação

    1. Descreva e justifique as estruturas de dados utilizadas em sua solução.
    2. Descreva o mecanismo utilizado para controle de acesso e modificação às páginas.

1. As estruturas de dados utilizadas foram disponibilizadas na descrição do trabalho, envolvendo as estruturas 'pager', 'proc', 'frame_data' e 'page_data'. Falando especificamente da estrutura 'pager', dentro dela existem elementos fundamentais para o trabalho, como o 'pid2proc' e 'block2pid' que fazem a relação entre blocos e processos utilizando o pid como intermediário, além da estrutura 'frames' que foi utilizada no controle dos frames disponíveis na memória física.

A estrutura 'frames' é importante na inicialização da memória (função pager_init) em que sua estrutura é iniciada com valores padrão (sem proteção, não está associada a nenhuma página...), sendo utilizada tamém na função pager_fault quando é necessário uma nova alocação de frames a um processo, em que seus valores são atualizados, associando então um novo frame a ser utilizado para o processo que gerou a falha de página. Caso a falha tenha sido não por falta de frames, mas por falta de permissões, a estrutura é novamente utilizada para então ter suas permissões atualizadas no processo (alterando as permissões para leitura, ou leitura e escrita - além de assinar que a página está 'suja', sinalizando que esse endereço já foi escrito anteriormente além de auxiliar no 'clock_algorithm' explicado mais adiante). A estrutura é usada também na função pager_destroy, quando o processo chega ao fim, sendo necessário alocar o frame que foi utilizado, com seus valores sendo reinicializados.

Já as estruturas 'pid2proc' e 'block2pid' controlam então o mapeamento entre processo e bloco no disco. Elas são utilizadas durante a inicialização, em que ainda não existem processos mapeados. Durante a criação de um novo processo em pager_create, pid2proc é inicializada com o novo processo na posição referente ao pid do processo. Na função pager_extend, ao alocar um novo espaço na memória para o processo, as informações em pid2proc são agora utilizadas para assinalizar um bloco na estrutura block2pid a esse mesmo processo (utilizando o pid como intermediario da operação). Tal operçaão é realizada também em pager_destroy, utilizando esse mapeamento para liberar o bloco de um processo que foi finalizado (todo processo possui um bloco associado, mesmo que não for utilizado).

As demais estruturas são utilizadas também no código, como os valores da estrutura 'pager' sendo constantemente atualizadoas nas funçoes, visto que possui valores importantes como o mutex para sincronização, além de informações sobre os frames e blocos disponíveis, juntamente com o estado do algoritmo de reposição de páginas (clock = ultima posição da memória analisada na ultima execução). A estrutura proc também é atualizada constantemente para informar quando o processo sai da memória, além de informações sobre suas páginas e blocos.

Descreva o mecanismo utilizado para controle de acesso e modificação às páginas.
2. Assim como explicado anteriormente, as estruturas presentes no arquivo auxiliaram em todo o gerenciamento dos quadros e blocos a serem alocados entre os processos. Em geral, as estruturas pid2proc e block2pid foram muito utilizadas para coordenar os acessos e modificações da memória dos processos como dito na questão acima. Explicando agora sobre como cada função desenvolvida realiza essas tarefas (vale destacar que todas as funções implementam mutex para controlar corretamente o acesso):
- pager_init = Inicializa e aloca memória para as estruturas necessárias na execução do processo com base do número de blocos e de frames informados. Dessa forma, a estrutura pager é atualizada, tendo os valores de memória utilizados pelo processo, as estruturas pid2block e block2pid são iniciadas e os frames da estrutura pager que serão utilizados são inicializados.
- pager_create = Ao criar o processo, sua memória é alocada, sendo definidos o seu pid, o número de páginas e o máximo que pode ser utilizado. Desse modo, a estrutura pid2proc é atualizada para receber o novo processo criado.
- pager_extend = A função aloca mais memória a um processo dado o seu pid. Assim, as informações do processo são recuperadas na estrutura pid2proc e então é buscado um bloco livre para ser alocado (caso nenhum esteja disponível, a função é finalizada). Outra verificação realizada diz respeito ao número de paginas utilizadas pelo processo, que deve ser menor do que o máximo de páginas definido anteriormente. Assim, é atribuido então um novo bloco ao processo, que recebe 'on_disk' como 0 pois nada ainda foi gravado e a estrutura block2pid é atualizada agora que o processo possui um bloco associado. Ao fim, o endereço virtual da nova página de memória.
- pager_fault = Ao ocorrer uma falha de página, primeiramente são lidas as informações do processo em que a falha ocorreu. Assim, verifica-se então se a página é válida (menor que o número de total de páginas). Após isso, cabe analisar o motivo da falha de página:
    - Se ocorreu por conta da página não estar no disco, então é alocado um novo frame a ser utilizado (que pode ser um dos frames livres, ou então algum frame que foi liberado com o algoritmo de reposição - clock algorithm). Assim, a estrutura dos frames é atualizada em relação ao processo que está envolvido e os dados são carregados do disco para o frame (mmu_disk_read) se a página estiver no disco, caso contrário, a página é preenchida com 0 (mmu_zero_fill). Por fim, a página é marcada como residente.
    - Caso a falha ocorreu por falta de permissões, então ocorre a alteração interna (atualizando a estrutura de frames, caso não haja permissão agora é possível realizar a leitura, e caso contrário agora pode ser feita leitura/escrita). Por fim, as novas permissões agora são alteradas no MMU (mmu_chprot)
- pager_syslog = Função utilizada para printar mensagens sobre acessos a memória. Primeiramente são lidas as informações do processo atual utilizando a estrutura pid2block e seguindo as ações caso o processo for != Nulo. Após isso, é verificado se o endereço da página inicial do processo é valido, ou seja, se não é negativo, maior que o número de páginas disponível ao processo ou maior que a memória disponível ao processo. É então inciado o processo de copiar a memória residente (frame presente na memória física) para o buffer alocado. Após isso, basta printar o resultado.
- pager_destroy = A memória do processo finalizado é liberada. Apenas é lido a informação do processo finalizado (o processo deve existir) e então liberar cada bloco e frame alocado ao processo. Por fim, a própria estrutura do processo também é liberada.

* Funções (e macros) auxiliares:
- PAGE_SIZE = Retorna o tamanho da página
- ADDR_TO_PAGE e PAGE_TO_ADDR = Atuam convertendo páginas para endereços e vice-versa. É utilizado aritmetica entre os endereços base e o tamanho das páginas para realizar o calculo do offset e realizar a conversão corretamente.
- pid2idx = Função auxiliar para encontrar o indice de determinado pid na estrutura pid2proc. Como o pid pode ser um valor arbitrário, não seguindo a indexação 0,1...., essa função é necessária para utilizar sequencialmente as posições das estruturas.
- clock_algorithm = Função que implementa a política de reposição, percorrendo os frames e aplicando a ideia da segunda chance para então retornar um frame livre (e enviando o frame anterior para o disco).
- get_free_frame = Função auxiliar que retorna um frame livre para ser alocado. Automaticamente aplica o algoritmo da segunda chance se necessário.
- update_frame_permissions = Função auxiliar para retirar ou dar permissões totais a um frame.
