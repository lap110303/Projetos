package decorator;

public class Queijo extends IngredienteDecorator {

    public Queijo() {
        super(null);
    }

    public Queijo(Ingrediente ingrediente) {
        super(ingrediente);
    }

    @Override
    public String imprimeIngrediente() {
        String resultado = "Queijo";
        if (ingrediente != null) {
            resultado += ", " + ingrediente.imprimeIngrediente();
        }
        return resultado;
    }

    @Override
    public double valorDoIngrediente() {
        double valor = 0.75;
        if (ingrediente != null) {
            valor += ingrediente.valorDoIngrediente();
        }
        return valor;
    }
}
