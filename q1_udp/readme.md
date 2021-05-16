# Questão 1

Crie uma aplicação UDP simples, cujo fluxo de diálogo entre Cliente e Servidor seja bidirecional. Ou seja, os dois lados (cliente e servidor) enviam e recebem mensagens de um para o outro.

# Compilar:

```
gcc server.c -o build/server -lm -Wall
gcc client.c -o build/client -lm -Wall
```

Também é possível compilar utilizando *make all*. Os binários estarão na pasta *build*.

# Execução:

- Executar *./buid/server* e depois *./build/client*
- OBS: O servidor está configurado para escutar na porta 5000