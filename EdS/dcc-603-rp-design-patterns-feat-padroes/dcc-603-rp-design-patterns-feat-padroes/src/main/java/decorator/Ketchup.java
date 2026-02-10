package decorator;

public class Ketchup extends IngredienteDecorator {

    // Construtor sem parâmetros (testa valor isolado)
    public Ketchup() {
        super(null);
    }

    // Construtor que recebe outro Ingrediente
    public Ketchup(Ingrediente ingrediente) {
        super(ingrediente);
    }

    @Override
    public String imprimeIngrediente() {
        String resultado = "Ketchup";
        if (ingrediente != null) {
            resultado += ", " + ingrediente.imprimeIngrediente();
        }
        return resultado;
    }

    @Override
    public double valorDoIngrediente() {
        double valor = 1.00;
        if (ingrediente != null) {
            valor += ingrediente.valorDoIngrediente();
        }
        return valor;
    }
}
