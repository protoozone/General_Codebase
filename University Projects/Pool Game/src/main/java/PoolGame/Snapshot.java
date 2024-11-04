package PoolGame;
import PoolGame.objects.Ball;

import java.lang.reflect.Array;
import java.util.ArrayList;

public class Snapshot {
    private ArrayList<Ball> balls;
    private int score;
    private int time;

    public Snapshot(ArrayList<Ball> balls, int score, int time) {
        ArrayList<Ball> balls2 = new ArrayList<Ball>();

        for (Ball ball : balls) {
            Ball ball2 = new Ball(ball.getColourString(), ball.getxPos()-50, ball.getyPos()-50, ball.getxVel(), ball.getyVel()
                    , ball.getMass(), ball.isCue(), ball.getStrategy());
            balls2.add(ball2);
        }

        this.balls = balls2;
        this.score = score;
        this.time = time;
    }

    /**
     * Getter method for the balls stored in this snapshot
     *
     * @return The balls stored in this snapshot
     */
    public ArrayList<Ball> getBalls() {
        return balls;
    }

    /**
     * Getter method for the score stored in this snapshot
     *
     * @return The score stored in this snapshot
     */
    public int getScore() {
        return score;
    }

    /**
     * Getter method for the time stored in this snapshot
     *
     * @return The time stored in this snapshot
     */
    public int getTime() {
        return this.time;
    }
}
