import org.junit.Before;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import strategy.PasswordStrategy;
import strategy.PasswordValidator;

import static org.junit.jupiter.api.Assertions.*;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

@DisplayName("Padrão Strategy")
public class StrategyTest {

    private Object lengthStrategy;
    private Object upperCaseStrategy;
    private Object specialCharStrategy;
    private Method validateStrategy;

    @BeforeEach
    public void beforeAll() throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, InstantiationException, IllegalAccessException {
        lengthStrategy = Class.forName("strategy.LengthStrategy").getDeclaredConstructor().newInstance();
        upperCaseStrategy = Class.forName("strategy.UpperCaseStrategy").getDeclaredConstructor().newInstance();
        specialCharStrategy = Class.forName("strategy.SpecialCharStrategy").getDeclaredConstructor().newInstance();
        validateStrategy = PasswordStrategy.class.getDeclaredMethod("validate", String.class);
    }

    @Test
    @DisplayName("Senhas devem ter pelo menos 8 caracteres")
    public void testShouldHaveAtLeast8Characters() throws InvocationTargetException, IllegalAccessException {
        assertTrue((Boolean) validateStrategy.invoke(lengthStrategy, "UmaSenhaGrande"));
        assertFalse((Boolean) validateStrategy.invoke(lengthStrategy, "senha"));
    }

    @Test
    @DisplayName("Senhas devem ter pelo menos uma letra minúscula")
    public void testShoulHaveAtLeastOneUpperCaseCharacter() throws InvocationTargetException, IllegalAccessException {
        assertTrue((Boolean) validateStrategy.invoke(upperCaseStrategy, "Umaletramaiuscula"));
        assertFalse((Boolean) validateStrategy.invoke(upperCaseStrategy, "nenhumaletramaiuscula"));
    }

    @Test
    @DisplayName("Senhas devem ter pelo menos um caractere especial")
    public void testShouldHaveAtLeastOneSpecialChar() throws InvocationTargetException, IllegalAccessException {
        assertTrue((Boolean) validateStrategy.invoke(specialCharStrategy, "$pecial"));
        assertFalse((Boolean) validateStrategy.invoke(specialCharStrategy, "special"));
    }

    @Test
    @DisplayName("Strategies devem ser verificadas no validator")
    public void testShouldVerifyAllStrategies() throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        PasswordValidator validator = new PasswordValidator();
        Method setStrategy = PasswordValidator.class.getDeclaredMethod("setStrategy", PasswordStrategy.class);
        Method validate = PasswordValidator.class.getDeclaredMethod("validate", String.class);

        setStrategy.invoke(validator, lengthStrategy);
        assertTrue((Boolean) validate.invoke(validator, "UmaSenhaGrande"));

        setStrategy.invoke(validator, upperCaseStrategy);
        assertTrue((Boolean) validate.invoke(validator, "Uma"));

        setStrategy.invoke(validator, specialCharStrategy);
        assertTrue((Boolean) validate.invoke(validator, "$pecial"));
    }

}
