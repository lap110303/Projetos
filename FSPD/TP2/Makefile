# Makefile para o Exercício de Armazenamento Chave-Valor Distribuído (V3)
# Versão silenciosa (usa @ para suprimir o eco dos comandos)

# Define o interpretador Python
PYTHON = python3

# Define o compilador de protoc do gRPC
PROTOC = $(PYTHON) -m grpc_tools.protoc

# Arquivos .proto
PROTOS_PARES = pares.proto
PROTOS_CENTRAL = central.proto

# Arquivos .py gerados (stubs)
STUBS_PARES = pares_pb2.py pares_pb2_grpc.py
STUBS_CENTRAL = central_pb2.py central_pb2_grpc.py

# Arquivos de origem Python
SVC_PAR = svc_par.py
CLN_PAR = cln_par.py
SVC_CEN = svc_cen.py
CLN_CEN = cln_cen.py

# Regra padrão: garantir que os stubs sejam criados
all: stubs

# --- Regras de Geração e Limpeza ---

# Regra para gerar os stubs
.PHONY: stubs
stubs: $(STUBS_PARES) $(STUBS_CENTRAL)

$(STUBS_PARES): $(PROTOS_PARES)
	@echo "Gerando stubs para pares..."
	@$(PROTOC) -I. --python_out=. --grpc_python_out=. $(PROTOS_PARES)

$(STUBS_CENTRAL): $(PROTOS_CENTRAL)
	@echo "Gerando stubs para central..."
	@$(PROTOC) -I. --python_out=. --grpc_python_out=. $(PROTOS_CENTRAL)

# Regra para limpar arquivos gerados
.PHONY: clean
clean:
	@echo "Limpando arquivos gerados..."
	@rm -f $(STUBS_PARES) $(STUBS_CENTRAL)
	@rm -f *_pb2.py *_pb2_grpc.py

# --- Regras de Execução (conforme enunciado) ---

.PHONY: run_cli_pares run_serv_pares_1 run_serv_pares_2 run_serv_central run_cli_central run_super_par

# Garante que os stubs existam antes de tentar executar
run_cli_pares: stubs $(CLN_PAR)
	@./$(CLN_PAR) $(arg)

run_serv_pares_1: stubs $(SVC_PAR)
	@./$(SVC_PAR) $(arg)

run_serv_pares_2: stubs $(SVC_PAR)
	@./$(SVC_PAR) $(arg1) $(arg2)

run_serv_central: stubs $(SVC_CEN)
	@./$(SVC_CEN) $(arg)

run_cli_central: stubs $(CLN_CEN)
	@./$(CLN_CEN) $(arg)

run_super_par: stubs $(SVC_CEN)
	# O segundo argumento só precisa existir
	@./$(SVC_CEN) $(arg) super