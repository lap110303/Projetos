module tb_stream_cypher;

    reg [7:0] message;                // Registrador para armazenar a mensagem
    reg [7:0] otp;                    // Registrador para armazenar o One-Time Pad
    wire [7:0] cypher_text;           // Saída do texto cifrado
    wire [7:0] decrypted_message;     // Saída para o texto decifrado

    // Módulo do stream cypher
    stream_cypher uut (
        .message(message),
        .otp(otp),
        .cypher_text(cypher_text),
        .decrypted_message(decrypted_message)
    );

    // Casos de teste
    initial begin

        // Dados de simulação
        $dumpfile("stream_cypher_tb.vcd");
        $dumpvars(0, tb_stream_cypher);

        // Caso de teste 1
        message = 8'b10101010;  // Mensagem: 0xAA
        otp = 8'b11001100;      // OTP: 0xCC
        #10;                    // Espera 10 unidades de tempo

        // Caso de teste 2
        message = 8'b11110000;  // Mensagem: 0xF0
        otp = 8'b00110011;      // OTP: 0x33
        #10;                    // Espera 10 unidades de tempo

        // Caso de teste 3
        message = 8'b00001111;  // Mensagem: 0x0F
        otp = 8'b11111111;      // OTP: 0xFF
        #10;                    // Espera 10 unidades de tempo

        // Caso de teste 4
        message = 8'b01010101;  // Mensagem: 0x55
        otp = 8'b10101010;      // OTP: 0xAA
        #10;                    // Espera 10 unidades de tempo

        // Caso de teste 5
        message = 8'b11111111;  // Mensagem: 0xFF
        otp = 8'b00000000;      // OTP: 0x00
        #10;                    // Espera 10 unidades de tempo

        // Termina a simulação
        $finish;
    end

endmodule
