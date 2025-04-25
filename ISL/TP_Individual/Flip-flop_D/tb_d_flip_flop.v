module tb_d_flip_flop;

    reg D;         // Entrada D do flip-flop
    reg clk;       // Clock
    reg reset;     // Sinal de reset
    wire Q;        // Saída do Q do flip-flop

    // Módulo d_flip_flop
    d_flip_flop uut (
        .D(D),
        .clk(clk),
        .reset(reset),
        .Q(Q)
    );

    // Geração do clock
    initial begin
        clk = 0;                 // Inicializa o clock em 0
        forever #5 clk = ~clk;   // Inverte o clock a cada 5 unidades de tempo
    end

    // Casos de teste
    initial begin

        // Armazena os dados de simulação
        $dumpfile("d_flip_flop_tb.vcd");
        $dumpvars(0, tb_d_flip_flop);

        // Inicializa com reset ativo e entrada D em 0
        reset = 1;
        D = 0;
        #10;  // Espera 10 unidades de tempo

        // Desativa o reset e define D como 1
        reset = 0;
        D = 1;
        #10;  // Espera 10 unidades de tempo

        // Define D como 0
        D = 0;
        #10;  // Espera 10 unidades de tempo

        // Define D como 1
        D = 1;
        #10;  // Espera 10 unidades de tempo

        // Ativa o reset
        reset = 1;
        #10;  // Espera 10 unidades de tempo

        // Desativa o reset e define D como 0
        reset = 0;
        D = 0;
        #10;  // Espera 10 unidades de tempo

        // Termina a simulação
        $finish;
    end

endmodule
