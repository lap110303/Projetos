#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* MARK NAME Lucas Ferreira Pedras - 2021030835 */
/* MARK NAME Lucas Affonso Pires - 2023028420 */

/****************************************************************
 * Shell xv6 simplificado
 *
 * Este codigo foi adaptado do codigo do UNIX xv6 e do material do
 * curso de sistemas operacionais do MIT (6.828).
 ***************************************************************/

#define MAXARGS 10

/* Todos comandos tem um tipo.  Depois de olhar para o tipo do
 * comando, o código converte um *cmd para o tipo específico de
 * comando. */
struct cmd {
  int type; /* ' ' (exec)
               '|' (pipe)
               '<' or '>' (redirection)
               'S' (subshell) */
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // argumentos do comando a ser exec'utado
};

struct redircmd {
  int type;          // < ou > 
  struct cmd *cmd;   // o comando a rodar (ex.: um execcmd)
  char *file;        // o arquivo de entrada ou saída
  int mode;          // o modo no qual o arquivo deve ser aberto
  int fd;            // o número de descritor de arquivo que deve ser usado
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // lado esquerdo do pipe
  struct cmd *right; // lado direito do pipe
};

struct subcmd {
  int type;          // 'S' subshell
  struct cmd *cmd;   // comando dentro dos parênteses
};

int fork1(void);  // Fork mas fechar se ocorrer erro.
struct cmd *parsecmd(char*); // Processar o linha de comando.

/* Executar comando cmd.  Nunca retorna. */
__attribute__((noreturn)) void runcmd(struct cmd *cmd)
{
  int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;
  struct subcmd *scmd;

  if(cmd == 0)
    exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "tipo de comando desconhecido\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(0);
    /* MARK START task2
     * TAREFA2: Implemente código abaixo para executar
     * comandos simples. */
    execvp(ecmd->argv[0], ecmd->argv);
    /* Se execvp retornar, ocorreu erro */
    perror("exec");
    exit(1);
    /* MARK END task2 */
    break;

  case '>':
  case '<':
    rcmd = (struct redircmd*)cmd;
    /* MARK START task3
     * TAREFA3: Implemente codigo abaixo para executar
     * comando com redirecionamento. */
    // 0644 = -rw-r--r--
    close(rcmd->fd);
    r = open(rcmd->file, rcmd->mode, 0644);

    if (r >= 0) {
      /* redirecionamento pronto, executa o comando que está encapsulado */
      runcmd(rcmd->cmd);
    }

    fprintf(stderr, "Não foi possível abrir o arquivo: %s\n", rcmd->file);
    exit(1);
    /* MARK END task3 */
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    /* MARK START task4
     * TAREFA4: Implemente codigo abaixo para executar
     * comando com pipes. */
    if (pipe(p) < 0) {
      fprintf(stderr, "Não foi possível criar um PIPE\n");
      exit(3);
    }
    
    // Após o pipe(p)
    // o que a gente escrever em p[1] pode ser lido em p[0]

    switch (fork()) {
      case -1:
        fprintf(stderr, "Não foi possível criar um fork do processo do shell\n");
        exit(2);
        break;
      
      // Filho executa comando esquerdo
      case 0:
        close(STDOUT_FILENO);       
        dup(p[1]);      // STDOUT do lado esquerdo eh p[1]
        close(p[1]);
        close(p[0]);

        runcmd(pcmd->left);         
        break;

      // Pai executa comando direito
      default:
        close(STDIN_FILENO);
        dup(p[0]);      // STDIN do lado direito eh p[0]
        close(p[0]);
        close(p[1]);

        runcmd(pcmd->right);
        break;
    }
  
    /* MARK END task4 */
    break;

  case 'S':
    scmd = (struct subcmd*)cmd;
    /* Subshell: devemos executar o comando interno em um processo filho
       para isolar efeitos (por exemplo redirecionamentos) do processo pai. */
    if (fork1() == 0) {
      runcmd(scmd->cmd);
      /* runcmd não retorna, mas por segurança: */
      exit(0);
    }
    /* pai espera o subshell terminar */
    wait(&r);
    break;
  }   
   
  exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  if (isatty(fileno(stdin)))
    fprintf(stdout, "$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int r;

  // Ler e rodar comandos.
  while(getcmd(buf, sizeof(buf)) >= 0){
    /* MARK START task1 */
    /* TAREFA1: O que faz o if abaixo e por que ele é necessário?
     * Insira sua resposta no código e modifique o fprintf abaixo
     * para reportar o erro corretamente. */

    // Resposta:
    // Implementa o comando 'cd'. 
    // Não existe um binário chamado 'cd' que precisa ser executado para mudar de diretório, em vez disso
    // a chamada de sistema chdir muda o diretório do processo atual, que é o próprio shell.
    // O if verifica se foi digitado 'cd ' para entrar no primeiro 'if'.
    // Após isso, o ultimo caractere do buffer é definido como 0, para garantir o fim de string.
    // Depois é verificado abaixo se a troca para o diretorio especificado é valida (deve retornar 0). 
    // Assim, o erro deve reportar que não foi possível ir para o diretorio especificado.

    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
      buf[strlen(buf)-1] = 0;
      
      if(chdir(buf+3) < 0)
        fprintf(stderr, "cd: %s: caminho ou diretório não encontrado. O diretório desejado deve ser um caminho válido não vazio.\n", buf+3);
      continue;
    }
    /* MARK END task1 */

    /* Extras implementados: suporte para execução em background com '&' no fim da linha
       e comando built-in 'wait' que aguarda filhos. */

    /* Verifica se o comando é apenas 'wait' (possivelmente com '\n'). 
       Se sim, aguarda por todos os filhos existentes e continua. */
    if (strncmp(buf, "wait", 4) == 0 && (buf[4] == '\n' || buf[4] == '\0' || buf[4] == ' ')) {
      /* Espera por todos os filhos que ainda não foram colhidos */
      while (waitpid(-1, &r, 0) > 0) ;
      continue;
    }

    /* Suporte simples para execução em background: se o último caractere
       antes do '\n' for '&', roda sem esperar. */
    int background = 0;
    int len = strlen(buf);
    if (len > 0) {
      /* remover espaços finais */
      int i = len - 1;
      while (i >= 0 && (buf[i] == '\n' || buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\r')) i--;
      if (i >= 0 && buf[i] == '&'){
        background = 1;
        buf[i] = '\n'; // substitui '&' por nova linha terminadora
        buf[i+1] = '\0';
      }
    }

    if(fork1() == 0)
      runcmd(parsecmd(buf));

    if (!background)
      wait(&r);
    else {
      /* Background: não espera pelo filho. O usuário poderá executar 'wait' depois
         para aguardar por processos em background ou o sistema irá reaplicar os
         filhos eventualmente quando o processo terminar (podem ficar zumbis até lá). */
      /* opcional: print pid do filho poderia ser mostrado, mas o enunciado pede para
         não imprimir nada além da saída dos programas, então omitimos. */
    }
  }
  exit(0);
}

int
fork1(void)
{
  int pid;
  
  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

/****************************************************************
 * Funcoes auxiliares para criar estruturas de comando
 ***************************************************************/

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
subshellcmd(struct cmd *c)
{
  struct subcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = 'S';
  cmd->cmd = c;
  return (struct cmd*)cmd;
}

/****************************************************************
 * Processamento da linha de comando
 ***************************************************************/

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>()"; /* adicionado '(' e ')' */

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '<':
    s++;
    break;
  case '>':
    s++;
    break;
  case '(':
  case ')':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

/* Copiar os caracteres no buffer de entrada, comeando de s ate es.
 * Colocar terminador zero no final para obter um string valido. */
char 
*mkcopy(char *s, char *es)
{
  int n = es - s;
  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  /* Primeiro, suportar explicitamente subshells começando com '('.
     Usamos peek/gettoken para consumir '(' e depois parseline para
     construir o comando interno. Isso evita erros de sintaxe ao
     encontrar parênteses. */
  if (peek(ps, es, "(")) {
    /* consome '(' */
    gettoken(ps, es, 0, 0);
    /* parseia o conteúdo dentro dos parênteses */
    struct cmd *sub = parseline(ps, es);
    /* agora o próximo token deve ser ')' */
    if (!peek(ps, es, ")")) {
      fprintf(stderr, "missing )\n");
      exit(-1);
    }
    /* consome ')' */
    gettoken(ps, es, 0, 0);
    /* cria nó de subshell e permite redirecionamentos após ele */
    ret = subshellcmd(sub);
    ret = parseredirs(ret, ps, es);
    return ret;
  }
  
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      /* se encontrar '(' aqui, tratávamos antes; se chegou aqui, é erro */
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}

// vim: expandtab:ts=2:sw=2:sts=2
