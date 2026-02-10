package strategy;

public class PasswordValidator {

    private PasswordStrategy strategy;

    /** Construtor padrão (sem estratégia definida). */
    public PasswordValidator() { }

    /** Define a estratégia de validação. */
    public void setStrategy(PasswordStrategy strategy) {
        this.strategy = strategy;
    }

    /** Valida usando a estratégia corrente. */
    public boolean validate(String password) {
        if (strategy == null) {
            throw new IllegalStateException("Nenhuma estratégia definida");
        }
        return strategy.validate(password);
    }
}
