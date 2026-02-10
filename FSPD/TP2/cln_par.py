#!/usr/bin/env python3

import sys
import grpc

# Importar stubs gerados
import pares_pb2
import pares_pb2_grpc

def run_client():
    if len(sys.argv) != 2:
        print("Uso: ./cln_par.py <localizador_serv_pares>")
        sys.exit(1)

    server_addr = sys.argv[1]

    try:
        with grpc.insecure_channel(server_addr) as channel:
            stub = pares_pb2_grpc.ParesStub(channel)

            for line in sys.stdin:
                line = line.strip()
                
                # Ignorar linhas vazias ou comentários
                if not line or line.startswith('#'):
                    continue

                parts = line.split(' ', 2)
                cmd = parts[0]

                try:
                    if cmd == 'I':
                        if len(parts) < 3: continue # Ignora mal formado
                        chave = int(parts[1])
                        valor = parts[2]
                        response = stub.Insercao(pares_pb2.MsgInsercao(chave=chave, valor=valor))
                        print(response.retorno)
                    
                    elif cmd == 'C':
                        if len(parts) < 2: continue # Ignora mal formado
                        chave = int(parts[1])
                        response = stub.Consulta(pares_pb2.MsgConsulta(chave=chave))
                        print(response.valor)
                    
                    elif cmd == 'A':
                        response = stub.Ativacao(pares_pb2.MsgVazia())
                        print(response.retorno)
                    
                    elif cmd == 'T':
                        response = stub.Termino(pares_pb2.MsgVazia())
                        print(response.retorno)
                        break # Termina o cliente após o comando T
                    
                    # Outros comandos são ignorados
                
                except grpc.RpcError as e:
                    print(f"Erro de RPC: {e.details()}", file=sys.stderr)
                except ValueError:
                    # Ignora erros de conversão (ex: 'I' sem chave válida)
                    pass

    except grpc.RpcError as e:
        print(f"Erro ao conectar ao servidor: {e.details()}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    run_client()