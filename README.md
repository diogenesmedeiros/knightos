# Introdução
Este projeto consiste na criação de um kernel para um sistema operacional simples, desenvolvido com o objetivo principal de entender, na prática, como funciona um sistema operacional desde seu núcleo.

# Dependências
Para compilar e executar este KnightOS, você precisa de um ambiente com ferramentas para desenvolvimento de sistemas operacionais, como:
- ```i686-elf-gcc:``` Compilador cross-compiling para arquitetura i686
- ```nasm:``` Montador para arquivos assembly
- ```ld:``` Linkador para gerar o executável do kernel
- ```grub-mkrescue:``` Ferramenta para criar a ISO inicializável com GRUB
- ```qemu:``` Emulador para testar o sistema operacional

# Instalação e Compilação
Clone o repositório e compile com o ```make```:

```bash
git clone https://github.com/diogenesmedeiros/knightos.git
cd knightos
make
```

Para executar a ISO em QEMU:
```bash
make run
```

Para limpar builds anteriores:
```bash
make clean
```

# Criação do Disco
> ⚠️ **Importante:**  
> É necessário criar um arquivo de disco chamado disk.img, que será usado para armazenar arquivos, diretórios e setores do sistema de arquivos.
> Você pode criá-lo com o seguinte comando:
```bash
qemu-img create -f raw disk.img 64M
```
Você pode ajustar o tamanho conforme sua necessidade (64M, 128M, etc.).

# Objetivo Didático
Este projeto é **educacional**, com foco em:
- Entendimento de como um kernel gerencia hardware e software.
- Implementação de comandos de terminal.
- Gerenciamento de memória, sistema de arquivos e processos.
- Escrita em setores diretos no disco via interface ATA.
