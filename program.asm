# MeuPrimeiroArquivo.asm
# Um programa simples com duas instru��es b�sicas em Assembly RISC-V

.text         # Define que o que vem a seguir � c�digo (instru��es)
.globl main   # Declara a etiqueta 'main' como global (ponto de entrada do programa)

main:         # In�cio do programa principal
    # Primeira instru��o b�sica: Carregar um valor em um registrador
    li t0, 10           # Carrega o n�mero 10 no registrador t0 (t0 = 10)

    # Segunda instru��o b�sica: Somar valores de registradores
    li t1, 25           # Carrega o n�mero 25 no registrador t1 (t1 = 25)
    add t2, t0, t1      # Soma o valor de t0 com t1 e guarda em t2
                        # t2 = t0 + t1  =>  t2 = 10 + 25  =>  t2 = 35

    # Para tornar este programa completo e execut�vel no RARS,
    # vamos adicionar as instru��es para encerr�-lo corretamente.
    # Primeiro, vamos imprimir o resultado em t2 (opcional, mas �til para ver)
    mv a0, t2           # Move o valor de t2 para a0 (argumento para syscall de impress�o)
    li a7, 1            # C�digo da syscall para "Print Integer" (Imprimir Inteiro)
    ecall               # Executa a syscall (imprime o valor em a0, que � 35)

    # Agora, encerrar o programa
    li a7, 10           # C�digo da syscall para "Exit" (Encerrar Programa)
    ecall               # Executa a syscall

# Fim do arquivo