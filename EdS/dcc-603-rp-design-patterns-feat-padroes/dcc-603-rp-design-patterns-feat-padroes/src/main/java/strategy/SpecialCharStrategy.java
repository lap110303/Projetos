package strategy;

public class SpecialCharStrategy extends PasswordStrategy {
    @Override
    public boolean validate(String password) {
        if (password == null) return false;
        for (char c : password.toCharArray()) {
            if (!Character.isLetterOrDigit(c)) {
                return true;
            }
        }
        return false;
    }
}
