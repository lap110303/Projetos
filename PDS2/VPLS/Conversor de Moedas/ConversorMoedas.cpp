#include "ConversorMoedas.hpp"
#include <algorithm>

void ConversorMoedas::definirTaxaCambio(std::string de, std::string para, double taxa) {
    if (taxa < 0) {
        throw cexc::valor_invalido();
    }

    if (!checarMoedaValida(de) || !checarMoedaValida(para)) {
        throw cexc::moeda_invalida();
    }

    std::string chave = de + para;
    taxasCambio[chave] = taxa;
}

bool ConversorMoedas::checarMoedaValida(std::string moeda) {
    return std::find(MOEDAS_VALIDAS.begin(), MOEDAS_VALIDAS.end(), moeda) != MOEDAS_VALIDAS.end();
}

double ConversorMoedas::converter(std::string de, std::string para, double valor) {
    if (valor < 0) {
        throw cexc::valor_invalido();
    }

    if (!checarMoedaValida(de) || !checarMoedaValida(para)) {
        throw cexc::moeda_invalida();
    }

    std::string chave = de + para;

    if (!possuiTaxaCambio(de, para)) {
        throw cexc::conversao_inexistente();
    }

    return valor * taxasCambio[chave];
}

bool ConversorMoedas::possuiTaxaCambio(std::string de, std::string para) {
    std::string chave = de + para;
    return taxasCambio.find(chave) != taxasCambio.end();
}

