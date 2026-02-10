import observer.Subject;
import observer.Temperatura;
import observer.TermometroCelsius;
import observer.TermometroFahrenheit;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.lang.reflect.Field;
import java.util.List;

@DisplayName("Padrão Observer")
public class ObserverTest {

    @Test
    @DisplayName("Observers devem ser adicionados")
    public void testObserversAreAdded() throws NoSuchFieldException, IllegalAccessException {
        Temperatura sensorTemp = new Temperatura();
        sensorTemp.add(new TermometroCelsius());
        sensorTemp.add(new TermometroFahrenheit());

        Field observersField = Subject.class.getDeclaredField("observers");
        observersField.setAccessible(true);
        List<Temperatura> observers = (List<Temperatura>) observersField.get(sensorTemp);

        Assertions.assertEquals(2, observers.size());
    }

    @Test
    @DisplayName("Observers devem ser notificados")
    public void testObservesAreNotified() {
        String expectedOutput = "Temperatura em Celsius: 32.0" + System.getProperty("line.separator") +
                "Temperatura em Fahrenheit: 89.6" + System.getProperty("line.separator");
        PrintStream originalOut = System.out;
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        System.setOut(new PrintStream(bos));

        Temperatura sensorTemp = new Temperatura();
        sensorTemp.add(new TermometroCelsius());
        sensorTemp.add(new TermometroFahrenheit());
        sensorTemp.setTemperatura(32);

        Assertions.assertEquals(expectedOutput, bos.toString());
    }

}
