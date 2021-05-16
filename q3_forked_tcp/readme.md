# Questão 3

Crie um código TCP, no qual o servidor consiga atender mais de cliente ao mesmo tempo, para promover diálogo bidirecional (half-duplex, ou seja, enquanto um dos lados fala o outro escuta e vice-versa, numa ordem pré-definida), usando a system call fork(). Nos testes de validação, é importante que vários clientes possam dialogar com o servidor, mas não é necessário que clientes dialoguem entre si.

# Compilar:

```
gcc server.c -o build/server -lm -Wall
gcc client.c -o build/client -lm -Wall
```

Também é possível compilar utilizando *make all*. Os binários estarão na pasta *build*.

# Execução:

- Executar *./buid/server* e depois *./build/client*
- Ao iniciar a conexão o cliente espera o usuário digitar uma mensagem que será enviada para o servidor
- Após isso servidor irá responder com a contagem de caracteres da mensagem
- O cliente ao mensagem do servidor irá exibi-la e então poderá enviar uma nova, repetindo estes ultimos passos
- O cliente é o unico que pode encerrar a conexão digitanto 'quit' (ou encerrando o programa)
- O servidor lida com mais de um cliente por vez, criando um processo por meio de fork para lidar cada cliente
- OBS: O servidor está configurado para escutar na porta 5000