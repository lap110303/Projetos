#include "doctest.h"
#include "ConversorMoedas.hpp"

TEST_CASE("ConversorMoedas - Moeda Inválida") {
    ConversorMoedas conversor;
    CHECK(conversor.checarMoedaValida("LAP") == false);
}

TEST_CASE("ConversorMoedas - Taxa Inválida") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.definirTaxaCambio("USD", "BRL", -0.15), cexc::valor_invalido);
}

TEST_CASE("ConversorMoedas - Conversão Inexistente") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.converter("BRL", "JPY", 5.0), cexc::conversao_inexistente);
}

TEST_CASE("ConversorMoedas - Moeda Válida") {
    ConversorMoedas conversor;
    CHECK(conversor.checarMoedaValida("GBP") == true);
}

TEST_CASE("ConversorMoedas - Taxa de Câmbio com Moeda Inválida") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.definirTaxaCambio("LAP", "EUR", 0.5), cexc::moeda_invalida);
}

TEST_CASE("ConversorMoedas - Taxa de Câmbio com Moeda Válida") {
    ConversorMoedas conversor;
    conversor.definirTaxaCambio("BRL", "AUD", 0.1);
    CHECK(conversor.possuiTaxaCambio("BRL", "AUD") == true);
}

TEST_CASE("ConversorMoedas - Conversão com Valor Inválido") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.converter("BRL", "EUR", -404.0), cexc::valor_invalido);
}

TEST_CASE("ConversorMoedas - Conversão com Moeda Inválida") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.converter("LAP", "INR", 60.0), cexc::moeda_invalida);
}

TEST_CASE("ConversorMoedas - Conversão com Taxa Inválida") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.converter("BRL", "CHF", 100.0), cexc::conversao_inexistente);
}

TEST_CASE("ConversorMoedas - Conversão com Valor Negativo") {
    ConversorMoedas conversor;
    CHECK_THROWS_AS(conversor.converter("USD", "EUR", -100.0), cexc::valor_invalido);
}
