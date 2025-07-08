# Introdução
Este projeto consiste na criação de um Mini Kernel para um sistema operacional simples, desenvolvido com o objetivo principal de entender, na prática, como funciona um sistema operacional desde seu núcleo (kernel).

# Instalação
Para compilar e executar este Mini Kernel, você precisa de um ambiente com ferramentas para desenvolvimento de sistemas operacionais, como:
- Um compilador cross-compiler para ```i686-elf-gcc``` (compilador para arquitetura i686)
- ```ld``` linker compatível com a arquitetura
- ```grub-mkrescue``` para criar a imagem ISO inicializável
- ```qemu``` para rodar a ISO emulado

# Compilação
Para compilar o kernel e gerar a ISO inicializável, basta rodar:
```bash
make
```

Para executar a ISO em QEMU:
```bash
make run
```

Para limpar versões antigas:
```bash
make clean
```

O sistema compila os arquivos fonte, linka o kernel, gera a imagem ISO e executa o SO no emulador.