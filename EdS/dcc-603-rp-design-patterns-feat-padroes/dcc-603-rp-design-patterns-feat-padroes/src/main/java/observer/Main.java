package observer;

public class Main {

    public static void main(String[] args) {
        Temperatura sensorTemp = new Temperatura();

        sensorTemp.add(new TermometroCelsius());
        sensorTemp.add(new TermometroFahrenheit());

        sensorTemp.setTemperatura(32);
        System.out.println("=======================");
        sensorTemp.setTemperatura(45);
        System.out.println("=======================");
        sensorTemp.setTemperatura(100);
    }
}

