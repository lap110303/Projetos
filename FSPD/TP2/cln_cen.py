#!/usr/bin/env python3

import sys
import grpc

# Importar stubs gerados
import central_pb2
import central_pb2_grpc
import pares_pb2         # Necessário para o comando 'B'
import pares_pb2_grpc    # Necessário para o comando 'B'

def run_client():
    if len(sys.argv) != 2:
        print("Uso: ./cln_cen.py <localizador_serv_central>")
        sys.exit(1)

    central_addr = sys.argv[1]

    try:
        # Canal persistente com o servidor central
        with grpc.insecure_channel(central_addr) as central_channel:
            central_stub = central_pb2_grpc.CentralStub(central_channel)

            for line in sys.stdin:
                line = line.strip()
                
                if not line or line.startswith('#'):
                    continue

                parts = line.split(' ', 1)
                cmd = parts[0]

                try:
                    if cmd == 'B':
                        if len(parts) < 2: continue
                        chave = int(parts[1])
                        
                        # 1. Busca no servidor central
                        # Passa "" como quem_pediu, indicando ser um cliente
                        req_busca = central_pb2.MsgBusca(chave=chave, quem_pediu="")
                        resp_busca = central_stub.Busca(req_busca)
                        
                        localizador_par = resp_busca.localizador

                        if not localizador_par:
                            # Não escreve nada se a chave não for encontrada
                            pass
                        else:
                            # 2. Conecta-se ao servidor de pares retornado
                            try:
                                with grpc.insecure_channel(localizador_par) as par_channel:
                                    par_stub = pares_pb2_grpc.ParesStub(par_channel)
                                    req_consulta = pares_pb2.MsgConsulta(chave=chave)
                                    resp_consulta = par_stub.Consulta(req_consulta)
                                    
                                    # Formato: localizador=valor
                                    print(f"{localizador_par}={resp_consulta.valor}")
                            except grpc.RpcError as e:
                                # Falha ao conectar no servidor de pares
                                print(f"{localizador_par}=ERRO", file=sys.stderr)

                    elif cmd == 'P':
                        if len(parts) < 2: continue
                        localizador_peer = parts[1]
                        req = central_pb2.MsgPareamento(localizador=localizador_peer)
                        response = central_stub.Pareamento(req)
                        print(response.retorno)
                    
                    elif cmd == 'T':
                        response = central_stub.Termino(central_pb2.MsgVazia())
                        print(response.retorno)
                        break # Termina o cliente

                except grpc.RpcError as e:
                    print(f"Erro de RPC: {e.details()}", file=sys.stderr)
                except ValueError:
                    pass
                    
    except grpc.RpcError as e:
        print(f"Erro ao conectar ao servidor central: {e.details()}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    run_client()