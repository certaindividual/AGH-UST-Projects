package app.controllers;

import app.exceptions.InvalidContentException;
import app.model.*;
import app.readers.*;
import app.support.ZoomManager;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.chart.CategoryAxis;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Alert;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;

import java.io.IOException;


public class ResultsController {

    private AppController appController;

    private DataPointList resultsList;

    @FXML
    private CategoryAxis xAxis;

    @FXML
    private NumberAxis yAxis;

    @FXML
    private LineChart<String, Number> lineChart;

    @FXML
    private void handleGoingBack(ActionEvent event){

        appController.goBackToGraphScene();
    }

    public void initialize(AppController controller, DataPointList resultsList, Pane chartParent) throws IOException  {

        this.appController=controller;
        this.resultsList = resultsList;

        xAxis.setLabel("Date");
        yAxis.setLabel("Value");

        XYChart.Series series = new XYChart.Series();
        series.setName("Portfolio value fluctuations");

        for(DataPoint dp: this.resultsList.getDataPoints()) {
            series.getData().add(new XYChart.Data(dp.getDate().toString(), dp.getPrice()));
        }
        new ZoomManager(chartParent, lineChart,series);

    }

}
