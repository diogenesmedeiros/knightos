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

> ⚠️ **Importante:**  
> É necessário criar um arquivo de disco chamado disk.img, que será usado para armazenar arquivos, diretórios e setores do sistema de arquivos.
> Você pode criá-lo com o seguinte comando:
> ```bash
> qemu-img create -f raw disk.img <SIZE_DISK>
> ```

# Contribuições
Contribuições são muito bem-vindas! Se você deseja ajudar a melhorar o KnightOS, siga estas etapas:

1. *Fork* este repositório para sua conta pessoal.
2. Crie uma branch para sua feature ou correção de bug:
```bash
git checkout -b minha-feature
```
3. Faça suas modificações e commit:
```bash
git commit -m "Descrição clara do que foi alterado"
```
4. Envie suas alterações para seu fork:
```bash
git push origin minha-feature
```
5. Abra um *Pull Request* para este repositório principal descrevendo as mudanças e o motivo delas.

# Regras para contribuições
- Por favor, mantenha o código legível e comentado.
- Certifique-se de que seu código está testado e funcionando.
- Siga o padrão de estilo do projeto.
- Antes de abrir um PR, verifique se não há issues relacionadas ou PRs abertos para o mesmo problema/feature.

#
Assim, todos podem colaborar e ajudar a tornar o projeto melhor!
#