package decorator;

public class Salada extends IngredienteDecorator {

    public Salada() {
        super(null);
    }

    public Salada(Ingrediente ingrediente) {
        super(ingrediente);
    }

    @Override
    public String imprimeIngrediente() {
        String resultado = "Salada";
        if (ingrediente != null) {
            resultado += ", " + ingrediente.imprimeIngrediente();
        }
        return resultado;
    }

    @Override
    public double valorDoIngrediente() {
        double valor = 0.25;
        if (ingrediente != null) {
            valor += ingrediente.valorDoIngrediente();
        }
        return valor;
    }
}
