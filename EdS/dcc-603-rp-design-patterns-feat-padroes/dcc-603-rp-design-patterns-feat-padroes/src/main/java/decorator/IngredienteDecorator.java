package decorator;

public abstract class IngredienteDecorator implements Ingrediente {

    protected Ingrediente ingrediente;

    public IngredienteDecorator(Ingrediente ingrediente) {
        this.ingrediente = ingrediente;
    }

    @Override
    public String imprimeIngrediente() {
        return this.ingrediente.imprimeIngrediente();
    }

    @Override
    public double valorDoIngrediente() {
        return this.ingrediente.valorDoIngrediente();
    }
}
