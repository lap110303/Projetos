#!/usr/bin/env python3

import sys
import grpc
import socket
import threading
import time
from concurrent import futures

# Importar stubs gerados
import central_pb2
import central_pb2_grpc

class CentralServicer(central_pb2_grpc.CentralServicer):
    """Implementação do serviço gRPC do servidor central."""

    def __init__(self, port, is_super_peer, my_addr):
        self.key_map = {}  # Dicionário: chave (int) -> localizador (str)
        self.peers = []    # Lista de localizadores de super-pares (Parte 3)
        self.lock = threading.Lock()
        self.server_ref = None
        self.is_super_peer = is_super_peer
        self.my_addr = my_addr # Endereço próprio (ex: "ganges.grad...:6666")

    def Registro(self, request, context):
        count = 0
        with self.lock:
            for chave in request.chaves:
                # Sobrescreve se a chave já existir
                self.key_map[chave] = request.localizador
                count += 1
        return central_pb2.MsgRetornoRegistro(retorno=count)

    def Pareamento(self, request, context):
        # Parte 2: Não faz nada
        if not self.is_super_peer:
            return central_pb2.MsgRetornoPareamento(retorno=0)

        # Parte 3: Adiciona o par à lista de peers
        ret_val = 0
        with self.lock:
            if request.localizador not in self.peers:
                self.peers.append(request.localizador)
            ret_val = len(self.peers) # Exemplo da P3 sugere retornar o count
            
        return central_pb2.MsgRetornoPareamento(retorno=ret_val)

    def Busca(self, request, context):
        chave = request.chave
        quem_pediu = request.quem_pediu
        localizador = ""

        # 1. Busca local
        with self.lock:
            localizador = self.key_map.get(chave, "")

        if localizador:
            return central_pb2.MsgLocalizador(localizador=localizador)

        # 2. Se não achou localmente e for super-par, busca nos peers (Parte 3)
        if not localizador and self.is_super_peer:
            with self.lock:
                peers_to_search = list(self.peers)
            
            for peer_addr in peers_to_search:
                # Evita alagamento de volta para quem pediu
                if peer_addr == quem_pediu:
                    continue
                
                try:
                    with grpc.insecure_channel(peer_addr) as channel:
                        stub = central_pb2_grpc.CentralStub(channel)
                        # Passa o *meu* endereço para o próximo peer saber quem pediu
                        req = central_pb2.MsgBusca(chave=chave, quem_pediu=self.my_addr)
                        response = stub.Busca(req)
                        
                        if response.localizador:
                            return response # Achou!
                except grpc.RpcError:
                    # Ignora peers inalcançáveis e continua a busca
                    pass

        # Não encontrado em nenhum lugar
        return central_pb2.MsgLocalizador(localizador="")

    def Termino(self, request, context):
        with self.lock:
            key_count = len(self.key_map)

        def stop_server():
            time.sleep(0.1)
            if self.server_ref:
                self.server_ref.stop(0)

        threading.Thread(target=stop_server).start()
        return central_pb2.MsgRetornoTermino(retorno=key_count)

def serve():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Uso: ./svc_cen.py <porta> [modo_super_par]")
        sys.exit(1)

    port = sys.argv[1]
    is_super_peer = False
    
    # Comportamento da Parte 3 é ativado com 2 argumentos
    if len(sys.argv) == 3:
        is_super_peer = True

    server_addr = f'0.0.0.0:{port}'
    my_addr = f"{socket.getfqdn()}:{port}"
    
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    servicer = CentralServicer(port, is_super_peer, my_addr)
    servicer.server_ref = server

    central_pb2_grpc.add_CentralServicer_to_server(servicer, server)
    
    server.add_insecure_port(server_addr)
    server.start()
    server.wait_for_termination()

if __name__ == '__main__':
    serve()