#!/usr/bin/env python3

import sys
import grpc
import socket
import threading
import time
from concurrent import futures

# Importar stubs gerados
import pares_pb2
import pares_pb2_grpc
import central_pb2
import central_pb2_grpc

class ParesServicer(pares_pb2_grpc.ParesServicer):
    """Implementação do serviço gRPC do servidor de pares."""

    def __init__(self, port, central_addr=None):
        self.storage = {}
        self.lock = threading.Lock()
        self.server_ref = None  # Referência ao servidor gRPC para o Termino
        self.port = port
        self.central_addr = central_addr # Endereço do servidor central (Parte 2)

    def Insercao(self, request, context):
        with self.lock:
            ret_val = 1 if request.chave in self.storage else 0
            self.storage[request.chave] = request.valor
        return pares_pb2.MsgRetorno(retorno=ret_val)

    def Consulta(self, request, context):
        with self.lock:
            valor = self.storage.get(request.chave, "")
        return pares_pb2.MsgValor(valor=valor)

    def Ativacao(self, request, context):
        # Parte 1: central_addr é None, não faz nada
        if self.central_addr is None:
            return pares_pb2.MsgRetornoAtivacao(retorno=0)

        # Parte 2: Conectar ao servidor central e registrar chaves
        with self.lock:
            keys_list = list(self.storage.keys())

        try:
            # Obter o FQDN (Fully Qualified Domain Name) como instruído
            my_host = socket.getfqdn()
            my_addr = f"{my_host}:{self.port}"

            with grpc.insecure_channel(self.central_addr) as channel:
                stub = central_pb2_grpc.CentralStub(channel)
                req = central_pb2.MsgRegistro(localizador=my_addr, chaves=keys_list)
                response = stub.Registro(req)
                return pares_pb2.MsgRetornoAtivacao(retorno=response.retorno)
        except grpc.RpcError as e:
            # Em caso de falha na conexão, retorna -1 (comportamento não especificado, mas razoável)
            return pares_pb2.MsgRetornoAtivacao(retorno=-1)

    def Termino(self, request, context):
        with self.lock:
            key_count = len(self.storage)

        # Função para parar o servidor após um breve delay
        # Isso dá tempo para a resposta ser enviada antes do shutdown
        def stop_server():
            time.sleep(0.1)
            if self.server_ref:
                self.server_ref.stop(0) # 0 = shutdown não gracioso imediato

        threading.Thread(target=stop_server).start()
        return pares_pb2.MsgRetornoTermino(retorno=key_count)

def serve():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Uso: ./svc_par.py <porta> [localizador_serv_central]")
        sys.exit(1)

    port = sys.argv[1]
    central_addr = None
    
    # Comportamento da Parte 2 é ativado com 2 argumentos
    if len(sys.argv) == 3:
        central_addr = sys.argv[2]

    # Usar '0.0.0.0' para aceitar conexões de qualquer interface
    server_addr = f'0.0.0.0:{port}'
    
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    servicer = ParesServicer(port, central_addr)
    servicer.server_ref = server  # Passa a referência do servidor para o servicer

    pares_pb2_grpc.add_ParesServicer_to_server(servicer, server)
    
    server.add_insecure_port(server_addr)
    server.start()
    
    # Espera bloqueante até que o servidor seja terminado (via RPC Termino)
    server.wait_for_termination()

if __name__ == '__main__':
    serve()