package observer;

public class Temperatura extends Subject {

    private double temperatura;

    public double getTemperatura() {
        return temperatura;
    }

    public void setTemperatura(double temperatura) {
        this.temperatura = temperatura;
        super.notifyObservers();
    }
}
