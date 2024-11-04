package PoolGame;

import PoolGame.config.*;

import java.util.List;

import javafx.application.Application;
import javafx.stage.Stage;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.Scene;
import javafx.scene.input.MouseEvent;
import javafx.scene.Group;

import javafx.event.EventHandler;

/** Main application entry point. */
public class App extends Application {
    /**
     * @param args First argument is the path to the config file
     */
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    /**
     * Starts the application.
     * 
     * @param primaryStage The primary stage for the application.
     */
    public void start(Stage primaryStage) {
        //Producing buttons for difficulties
        Group root = new Group();
        Button button = new Button("Easy");
        Button button2 = new Button("Medium");
        Button button3 = new Button("Hard");

        //Adding a label for context
        Label label = new Label("Please select a difficulty:");

        //Button sizings...
        button.setMaxHeight(200);
        button.setMaxWidth(200);
        button.setMinSize(200, 200);
        button.setTranslateY(100);

        button2.setMaxHeight(200);
        button2.setMaxWidth(200);
        button2.setMinSize(200, 200);
        button2.setTranslateX(200);
        button2.setTranslateY(100);

        button3.setMaxHeight(200);
        button3.setMaxWidth(200);
        button3.setMinSize(200, 200);
        button3.setTranslateX(400);
        button3.setTranslateY(100);

        label.setTranslateX(220);
        label.setTranslateY(50);
        label.resize(200, 50);

        //Adding buttons
        root.getChildren().add(button);
        root.getChildren().add(button2);
        root.getChildren().add(button3);
        root.getChildren().add(label);

        //Using a string array to store a temp variable in a final variable
        final String[] path = {""};

        //Mouse events are used to redirect into gameplay flows for difficulties
        button.onMouseClickedProperty().set(new EventHandler<MouseEvent>()
        {
            @Override
            public void handle(MouseEvent arg0)
            {
                path[0] = "src/main/resources/config_easy.json";

                // READ IN CONFIG
                GameManager gameManager = new GameManager();

                List<String> args = getParameters().getRaw();
                String configPath = path[0];
                System.out.println(configPath);

                ReaderFactory tableFactory = new TableReaderFactory();
                Reader tableReader = tableFactory.buildReader();
                tableReader.parse(configPath, gameManager);

                ReaderFactory ballFactory = new BallReaderFactory();
                Reader ballReader = ballFactory.buildReader();
                ballReader.parse(configPath, gameManager);
                gameManager.buildManager();

                // START GAME MANAGER
                gameManager.run();
                primaryStage.setTitle("Pool");
                primaryStage.setScene(gameManager.getScene());
                primaryStage.show();
                gameManager.run();
            }
        });

        button2.onMouseClickedProperty().set(new EventHandler<MouseEvent>()
        {
            @Override
            public void handle(MouseEvent arg0)
            {
                path[0] = "src/main/resources/config_normal.json";

                // READ IN CONFIG
                GameManager gameManager = new GameManager();

                List<String> args = getParameters().getRaw();
                String configPath = path[0];

                ReaderFactory tableFactory = new TableReaderFactory();
                Reader tableReader = tableFactory.buildReader();
                tableReader.parse(configPath, gameManager);

                ReaderFactory ballFactory = new BallReaderFactory();
                Reader ballReader = ballFactory.buildReader();
                ballReader.parse(configPath, gameManager);
                gameManager.buildManager();

                // START GAME MANAGER
                gameManager.run();
                primaryStage.setTitle("Pool");
                primaryStage.setScene(gameManager.getScene());
                primaryStage.show();
                gameManager.run();
            }
        });

        button3.onMouseClickedProperty().set(new EventHandler<MouseEvent>()
        {
            @Override
            public void handle(MouseEvent arg0)
            {
                path[0] = "src/main/resources/config_hard.json";

                // READ IN CONFIG
                GameManager gameManager = new GameManager();

                List<String> args = getParameters().getRaw();
                String configPath = path[0];

                ReaderFactory tableFactory = new TableReaderFactory();
                Reader tableReader = tableFactory.buildReader();
                tableReader.parse(configPath, gameManager);

                ReaderFactory ballFactory = new BallReaderFactory();
                Reader ballReader = ballFactory.buildReader();
                ballReader.parse(configPath, gameManager);
                gameManager.buildManager();

                // START GAME MANAGER
                gameManager.run();
                primaryStage.setTitle("Pool");
                primaryStage.setScene(gameManager.getScene());
                primaryStage.show();
                gameManager.run();
            }
        });

        //And of course, set the scenes
        Scene newScene = new Scene(root);
        primaryStage.setScene(newScene);
        primaryStage.show();
    }
}
