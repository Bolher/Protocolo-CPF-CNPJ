import socket

def send_request(number, format):
    server_ip = '127.0.0.1'
    server_port = 80

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect((server_ip, server_port))
        request = f"{number},{format}"
        client_socket.send(request.encode())

        response = client_socket.recv(1024).decode()
        print("Resposta do servidor:")
        print(response)

    except Exception as e:
        print(f"Erro na comunicação com o servidor: {e}")

    finally:
        client_socket.close()

if __name__ == "__main__":
    number = input("Digite o número do documento: ")
    format = input("Digite o formato (0 para CPF, 1 para CNPJ): ")
    send_request(number, format)
