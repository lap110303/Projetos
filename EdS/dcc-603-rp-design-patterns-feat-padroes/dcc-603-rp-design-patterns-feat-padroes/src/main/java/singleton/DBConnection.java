package singleton;

/**
 * Implementação Singleton (lazy + thread-safe).
 * Apenas UMA instância de DBConnection existirá durante toda a aplicação.
 */
public final class DBConnection {

    /** Instância única — volatile garante visibilidade entre threads. */
    private static volatile DBConnection instance;

    /* String de conexão que a aplicação usa. */
    private final String connectionString;

    /* Construtor privado impede uso de new fora da classe. */
    private DBConnection(String connectionString) {
        this.connectionString = connectionString;
    }

    /**
     * Ponto de acesso global.
     *
     * @param connectionString string usada somente na primeira chamada;
     *                         chamadas subsequentes ignoram o parâmetro.
     */
    public static DBConnection getInstance(String connectionString) {
        if (instance == null) {                       // 1ª verificação (sem lock)
            synchronized (DBConnection.class) {
                if (instance == null) {               // 2ª verificação (com lock)
                    instance = new DBConnection(connectionString);
                }
            }
        }
        return instance;
    }

    /** Simula abertura da conexão. */
    public void connect() throws InterruptedException {
        System.out.println("Connecting to " + this.connectionString);
        Thread.sleep(1000);
        System.out.println("Connected!!");
    }
}
