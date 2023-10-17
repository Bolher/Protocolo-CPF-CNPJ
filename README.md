# Protocolo de Validação de Documentos - Documentação

Este é um exemplo de um servidor e cliente que implementam um protocolo de validação de documentos usando TCP. O servidor valida números de CPF ou CNPJ informados pelo cliente e responde com um código de validação. O servidor é escrito em C e o cliente em Python.

## Servidor (Código em C)

### Pré-requisitos

- Windows (Winsock)
- Compilador C (como MinGW)
- Winsock2 Library

### Funcionalidade

- O servidor escuta na porta 80.
- Aceita conexões de clientes e gerencia até 10 conexões simultâneas.
- Recebe solicitações de clientes contendo um número e um formato (0 para CPF, 1 para CNPJ).
- Valida o documento de acordo com o formato e responde com um código de validação (0 = válido, 1 = inválido, 2 = tamanho inválido, 3 = formato inválido).

### Validação de Documentos

- A validação de CPF e CNPJ é realizada com base nas regras oficiais.
- Os documentos devem estar no formato adequado (CPF com 11 dígitos e CNPJ com 14 dígitos).
- Os códigos de validação são retornados de acordo com a validação realizada.

## Cliente (Código em Python)

### Pré-requisitos

- Python 3

### Funcionalidade

- O cliente Python permite que o usuário insira um número de documento e um formato (CPF ou CNPJ).
- Ele envia uma solicitação para o servidor em execução na porta 80.
- Exibe a resposta do servidor.

## Uso

1. Compile o servidor conforme as instruções fornecidas.
2. Execute o servidor.
3. Execute o cliente Python.
4. Insira o número de documento e o formato quando solicitado.
5. O cliente enviará a solicitação ao servidor, que responderá com o resultado da validação.

Lembre-se de manter o código do servidor em execução enquanto testa o cliente.
