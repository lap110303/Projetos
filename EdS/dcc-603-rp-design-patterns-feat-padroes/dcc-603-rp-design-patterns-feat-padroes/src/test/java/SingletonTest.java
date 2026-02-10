import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import singleton.DBConnection;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

@DisplayName("Padrão Singleton")
public class SingletonTest {

    @Test()
    @DisplayName("Construtor deve ser privado")
    public void testNoPublicConstructor() throws NoSuchMethodException {
        Constructor<DBConnection> constructor = DBConnection.class.getDeclaredConstructor(String.class);

        assertTrue(Modifier.isPrivate(constructor.getModifiers()));
    }

    @Test()
    @DisplayName("Deve possuir método getInstance(String)")
    public void testHasGetInstance() {
        assertDoesNotThrow(() -> DBConnection.class.getDeclaredMethod("getInstance", String.class));
    }

    @Test
    @DisplayName("Duas instâncias distintas devem ser iguais")
    public void testHasSameInstance() throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Method getInstance = DBConnection.class.getDeclaredMethod("getInstance", String.class);
        getInstance.setAccessible(true);

        DBConnection db1 = (DBConnection) getInstance.invoke(null, "jdbc:as400://myiSeries;proxy server=myHODServer:3470");
        DBConnection db2 = (DBConnection) getInstance.invoke(null, "jdbc:as400://myiSeries;proxy server=myHODServer:3470");

        assertEquals(db1, db2);
    }

}
