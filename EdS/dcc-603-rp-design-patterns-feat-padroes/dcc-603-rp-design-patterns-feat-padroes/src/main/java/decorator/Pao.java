package decorator;

public class Pao extends IngredienteDecorator {


    public Pao(Ingrediente ingrediente) {
        super(ingrediente);
    }

    public Pao() {
        super(null);
    }

    @Override
    public String imprimeIngrediente() {
        String ingredientes = "Pão";

        if (super.ingrediente != null)
            ingredientes += ", " + super.ingrediente.imprimeIngrediente();

        return ingredientes;
    }

    @Override
    public double valorDoIngrediente() {

        double valor = 1.5;

        if (super.ingrediente != null)
            valor += ingrediente.valorDoIngrediente();

        return valor;
    }
}
