# Questão 2

Crie uma aplicação TCP, para diálogo cliente/servidor, para diálogo bidirecional, cujo lado servidor permaneça com a conexão ativa até que o cliente informe ao servidor que não há mais nada a enviar; quando isso ocorrer, apenas o cliente encerra a conexão, mas o servidor deve continuar ativo, aguardando novas conexões.

# Compilar:

```
gcc server.c -o build/server -lm -Wall
gcc client.c -o build/client -lm -Wall
```

Também é possível compilar utilizando *make all*. Os binários estarão na pasta *build*.

# Execução:

- Executar *./buid/server* e depois *./build/client*
- Ao iniciar a conexão o cliente espera o usuário digitar uma mensagem que será enviada para o servidor
- Após isso o usuário deve digitar uma mensagen no servidor para responder ao cliente.
- O cliente receberá a mensagem do servidor e poderá enviar uma nova, repetindo estes ultimos passos
- O cliente é o unico que pode encerrar a conexão digitanto 'quit'. Caso o servidor digite 
'quit' será considerado como uma mensagem ao cliente
- Após um cliente sair o servidor ficará esperando por um novo cliente (ou irá começar a processar o próximo, caso esteja na fila)
- OBS: O servidor está configurado para escutar na porta 5000