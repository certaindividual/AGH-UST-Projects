package app.base;

import app.controllers.AppController;
import javafx.application.Application;
import javafx.stage.Stage;

public class Main extends Application {

    private Stage primaryStage;

    private AppController appController;

    @Override
    public void start(Stage primaryStage){
        this.primaryStage = primaryStage;
        this.appController = new AppController(primaryStage);
        this.appController.initialize();
    }


    public static void main(String[] args) { launch(args); }

}
