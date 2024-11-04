package PoolGame.strategy;

public class BlackStrategy extends PocketStrategy {
    /** Creates a new Orange strategy. */
    public BlackStrategy() {
        this.lives = 3;
    }

    public void reset() {
        this.lives = 3;
    }
}