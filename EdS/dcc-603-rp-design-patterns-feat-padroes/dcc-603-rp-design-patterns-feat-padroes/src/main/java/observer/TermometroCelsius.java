package observer;

public class TermometroCelsius  implements Observer {
    @Override
    public void update(Subject s) {
        double temperatura = ((Temperatura) s).getTemperatura();
        System.out.println("Temperatura em Celsius: " + temperatura);
    }
}
