import decorator.Ingrediente;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import java.lang.reflect.InvocationTargetException;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

@DisplayName("Padrão Decorator")
public class DecoratorTest {

    @Test
    @DisplayName("Deve ter os quatro ingredientes")
    public void testHasAllIngredients() {
        assertDoesNotThrow(() -> Class.forName("decorator.Ketchup"), "Sem classe Ketchup");
        assertDoesNotThrow(() -> Class.forName("decorator.Bacon"), "Sem classe Bacon");
        assertDoesNotThrow(() -> Class.forName("decorator.Queijo"), "Sem classe Queijo");
        assertDoesNotThrow(() -> Class.forName("decorator.Salada"), "Sem classe Salada");
    }

    @Test
    @DisplayName("Ingredientes devem ter o preço certo")
    public void testHasCorrectPrices() throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException, InstantiationException {
        Object ketchup = Class.forName("decorator.Ketchup").getDeclaredConstructor().newInstance();
        Object bacon = Class.forName("decorator.Bacon").getDeclaredConstructor().newInstance();
        Object queijo = Class.forName("decorator.Queijo").getDeclaredConstructor().newInstance();
        Object salada = Class.forName("decorator.Salada").getDeclaredConstructor().newInstance();

        assertEquals(1.00, Ingrediente.class.getDeclaredMethod("valorDoIngrediente").invoke(ketchup));
        assertEquals(1.50, Ingrediente.class.getDeclaredMethod("valorDoIngrediente").invoke(bacon));
        assertEquals(0.75, Ingrediente.class.getDeclaredMethod("valorDoIngrediente").invoke(queijo));
        assertEquals(0.25, Ingrediente.class.getDeclaredMethod("valorDoIngrediente").invoke(salada));
    }

    @Test
    @DisplayName("Ingredientes devem poder ser combinados")
    public void testIngredientsCombination() throws ClassNotFoundException, NoSuchMethodException, InstantiationException, IllegalAccessException, InvocationTargetException {
        Class ketchupClass = Class.forName("decorator.Ketchup");
        Class baconClass = Class.forName("decorator.Bacon");
        Class queijoClass = Class.forName("decorator.Queijo");
        Class saladaClass = Class.forName("decorator.Salada");
        Class paoClass = Class.forName("decorator.Pao");

        Object pao = paoClass.getDeclaredConstructor().newInstance();
        Object paoBacon = baconClass.getDeclaredConstructor(Ingrediente.class).newInstance(pao);
        Object paoBaconQueijo = queijoClass.getDeclaredConstructor(Ingrediente.class).newInstance(paoBacon);
        Object paoBaconQueijoSalada = saladaClass.getDeclaredConstructor(Ingrediente.class).newInstance(paoBaconQueijo);
        Object all = ketchupClass.getDeclaredConstructor(Ingrediente.class).newInstance(paoBaconQueijoSalada);

        assertEquals(5.0, Ingrediente.class.getDeclaredMethod("valorDoIngrediente").invoke(all));
        assertEquals("Ketchup, Salada, Queijo, Bacon, Pão",
                Ingrediente.class.getDeclaredMethod("imprimeIngrediente").invoke(all));
    }

}
