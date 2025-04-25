module d_flip_flop (
    input wire D,    // Entradas do flip-flop D
    input wire clk,  // Entradas do clock
    input wire reset,// Entradas de reset
    output reg Q     // Saída do flip-flop D
);

    // Bloco sempre sensível à borda do clock ou do reset
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            Q <= 0;  // Se reset estiver ativo, zera a saída Q
        end else begin
            Q <= D;  // Se reset estiver desativado, a saída Q recebe o valor de D na borda do clock
        end
    end

endmodule
