package app.controllers;

import app.model.DataPoint;
import app.model.DataPointList;
import app.readers.FormatReader;
import app.readers.ReaderFactory;
import app.readers.WebSites;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.chart.CategoryAxis;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Alert;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;
import sun.misc.FormattedFloatingDecimal;

import java.util.Date;

import java.io.IOException;
import java.math.BigDecimal;
import java.net.URL;



public class AppController {

    private Stage primaryStage;

    private Scene helloScene;
    private Scene graphScene;
    private Scene resultsScene;

    private String fileLocation;

    private WebSites.SupportedWebSites chosenWebsite;

    private DataPointList resultsList;

    public AppController(Stage stage){
        this.primaryStage = stage;
    }

    public void initialize(){
        try {
            primaryStage.setTitle("Stock Winner");
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(getClass().getClassLoader().getResource("views/FileOpenerView.fxml"));
            AnchorPane layout = loader.load();

            FileOpenerController controller = loader.getController();
            controller.initialize(this);
            helloScene = new Scene(layout);
            primaryStage.setScene(helloScene);
            primaryStage.show();
        }
        catch (IOException e){
            
        }
    }

    public void initGraphScene(String view){
        try {
            primaryStage.setTitle("Line Chart");
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(getClass().getClassLoader().getResource(view));
            AnchorPane layout = loader.load();

            GraphDialogController controller = loader.getController();
            controller.initialize(this, fileLocation, chosenWebsite, layout);
            graphScene = new Scene(layout);
            primaryStage.setScene(graphScene);
            primaryStage.show();
        }
        catch (IOException e){
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Warning");
            alert.setHeaderText("This format is not supported");
            alert.setContentText("Choose another file or source website!");
            alert.showAndWait();
        }
    }

    public void goBackToFirstWindow() {
        primaryStage.setScene(helloScene);
        primaryStage.show();
    }

    public void initResultsScene(String view) {

        try {
            primaryStage.setTitle("Strategy results");
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(getClass().getClassLoader().getResource(view));
            AnchorPane layout = loader.load();

            ResultsController controller = loader.getController();
            controller.initialize(this, this.resultsList, layout);
            resultsScene = new Scene(layout);
            primaryStage.setScene(resultsScene);
            primaryStage.show();
        }
        catch (IOException e) {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Warning");
            alert.setHeaderText("Error");
            alert.setContentText("Error during UI loading");
            alert.showAndWait();
        }
    }

    public void goBackToGraphScene() {
        primaryStage.setScene(graphScene);
        primaryStage.show();
    }

    public Stage getPrimaryStage(){
        return this.primaryStage;
    }

    public void setFileLocation(String location){
        this.fileLocation = location;
    }

    public void setChosenWebsite(WebSites.SupportedWebSites chosenWebsite) {
        this.chosenWebsite = chosenWebsite;
    }

    public void setResultsList(DataPointList list) {
        this.resultsList = list;
    }
}
