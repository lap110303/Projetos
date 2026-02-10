package decorator;

public class Bacon extends IngredienteDecorator {

    public Bacon() {
        super(null);
    }

    public Bacon(Ingrediente ingrediente) {
        super(ingrediente);
    }

    @Override
    public String imprimeIngrediente() {
        String resultado = "Bacon";
        if (ingrediente != null) {
            resultado += ", " + ingrediente.imprimeIngrediente();
        }
        return resultado;
    }

    @Override
    public double valorDoIngrediente() {
        double valor = 1.50;
        if (ingrediente != null) {
            valor += ingrediente.valorDoIngrediente();
        }
        return valor;
    }
}
