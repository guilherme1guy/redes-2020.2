# Questão 3

O aluno deve pesquisar a system call select() e produzir novo código TCP, de modo a implementar um código TCP no qual os clientes se conectem ao servidor TCP e consigam dialogar entre si, num diálogo full-duplex, ou seja, os dois lados podem iniciar o diálogo, sem uma ordem pré-definida. Nesse caso, tudo o que um usuário digitar deve chegar aos pares conectados ao servidor TCP naquele momento. Obs.: Uma referência sobre essa system call pode ser encontrada em https://man7.org/linux/manpages/man2/select.2.html. 

# Compilar:

```
gcc server.c -o build/server -lm -Wall
gcc client.c -o build/client -lm -Wall
```

Também é possível compilar utilizando *make all*. Os binários estarão na pasta *build*.

# Execução:

- Executar *./buid/server* e depois até 64 instancias de *./build/client*
- Ao iniciar a conexão o cliente se conecta ao servidor e cria dois processos, um para receber o input do usuário e enviar ao servidor e outro processo para exibir mensagens recebidas do servidor
- Ao receber uma mensagem para servidor irá retrasmiti-la para todos os clientes conectados que não sejam o remetente da mensagem
- O cliente exibirá a mensagem na tela assim que a receber, uma limitação do código é que a interface não separa o espaço em que o usuário digita o seu texto e o espaço onde são imprimidas as mensagens do usuário, o que pode causar encavalamento na hora de exibir (entretanto o texto do usuário não é afetado pela mensagem do servidor)
- OBS: O servidor está configurado para escutar na porta 5000