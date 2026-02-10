package observer;

public class TermometroFahrenheit implements Observer {
    @Override
    public void update(Subject s) {
        double temperatura = ((Temperatura) s).getTemperatura() * 1.8 + 32;
        System.out.println("Temperatura em Fahrenheit: " + temperatura);
    }
}
