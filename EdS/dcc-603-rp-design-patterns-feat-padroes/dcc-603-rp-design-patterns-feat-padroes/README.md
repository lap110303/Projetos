# Roteiro Prático: Padrões de Projeto

Professor: João Eduardo Montandon

**Objetivo: Nesta prática, você irá experimentar o uso dos principais padrões de projeto existentes.**

## Requisitos

- Git SCM (https://git-scm.com/)
- IntelliJ IDEA Community Edition (https://www.jetbrains.com/idea/download)
  - Essa IDE é uma **recomendação**, apenas. O projeto foi construído utilizando a sua estrutura. Fique a vontade para utilizar outra, caso queira.

## Entrega

O roteiro deverá ser entregue utilizando o mecanismo de pull-request.

1. Faça o fork do projeto para a sua conta (https://github.com/jemaf/dcc-603-rp-design-patterns/fork)
2. Implemente sua solução no fork **associado a sua conta**.
   - Não se esqueça de fazer os commits a medida em que implementar as tarefas pendentes.
3. Certifique-se de que você realizou o push com todos os commits.
4. Realize o Pull Request do seu projeto. Para isso, acesse o repositório original, e clique no botão "Compare & Pull Request", presente no banner amarelo do topo da página.
   - Para mais informações veja: https://docs.github.com/pt/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork

## Avaliação

- Cada implementação de padrão será avaliada em 1 ponto.
  - Total se sua implementação passar todos os testes.
  - 50% se sua implementação passar por pela metade de casos de teste.
  - 25% se sua implementação passar por pelo menos um caso de teste.


## Padrão Singleton

**Classes presentes no pacote `singleton`**

O padrão de projeto Singleton é utilizado como forma de disponibilizar apenas uma instância de uma determinada classe.
Esse tipo de comportamento é especialmente útil em situações onde um determinado recurso a ser obtido deve ser compartilhado em diferentes partes da aplicação, como gerenciadores de logging.

Altere a classe `singleton.DBConnection` para que sua instância seja disponibilizada por meio de um singleton.

## Padrão Decorator

**Classes presentes no pacote `decorator`**

A equipe de TI de um aplicativo de gerenciamento de entrega de alimentos entrou em contato requisitando seus serviços.
Eles desejam implementar uma funcionalidade para permitir aos seus usuários incluir os mais variados ingredientes em seus pedidos.
Na solução implementada atualmente, os projetistas criaram uma classe geral denominada `Ingrediente`, e então derivaram uma série de subclasses especificando cada ingrediente de forma concreta, tais como `Ketchup`, `Bacon`, `BancoEKetchup`, etc.
Rapidamente você observou que esse problema é grave, pois o número de classes aumentaria vertiginosamente de acordo com a combinação dos acréscimos.

Ao analisar o problema mais de perto, você percebeu que ele poderia ser tratado pelo padrão decorador.
Implemente uma demonstração desse padrão para os membros da equipe de TI com base no modelo já existente no pacote `decorator`.
Essa implementação deverá conter as quatro classes de acréscimo: `Ketchup`, `Bacon`, `Queijo`, e `Salada`.
Ainda, as classes deverão reimplementar os métodos `imprimeIngrediente` e `valorDoIngrediente`, de acordo com as seguintes informações:
* Ingredientes deverão ser impressos separados por vírgula
* Os ingredientes possuem o seguinte preço: Ketchup custa 1.00, Bacon custa 1.50, Queijo custa 0.75, e Salada custa 0.25

## Padrão Strategy

**Classes presentes no pacote `strategy`**

Você foi designado como responsável por desenvolver o módulo para validar senhas de uma biblioteca externa que é mantida pela empresa onde trabalha.
Por se tratar de uma biblioteca que será exposta à comunidade, é de preocupação dos projetistas que o módulo de validação seja genérico o suficiente para aceitar os mais variados métodos de validação, tais como a presença de caracteres alpha-numéricos, tamanho, etc.
Ainda, o módulo deve respeitar o princípio de aberto/fechado para proteger a biblioteca de acessos e modificações indevidas.

Ao estudar o problema, você chegou a conclusão de que essa validação pode ser feita utilizando o padrão strategy.
Implemente este padrão com base nos arquivos já existentes no pacote `strategy`.
Seu padrão deverá suportar três modos diferentes de autenticação:
1. `LengthStrategy`, senhas devem ter pelo menos 8 caracteres de tamanho
2. `UpperCaseStrategy`, senhas devem ter pelo menos uma letra maiúscula
3. `SpecialCharStrategy`, senhas devem ter pelo menos um caractere especial

Para isso, suas classes de validação deverão herdar a classe abstrata `PasswordStrategy`. 
Você ainda deverá alterar a classe `PasswordValidator` para que ela faça a verificação das estratégias criadas anteriormente.
Essa classe deverá implementar um `setStrategy(PasswordStrategy)` para definir a estratégia de validação atual e um método `validate` para chamar a validação da estratégia definida anteriormente.

## Padrão Observer

**Classes presentes no pacote `observer`**

Dado o código do padrão observer presente no pacote `observer`, implemente os trechos que estão faltando.