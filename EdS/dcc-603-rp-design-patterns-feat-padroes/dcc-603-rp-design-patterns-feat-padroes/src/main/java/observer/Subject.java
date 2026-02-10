package observer;

import java.util.ArrayList;
import java.util.List;

public class Subject {

    private List<Observer> observers;

    public Subject() {
        this.observers = new ArrayList<>();
    }

    /** Adiciona um observador à lista. */
    public void add(Observer observer) {
        this.observers.add(observer);
    }

    /** Notifica todos os observadores chamando update(this). */
    public void notifyObservers() {
        for (Observer observer : observers) {
            observer.update(this);
        }
    }
}
