module stream_cypher (
    input wire [7:0] message,           // Entrada da mensagem
    input wire [7:0] otp,               // Entrada do One-Time Pad
    output wire [7:0] cypher_text,      // Saída do texto depois de ser cifrado
    output wire [7:0] decrypted_message // Saída da mensagem depois de ser decifrada
);

    // XOR utilizado para a cifragem
    assign cypher_text = message ^ otp;

    // XOR utilizado para a decifragem
    assign decrypted_message = cypher_text ^ otp;

endmodule
