# MeuPrimeiroArquivo.asm
# Um programa simples com duas instruções básicas em Assembly RISC-V

.text         # Define que o que vem a seguir é código (instruções)
.globl main   # Declara a etiqueta 'main' como global (ponto de entrada do programa)

main:         # Início do programa principal
    # Primeira instrução básica: Carregar um valor em um registrador
    li t0, 10           # Carrega o número 10 no registrador t0 (t0 = 10)

    # Segunda instrução básica: Somar valores de registradores
    li t1, 25           # Carrega o número 25 no registrador t1 (t1 = 25)
    add t2, t0, t1      # Soma o valor de t0 com t1 e guarda em t2
                        # t2 = t0 + t1  =>  t2 = 10 + 25  =>  t2 = 35

    # Para tornar este programa completo e executável no RARS,
    # vamos adicionar as instruções para encerrá-lo corretamente.
    # Primeiro, vamos imprimir o resultado em t2 (opcional, mas útil para ver)
    mv a0, t2           # Move o valor de t2 para a0 (argumento para syscall de impressão)
    li a7, 1            # Código da syscall para "Print Integer" (Imprimir Inteiro)
    ecall               # Executa a syscall (imprime o valor em a0, que é 35)

    # Agora, encerrar o programa
    li a7, 10           # Código da syscall para "Exit" (Encerrar Programa)
    ecall               # Executa a syscall

# Fim do arquivo