package decorator;

public class Main {

    public static void main(String[] args) {

        Ingrediente produto = new Pao(new Pao());

        System.out.println(produto.imprimeIngrediente());
        System.out.println(produto.valorDoIngrediente());

    }
}
